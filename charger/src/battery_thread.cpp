/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "battery_thread.h"
#include <cerrno>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <linux/netlink.h>
#include "hdf_base.h"
#include "charger_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr int32_t UEVENT_BUFF_SIZE = (64 * 1024);
constexpr int32_t UEVENT_RESERVED_SIZE = 2;
constexpr int32_t UEVENT_MSG_LEN = (2 * 1024);
constexpr int32_t TIMER_FAST_SEC = 2;
constexpr int32_t SEC_TO_MSEC = 1000;
const std::string POWER_SUPPLY = "SUBSYSTEM=power_supply";
}

int32_t BatteryThread::OpenUeventSocket()
{
    int32_t bufferSize = UEVENT_BUFF_SIZE;
    struct sockaddr_nl address = {
        .nl_family = AF_NETLINK,
        .nl_pid = getpid(),
        .nl_groups = 0xffffffff
    };

    int32_t fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (fd == INVALID_FD) {
        BATTERY_HILOGE(FEATURE_CHARGING, "open uevent socket failed, fd is invalid");
        return INVALID_FD;
    }

    int32_t ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    if (ret < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "set socket opt failed, ret: %{public}d", ret);
        close(fd);
        return INVALID_FD;
    }

    ret = bind(fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(struct sockaddr_nl));
    if (ret < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "bind socket address failed, ret: %{public}d", ret);
        close(fd);
        return INVALID_FD;
    }
    return fd;
}

int32_t BatteryThread::RegisterCallback(int32_t fd, EventType et)
{
    struct epoll_event ev = {0};

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }

    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        BATTERY_HILOGE(FEATURE_CHARGING, "epoll_ctl failed, error num =%{public}d", errno);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

void BatteryThread::UpdateEpollInterval(const int32_t chargeState)
{
    if ((chargeState != PowerSupplyProvider::CHARGE_STATE_NONE) &&
        (chargeState != PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        epollInterval_ = TIMER_FAST_SEC * SEC_TO_MSEC;
    } else {
        epollInterval_ = -1;
    }
}

int32_t BatteryThread::InitUevent()
{
    ueventFd_ = OpenUeventSocket();
    if (ueventFd_ == INVALID_FD) {
        BATTERY_HILOGE(FEATURE_CHARGING, "open uevent socket failed, fd is invalid");
        return HDF_ERR_BAD_FD;
    }

    fcntl(ueventFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(ueventFd_, &BatteryThread::UeventCallback));

    if (RegisterCallback(ueventFd_, EVENT_UEVENT_FD)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "register Uevent event failed");
        return HDF_ERR_BAD_FD;
    }
    return HDF_SUCCESS;
}

int32_t BatteryThread::Init([[maybe_unused]] void* service)
{
    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ != nullptr) {
        provider_->InitBatteryPath();
        provider_->InitPowerSupplySysfs();
    }

    epFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epFd_ == INVALID_FD) {
        BATTERY_HILOGE(FEATURE_CHARGING, "epoll create failed, epFd_ is invalid");
        return HDF_ERR_BAD_FD;
    }

    InitUevent();

    return HDF_SUCCESS;
}

int32_t BatteryThread::UpdateWaitInterval()
{
    return HDF_FAILURE;
}

void BatteryThread::UeventCallback(void* service)
{
    char msg[UEVENT_MSG_LEN + UEVENT_RESERVED_SIZE] = { 0 };

    ssize_t len = recv(ueventFd_, msg, UEVENT_MSG_LEN, 0);
    if (len < 0 || len >= UEVENT_MSG_LEN) {
        BATTERY_HILOGI(FEATURE_CHARGING, "recv return msg is invalid, len: %{public}zd", len);
        return;
    }

    // msg separator
    msg[len] = '\0';
    msg[len + 1] = '\0';
    if (!IsPowerSupplyEvent(msg)) {
        return;
    }
    UpdateBatteryInfo(service);
}

void BatteryThread::UpdateBatteryInfo(void* service) {}

bool BatteryThread::IsPowerSupplyEvent(const char* msg)
{
    while (*msg) {
        if (!strcmp(msg, POWER_SUPPLY.c_str())) {
            return true;
        }
        while (*msg++) {} // move to next
    }

    return false;
}

int32_t BatteryThread::LoopingThreadEntry(void* arg)
{
    int32_t nevents = 0;
    size_t size = callbacks_.size();
    struct epoll_event events[size];

    while (true) {
        if (!nevents) {
            CycleMatters();
        }

        HandleStates();

        int32_t timeout = epollInterval_;
        int32_t waitTimeout = UpdateWaitInterval();
        if ((timeout < 0) || (waitTimeout > 0 && waitTimeout < timeout)) {
            timeout = waitTimeout;
        }

        nevents = epoll_wait(epFd_, events, static_cast<int32_t>(size), timeout);
        if (nevents <= 0) {
            continue;
        }

        for (int32_t n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                auto* func = const_cast<BatteryThread*>(this);
                (callbacks_.find(events[n].data.fd)->second)(func, arg);
            }
        }
    }
}

void BatteryThread::StartThread(void* service)
{
    Init(service);
    Run(service);
}

void BatteryThread::Run(void* service) {}
} // namespace PowerMgr
} // namespace OHOS

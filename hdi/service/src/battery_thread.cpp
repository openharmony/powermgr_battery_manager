/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <linux/netlink.h>

#include "utils/hdf_log.h"

#include "battery_host_service_stub.h"

#define HDF_LOG_TAG batteryd

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const int ERR_INVALID_FD = -1;
const int ERR_OPERATION_FAILED = -1;
const int UEVENT_BUFF_SIZE = (64 * 1024);
const int UEVENT_RESERVED_SIZE = 2;
const int UEVENT_MSG_LEN = (2 * 1024);
const int TIMER_INTERVAL = 10;
const int TIMER_FAST_SEC = 2;
const int TIMER_SLOW_SEC = 10;
const int SEC_TO_MSEC = 1000;
const std::string POWER_SUPPLY = "SUBSYSTEM=power_supply";

int32_t BatteryThread::OpenUeventSocket(void) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t bufferSize = UEVENT_BUFF_SIZE;
    struct sockaddr_nl address = {
        .nl_pid = getpid(),
        .nl_family = AF_NETLINK,
        .nl_groups = 0xffffffff
    };

    int32_t fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (fd == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s: open uevent socket failed, fd is invalid", __func__);
        return ERR_INVALID_FD;
    }

    int32_t ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, &bufferSize, sizeof(bufferSize));
    if (ret == ERR_OPERATION_FAILED) {
        HDF_LOGE("%{public}s: set socket opt failed, ret: %{public}d", __func__, ret);
        close(fd);
        return ERR_INVALID_FD;
    }

    ret = bind(fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(struct sockaddr_nl));
    if (ret == ERR_OPERATION_FAILED) {
        HDF_LOGE("%{public}s: bind socket address failed, ret: %{public}d", __func__, ret);
        close(fd);
        return ERR_INVALID_FD;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return fd;
}

int BatteryThread::RegisterCallback(const int fd, const EventType et)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct epoll_event ev;

    ev.events = EPOLLIN;
    if (et == EVENT_TIMER_FD) {
        ev.events |= EPOLLWAKEUP;
    }

    ev.data.ptr = reinterpret_cast<void*>(this);
    ev.data.fd = fd;
    if (epoll_ctl(epFd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
        HDF_LOGE("%{public}s: epoll_ctl failed, error num =%{public}d", __func__, errno);
        return -1;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return 0;
}

void BatteryThread::SetTimerInterval(int interval)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct itimerspec itval;

    if (timerFd_ == ERR_INVALID_FD) {
        return;
    }

    timerInterval_ = interval;

    if (interval < 0) {
        interval = 0;
    }

    itval.it_interval.tv_sec = interval;
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = interval;
    itval.it_value.tv_nsec = 0;

    if (timerfd_settime(timerFd_, 0, &itval, nullptr) == -1) {
        HDF_LOGE("%{public}s: timer failed\n", __func__);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryThread::UpdateEpollInterval(const int32_t chargestate)
{
    HDF_LOGI("%{public}s enter", __func__);
    int interval;

    if ((chargestate != PowerSupplyProvider::CHARGE_STATE_NONE) &&
        (chargestate != PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        interval = TIMER_FAST_SEC;
        epollInterval_ = interval * SEC_TO_MSEC;
    } else {
        interval = TIMER_SLOW_SEC;
        epollInterval_ = -1;
    }

    if ((interval != timerInterval_) && (timerInterval_ > 0)) {
        SetTimerInterval(interval);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

int32_t BatteryThread::InitUevent()
{
    HDF_LOGI("%{public}s enter", __func__);
    ueventFd_ = OpenUeventSocket();
    if (ueventFd_ == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s: open uevent socket failed, fd is invalid", __func__);
        return HDF_ERR_BAD_FD;
    }

    fcntl(ueventFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(ueventFd_, &BatteryThread::UeventCallback));

    if (RegisterCallback(ueventFd_, EVENT_UEVENT_FD)) {
        HDF_LOGE("%{public}s: register Uevent event failed", __func__);
        return HDF_ERR_BAD_FD;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryThread::Init(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    BatteryHostServiceStub* stubService = reinterpret_cast<BatteryHostServiceStub*>(service);
    if (stubService != nullptr) {
        stubService->provider_->InitBatteryPath();
        stubService->provider_->InitPowerSupplySysfs();
    }

    epFd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epFd_ == -1) {
        HDF_LOGE("%{public}s: epoll create failed, timerFd_ is invalid", __func__);
        return HDF_ERR_BAD_FD;
    }

    InitTimer();
    InitUevent();

    HDF_LOGI("%{public}s exit", __func__);
    return 0;
}

int BatteryThread::UpdateWaitInterval()
{
    HDF_LOGI("%{public}s enter", __func__);
    return -1;
}

int32_t BatteryThread::InitTimer()
{
    HDF_LOGI("%{public}s enter", __func__);
    timerFd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timerFd_ == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s: epoll create failed, timerFd_ is invalid", __func__);
        return HDF_ERR_BAD_FD;
    }

    SetTimerInterval(TIMER_INTERVAL);
    fcntl(timerFd_, F_SETFL, O_NONBLOCK);
    callbacks_.insert(std::make_pair(timerFd_, &BatteryThread::TimerCallback));

    if (RegisterCallback(timerFd_, EVENT_TIMER_FD)) {
        HDF_LOGE("%{public}s: register Timer event failed", __func__);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void BatteryThread::TimerCallback(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    unsigned long long timers;

    if (read(timerFd_, &timers, sizeof(timers)) == -1) {
        HDF_LOGE("%{public}s: read timerFd_ failed", __func__);
        return;
    }

    UpdateBatteryInfo(service);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryThread::UeventCallback(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    char msg[UEVENT_MSG_LEN + UEVENT_RESERVED_SIZE] = { 0 };

    int32_t len = recv(ueventFd_, msg, UEVENT_MSG_LEN, 0);
    if (len < 0 || len >= UEVENT_MSG_LEN) {
        HDF_LOGD("%{public}s: recv return msg is invalid, len: %{public}d", __func__, len);
        return;
    }

    // msg separator
    msg[len] = '\0';
    msg[len + 1] = '\0';
    if (!IsPowerSupplyEvent(msg)) {
        return;
    }
    UpdateBatteryInfo(service, msg);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryThread::UpdateBatteryInfo(void* service, char* msg)
{
    HDF_LOGI("%{public}s enter", __func__);
    BatteryHostServiceStub* stubService = reinterpret_cast<BatteryHostServiceStub*>(service);
    if (stubService != nullptr) {
        stubService->UpdateBatterydInfo(msg);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryThread::UpdateBatteryInfo(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    BatteryHostServiceStub* stubService = reinterpret_cast<BatteryHostServiceStub*>(service);
    if (stubService != nullptr) {
        stubService->UpdateBatterydInfo();
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

bool BatteryThread::IsPowerSupplyEvent(const char* msg) const
{
    HDF_LOGI("%{public}s enter", __func__);
    while (*msg) {
        if (!strcmp(msg, POWER_SUPPLY.c_str())) {
            return true;
        }
        while (*msg++) {} // move to next
    }

    HDF_LOGI("%{public}s exit", __func__);
    return false;
}

int BatteryThread::LoopingThreadEntry(void* arg)
{
    HDF_LOGI("%{public}s enter", __func__);
    int nevents = 0;
    size_t cbct = callbacks_.size();
    struct epoll_event events[cbct];

    HDF_LOGD("%{public}s: start batteryd looping", __func__);
    while (true) {
        if (!nevents) {
            CycleMatters();
        }

        HandleStates();

        int timeout = epollInterval_;
        int waitTimeout = UpdateWaitInterval();
        if ((timeout < 0) || (waitTimeout > 0 && waitTimeout < timeout)) {
            timeout = waitTimeout;
        }
        HDF_LOGD("%{public}s: timeout=%{public}d, nevents=%{public}d", __func__, timeout, nevents);

        nevents = epoll_wait(epFd_, events, cbct, timeout);
        if (nevents == -1) {
            continue;
        }

        for (int n = 0; n < nevents; ++n) {
            if (events[n].data.ptr) {
                BatteryThread* func = const_cast<BatteryThread*>(this);
                (callbacks_.find(events[n].data.fd)->second)(func, arg);
            }
        }
    }
}

void BatteryThread::StartThread(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    Init(service);
    Run(service);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryThread::Run(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    std::make_unique<std::thread>(&BatteryThread::LoopingThreadEntry, this, service)->detach();
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

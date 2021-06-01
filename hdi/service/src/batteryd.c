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

#include "batteryd.h"

#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "batteryd_dispatcher.h"
#include "batteryd_parser.h"
#include "batteryd_publisher.h"

#include "osal/osal_mem.h"
#include "osal/osal_thread.h"
#include "utils/hdf_log.h"

#define HDF_LOG_TAG batteryd
#define ERR_INVALID_FD (-1)
#define ERR_OPERATION_FAILED (-1)
#define UEVENT_BUFF_SIZE (64 * 1024)
#define UEVENT_RESERVED_SIZE (2)
#define UEVENT_MSG_LEN (2 * 1024)
#define SUBSYSTEM_POWER_SUPPLY "SUBSYSTEM=power_supply"

static int32_t BatterydBind(struct HdfDeviceObject *device);
static int32_t BatterydInit(struct HdfDeviceObject *device);
static void BatterydRelease(struct HdfDeviceObject *device);

struct HdfDriverEntry g_batterydEntry = {
    .moduleVersion = 1,
    .moduleName = "batteryd",
    .Bind = BatterydBind,
    .Init = BatterydInit,
    .Release = BatterydRelease,
};

HDF_INIT(g_batterydEntry);

static int32_t BatterydBind(struct HdfDeviceObject *device)
{
    HDF_LOGD("%{public}s enter", __func__);
    struct BatterydService *service = NULL;
    if (device == NULL) {
        HDF_LOGW("%{public}s device is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    service = (struct BatterydService *)OsalMemCalloc(sizeof(*service));
    if (service == NULL) {
        HDF_LOGE("%{public}s: calloc address error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    service->device = device;
    service->ioService.Dispatch = BatterydDispatch;
    device->service = &(service->ioService);
    return HDF_SUCCESS;
}

static int32_t OpenUeventSocket(void)
{
    int32_t ret;
    int32_t bufferSize = UEVENT_BUFF_SIZE;
    struct sockaddr_nl address = {
        .nl_pid = getpid(),
        .nl_family = AF_NETLINK,
        .nl_groups = 0xffffffff
    };

    int32_t fd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (fd == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s open uevent socket failed, fd is invalid", __func__);
        return ERR_INVALID_FD;
    }

    ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, &bufferSize, sizeof(bufferSize));
    if (ret == ERR_OPERATION_FAILED) {
        HDF_LOGE("%{public}s set socket opt failed, ret: %{public}d", __func__, ret);
        close(fd);
        return ERR_INVALID_FD;
    }

    ret = bind(fd, (struct sockaddr *)(&address), sizeof(struct sockaddr_nl));
    if (ret == ERR_OPERATION_FAILED) {
        HDF_LOGE("%{public}s bind socket address failed, ret: %{public}d", __func__, ret);
        close(fd);
        return ERR_INVALID_FD;
    }
    return fd;
}

static int32_t InitUevent(struct BatterydService *service)
{
    int32_t fd = OpenUeventSocket();
    if (fd == ERR_INVALID_FD) {
        HDF_LOGE("%{public}s open uevent socket failed, fd is invalid", __func__);
        return HDF_ERR_BAD_FD;
    }
    service->ueventFd = fd;
    return HDF_SUCCESS;
}

static bool IsPowerSupplyEvent(const char *msg)
{
    while (*msg) {
        if (!strcmp(msg, SUBSYSTEM_POWER_SUPPLY)) {
            return true;
        }
        while (*msg++) {} // move to next
    }
    return false;
}

static int LoopingThreadEntry(void *arg)
{
    int32_t len;
    char msg[UEVENT_MSG_LEN + UEVENT_RESERVED_SIZE] = {0};
    struct BatterydService *service = (struct BatterydService *)arg;

    HDF_LOGD("%{public}s enter, start batteryd looping", __func__);
    while (true) {
        do {
            len = recv(service->ueventFd, msg, UEVENT_MSG_LEN, 0);
            if (len < 0 || len >= UEVENT_MSG_LEN) {
                HDF_LOGD("%{public}s recv return msg is invalid, len: %{public}d", __func__, len);
                continue;
            }
            // msg separator
            msg[len] = '\0';
            msg[len + 1] = '\0';
            if (!IsPowerSupplyEvent(msg)) {
                continue;
            }
            struct BatterydInfo *batteryInfo = (struct BatterydInfo *)OsalMemCalloc(sizeof(*batteryInfo));
            if (batteryInfo == NULL) {
                HDF_LOGE("%{public}s: calloc battery info error", __func__);
                continue;
            }
            ParseUeventToBatterydInfo(msg, batteryInfo);
            NotifySubscriber(service->subscriber, batteryInfo);
            OsalMemFree(batteryInfo);
        } while (len > 0);
        HDF_LOGD("%{public}s recv error, len: %{public}d", __func__, len);
    }
}

static int32_t StartLoopingThread(struct BatterydService *service)
{
    int32_t ret;
    OSAL_DECLARE_THREAD(loopingThread);

    ret = OsalThreadCreate(&loopingThread, (OsalThreadEntry)LoopingThreadEntry, service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s osal create looping thread failed, ret: %{public}d", __func__, ret);
        return ret;
    }
    struct OsalThreadParam threadParam = {
        .name = "batteryd_looping_thread",
        .stackSize = 0x4000,
        .priority = OSAL_THREAD_PRI_DEFAULT
    };
    ret = OsalThreadStart(&loopingThread, &threadParam);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s osal start looping thread failed, ret: %{public}d", __func__, ret);
        OsalThreadDestroy(&loopingThread);
        return ret;
    }
    return ret;
}

static int32_t BatterydInit(struct HdfDeviceObject *device)
{
    HDF_LOGD("%{public}s enter", __func__);
    int32_t ret;
    if (device == NULL || device->service == NULL) {
        HDF_LOGW("%{public}s device or device->service is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    struct BatterydService *service = (struct BatterydService *)device->service;
    if (service == NULL) {
        HDF_LOGE("%{public}s get batteryd service failed", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = InitBatterydSysfs();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s init batteryd sysfs failed, ret: %{public}d", __func__, ret);
        return ret;
    }
    ret = InitUevent(service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s init uevent failed, ret: %{public}d", __func__, ret);
        return ret;
    }
    ret = StartLoopingThread(service);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s start loop thread failed, ret: %{public}d", __func__, ret);
        return ret;
    }
    return ret;
}

static void BatterydRelease(struct HdfDeviceObject *device)
{
    HDF_LOGD("%{public}s enter", __func__);
    if (device == NULL) {
        return;
    }
    struct BatterydService *service = (struct BatterydService *)device->service;
    if (service == NULL) {
        HDF_LOGW("%{public}s batteryd service is NULL, no need to release", __func__);
        return;
    }
    OsalMemFree(service);
}

int32_t BindBatterySubscriber(struct BatterydService *service, struct BatterydSubscriber *subscriber)
{
    if (service == NULL) {
        HDF_LOGW("%{public}s service is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    service->subscriber = subscriber;
    return HDF_SUCCESS;
}

int32_t UnbindBatterySubscriber(struct BatterydService *service)
{
    if (service == NULL) {
        HDF_LOGW("%{public}s service is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (service->subscriber != NULL) {
        OsalMemFree(service->subscriber);
        service->subscriber = NULL;
    }
    return HDF_SUCCESS;
}
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

#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_device_desc.h>
#include <osal_mem.h>

#include "battery_host_service_stub.h"

#define HDF_LOG_TAG batteryd

using namespace OHOS::HDI::Battery::V1_0;

namespace {
int32_t BatterydDispatch(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct HdfBatterydService *service =
        CONTAINER_OF(client->device->service, struct HdfBatterydService, ioService);

    HDF_LOGI("%{public}s exit", __func__);
    return BatteryHostServiceOnRemoteRequest(static_cast<char*>(service->instance), cmdId, data, reply);
}

int32_t BatterydBind(struct HdfDeviceObject *device)
{
    HDF_LOGI("%{public}s enter", __func__);

    struct HdfBatterydService *service =
        reinterpret_cast<HdfBatterydService *>(OsalMemAlloc(sizeof(HdfBatterydService)));
    if (service == nullptr) {
        HDF_LOGE("%{public}s: calloc HdfBatterydService error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    service->device = device;
    service->ioService.Dispatch = BatterydDispatch;
    service->ioService.Open = nullptr;
    service->ioService.Release = nullptr;
    service->instance = BatteryHostServiceStubInstance();

    device->service = &(service->ioService);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatterydInit(struct HdfDeviceObject *device)
{
    HDF_LOGI("%{public}s enter", __func__);

    return HDF_SUCCESS;
}

void BatterydRelease(struct HdfDeviceObject *device)
{
    HDF_LOGI("%{public}s enter", __func__);

    if (device == nullptr) {
        return;
    }
    struct HdfBatterydService *service = CONTAINER_OF(device->service, struct HdfBatterydService, ioService);
    if (service == nullptr) {
        HDF_LOGW("%{public}s: batteryd service is nullptr, no need to release", __func__);
        return;
    }
    OsalMemFree(service);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

struct HdfDriverEntry g_batterydEntry = {
    .moduleVersion = 1,
    .moduleName = "batteryd",
    .Bind = BatterydBind,
    .Init = BatterydInit,
    .Release = BatterydRelease,
};
}

#ifndef __cplusplus
extern "C" {
#endif

HDF_INIT(g_batterydEntry);

#ifndef __cplusplus
}
#endif

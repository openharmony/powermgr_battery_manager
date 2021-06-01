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

#include "batteryd_publisher.h"

#include "hdf_remote_service.h"
#include "utils/hdf_log.h"

#define HDF_LOG_TAG batteryd_publisher

void NotifySubscriber(const struct BatterydSubscriber *subscriber, const struct BatterydInfo *batteryInfo)
{
    HDF_LOGD("%{public}s: enter", __func__);
    if (subscriber == NULL) {
        HDF_LOGD("%{public}s: subscriber is NULL", __func__);
        return;
    }
    int ret;
    struct HdfRemoteService *service = subscriber->remoteService;
    struct HdfSBuf *data = HdfSBufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSBufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s failed to obtain hdf sbuf", __func__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return;
    }
    HDF_LOGD("%{public}s BatteryInfo: capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
             "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
             "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
             "technology=%{public}s", __func__, batteryInfo->capacity_, batteryInfo->voltage_,
        batteryInfo->temperature_, batteryInfo->healthState_, batteryInfo->pluggedType_,
        batteryInfo->pluggedMaxCurrent_, batteryInfo->pluggedMaxVoltage_, batteryInfo->chargeState_,
        batteryInfo->chargeCounter_, batteryInfo->present_, batteryInfo->technology_);

    HdfSbufWriteInt32(data, batteryInfo->capacity_);
    HdfSbufWriteInt32(data, batteryInfo->voltage_);
    HdfSbufWriteInt32(data, batteryInfo->temperature_);
    HdfSbufWriteInt32(data, batteryInfo->healthState_);
    HdfSbufWriteInt32(data, batteryInfo->pluggedType_);
    HdfSbufWriteInt32(data, batteryInfo->pluggedMaxCurrent_);
    HdfSbufWriteInt32(data, batteryInfo->pluggedMaxVoltage_);
    HdfSbufWriteInt32(data, batteryInfo->chargeState_);
    HdfSbufWriteInt32(data, batteryInfo->chargeCounter_);
    HdfSbufWriteInt8(data, batteryInfo->present_);
    HdfSbufWriteString(data, batteryInfo->technology_);

    ret = service->dispatcher->Dispatch(service, CMD_NOTIFY_SUBSCRIBER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed to notify subscriber, ret: %{public}d", __func__, ret);
    } else {
        HDF_LOGD("%{public}s: succeed to notify subscriber", __func__);
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);
}
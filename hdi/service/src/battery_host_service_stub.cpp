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

#include "battery_host_service_stub.h"

#include <parameters.h>
#include <securec.h>

#include "hdf_log.h"
#include "hdf_base.h"
#include "hdf_sbuf_ipc.h"
#include "osal/osal_mem.h"
#include "hdf_remote_service.h"
#include "utils/hdf_log.h"

#include "batteryd.h"
#include "power_mgr_client.h"

#define Hi3516DV300

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
int32_t BatteryHostServiceStub::Init()
{
    HDF_LOGI("%{public}s enter", __func__);
    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate PowerSupplyProvider error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
#ifdef Hi3516DV300
    provider_->InitDefaultSysfs();
#endif
    provider_->InitPowerSupplySysfs();

    batteryConfig_ = std::make_unique<BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        HDF_LOGD("%{public}s: instantiate batteryconfig error.", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    batteryConfig_->Init();

    batteryLed_ = std::make_unique<BatteryLed>();
    if (batteryLed_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate BatteryLed error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    loop_ = std::make_unique<BatteryThread>();
    if (loop_ == nullptr) {
        HDF_LOGE("%{public}s: Instantiate BatteryThread error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    loop_->StartThread(this);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchBindBatterySubscriber(HdfSBuf& data)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct HdfRemoteService *remoteService = HdfSBufReadRemoteService(&data);
    if (remoteService == nullptr) {
        HDF_LOGW("%{public}s: remoteService is nullptr", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    subscriber_ = remoteService;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchUnbindBatterySubscriber()
{
    HDF_LOGI("%{public}s enter", __func__);
    if (subscriber_ == nullptr) {
        HDF_LOGW("%{public}s: subscriber is nullptr", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    OsalMemFree(subscriber_);
    subscriber_ = nullptr;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchChangePath(MessageParcel& data) const
{
    HDF_LOGI("%{public}s enter", __func__);

    std::string path = data.ReadString();
    provider_->SetSysFilePath(path);
    HDF_LOGI("%{public}s: enter, change path to %{public}s", __func__, path.c_str());

    provider_->InitPowerSupplySysfs();
    UpdateBatterydInfo();

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetCapacity(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t capacity = 0;
    int32_t ret = provider_->ParseCapacity(&capacity);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(capacity)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetVoltage(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t voltage = 0;
    int32_t ret = provider_->ParseVoltage(&voltage);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(voltage)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetTemperature(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t temperature = 0;
    int32_t ret = provider_->ParseTemperature(&temperature);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(temperature)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetHealthState(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t healthState = 0;
    int32_t ret = provider_->ParseHealthState(&healthState);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(healthState)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetPluggedType(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t pluggedType = 0;
    int32_t ret = provider_->ParsePluggedType(&pluggedType);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(pluggedType)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetChargeState(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t chargeState = 0;
    int32_t ret = provider_->ParseChargeState(&chargeState);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt32(chargeState)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetPresent(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int8_t present = 0;
    int32_t ret = provider_->ParsePresent(&present);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteInt8(present)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryHostServiceStub::DispatchGetTechnology(MessageParcel& reply) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string technology;
    int32_t ret = provider_->ParseTechnology(technology);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if (!reply.WriteString(technology)) {
        HDF_LOGE("%{public}s: write result failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void BatteryHostServiceStub::UpdateBatterydInfo(const char *msg) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        HDF_LOGE("%{public}s: instantiate batteryInfo error", __func__);
        return;
    }

    provider_->ParseUeventToBatterydInfo(msg, batteryInfo.get());

    if (subscriber_ == nullptr) {
        HDF_LOGE("%{public}s: check subscriber_ failed", __func__);
        return;
    }

    NotifySubscriber(batteryInfo.get());
    HandleTemperature(batteryInfo->temperature_);
    batteryLed_->UpdateLedColor(batteryInfo->chargeState_, batteryInfo->capacity_);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryHostServiceStub::UpdateBatterydInfo() const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        HDF_LOGE("%{public}s: instantiate batteryInfo error", __func__);
        return;
    }

    provider_->UpdateInfoByReadSysFile(batteryInfo.get());
    NotifySubscriber(batteryInfo.get());
    HandleTemperature(batteryInfo->temperature_);
    batteryLed_->UpdateLedColor(batteryInfo->chargeState_, batteryInfo->capacity_);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryHostServiceStub::HandleTemperature(const int32_t &temperature) const
{
    HDF_LOGI("%{public}s enter", __func__);
    auto tempConf = batteryConfig_->GetTempConf();
    HDF_LOGD("%{public}s: temperature=%{public}d, tempConf.lower=%{public}d, tempConf.upper=%{public}d",
        __func__, temperature, tempConf.lower, tempConf.upper);

    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if (((temperature <= tempConf.lower) || (temperature >= tempConf.upper)) && (tempConf.lower != tempConf.upper)) {
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryHostServiceStub::NotifySubscriber(const struct BatterydInfo *batteryInfo) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (subscriber_ == nullptr) {
        HDF_LOGD("%{public}s: subscriber is nullptr", __func__);
        return;
    }

    struct HdfSBuf *data = HdfSBufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSBufTypedObtain(SBUF_IPC);
    if (data == nullptr || reply == nullptr) {
        HDF_LOGE("%{public}s: failed to obtain hdf sbuf", __func__);
        HdfSBufRecycle(data);
        HdfSBufRecycle(reply);
        return;
    }
    HDF_LOGD("%{public}s: BatteryInfo capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
             "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
             "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
             "technology=%{public}s", __func__, batteryInfo->capacity_, batteryInfo->voltage_,
        batteryInfo->temperature_, batteryInfo->healthState_, batteryInfo->pluggedType_,
        batteryInfo->pluggedMaxCurrent_, batteryInfo->pluggedMaxVoltage_, batteryInfo->chargeState_,
        batteryInfo->chargeCounter_, batteryInfo->present_, batteryInfo->technology_.c_str());

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
    HdfSbufWriteString(data, batteryInfo->technology_.c_str());

    int ret = subscriber_->dispatcher->Dispatch(subscriber_, CMD_NOTIFY_SUBSCRIBER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to notify subscriber, ret: %{public}d", __func__, ret);
    } else {
        HDF_LOGD("%{public}s: succeed to notify subscriber", __func__);
    }
    HdfSBufRecycle(data);
    HdfSBufRecycle(reply);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void *BatteryHostServiceStubInstance()
{
    HDF_LOGI("%{public}s enter", __func__);
    using namespace OHOS::HDI::Battery::V1_0;
    BatteryHostServiceStub *stub = new BatteryHostServiceStub();
    if (stub == nullptr) {
        HDF_LOGE("%{public}s: failed to create BatteryHostServiceStub", __func__);
        return nullptr;
    }

    if (stub->Init() != HDF_SUCCESS) {
        delete stub;
        stub = nullptr;
        return nullptr;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return reinterpret_cast<void *>(stub);
}

int32_t BatteryHostServiceOnRemoteRequest(char *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("%{public}s enter", __func__);
    using namespace OHOS::HDI::Battery::V1_0;
    BatteryHostServiceStub *serviceStub = reinterpret_cast<BatteryHostServiceStub *>(stub);

    OHOS::MessageParcel *replyParcel = nullptr;
    OHOS::MessageParcel *dataParcel = nullptr;

    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmdId) {
        case CMD_BIND_BATTERY_SUBSCRIBER: {
            return serviceStub->DispatchBindBatterySubscriber(*data);
        }
        case CMD_UNBIND_BATTERY_SUBSCRIBER: {
            return serviceStub->DispatchUnbindBatterySubscriber();
        }
        case CMD_GET_CAPACITY: {
            return serviceStub->DispatchGetCapacity(*replyParcel);
        }
        case CMD_CHANGE_PATH: {
            return serviceStub->DispatchChangePath(*dataParcel);
        }
        case CMD_GET_VOLTAGE: {
            return serviceStub->DispatchGetVoltage(*replyParcel);
        }
        case CMD_GET_TEMPERATURE: {
            return serviceStub->DispatchGetTemperature(*replyParcel);
        }
        case CMD_GET_HEALTH_STATE: {
            return serviceStub->DispatchGetHealthState(*replyParcel);
        }
        case CMD_GET_PLUGGED_TYPE: {
            return serviceStub->DispatchGetPluggedType(*replyParcel);
        }
        case CMD_GET_CHARGE_STATE: {
            return serviceStub->DispatchGetChargeState(*replyParcel);
        }
        case CMD_GET_PRESENT: {
            return serviceStub->DispatchGetPresent(*replyParcel);
        }
        case CMD_GET_TECHNOLOGY: {
            return serviceStub->DispatchGetTechnology(*replyParcel);
        }
        default: {
            return HDF_FAILURE;
        }
    }
    HDF_LOGI("%{public}s exit", __func__);
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

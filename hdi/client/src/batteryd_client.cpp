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

#include "batteryd_client.h"
#include "batteryd_api.h"
#include "power_common.h"
#include "iservmgr_hdi.h"

namespace OHOS {
namespace PowerMgr {
using OHOS::HDI::ServiceManager::V1_0::IServiceManager;

namespace {
const std::string BATTERYD_SERVICE = "batteryd";
}

sptr<IRemoteObject> BatterydClient::GetBatterydService()
{
    auto serviceManager = IServiceManager::Get();
    if (serviceManager == nullptr) {
        POWER_HILOGW(MODULE_BATTERYD, "service manager is nullptr");
        return nullptr;
    }
    auto batterydService = serviceManager->GetService(BATTERYD_SERVICE.c_str());
    if (batterydService == nullptr) {
        POWER_HILOGW(MODULE_BATTERYD, "batteryd service is nullptr");
        return nullptr;
    }
    return batterydService;
}

ErrCode BatterydClient::DoDispatch(uint32_t cmd, MessageParcel &data, MessageParcel &reply)
{
    POWER_HILOGD(MODULE_BATTERYD, "Start to dispatch cmd: %{public}d", cmd);
    auto batteryd = GetBatterydService();
    if (batteryd == nullptr) {
        return ERR_NO_INIT;
    }

    MessageOption option;
    auto ret = batteryd->SendRequest(cmd, data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATTERYD, "failed to send request, cmd: %{public}d, ret: %{public}d", cmd, ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode BatterydClient::BindBatterydSubscriber(const sptr<BatterydSubscriber> &subscriber)
{
    POWER_HILOGD(MODULE_BATTERYD, "BindBatterydSubscriber enter");
    if (subscriber == nullptr) {
        POWER_HILOGW(MODULE_BATTERYD, "subscriber is nullptr");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    data.WriteRemoteObject(subscriber);
    return DoDispatch(CMD_BIND_BATTERY_SUBSCRIBER, data, reply);
}

ErrCode BatterydClient::UnbindBatterydSubscriber()
{
    POWER_HILOGD(MODULE_BATTERYD, "UnbindBatterydSubscriber enter");
    MessageParcel data;
    MessageParcel reply;
    return DoDispatch(CMD_UNBIND_BATTERY_SUBSCRIBER, data, reply);
}

int32_t BatterydClient::GetCapacity()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetCapacity enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_CAPACITY, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_CAPACITY failed, return INVALID_BATT_INT_VALUE");
        return INVALID_BATT_INT_VALUE;
    }
    int32_t capacity = reply.ReadInt32();
    return capacity;
}

int32_t BatterydClient::GetVoltage()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetVoltage enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_VOLTAGE, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_VOLTAGE failed, return INVALID_BATT_INT_VALUE");
        return INVALID_BATT_INT_VALUE;
    }
    int32_t voltage = reply.ReadInt32();
    return voltage;
}

int32_t BatterydClient::GetTemperature()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetTemperature enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_TEMPERATURE, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_TEMPERATURE failed, return INVALID_BATT_TEMP_VALUE");
        return INVALID_BATT_TEMP_VALUE;
    }
    int32_t temperature = reply.ReadInt32();
    return temperature;
}

BatteryHealthState BatterydClient::GetHealthState()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetHealthState enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_HEALTH_STATE, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_HEALTH_STATE failed, return HEALTH_STATE_UNKNOWN");
        return BatteryHealthState::HEALTH_STATE_UNKNOWN;
    }
    int32_t healthState = reply.ReadInt32();
    return (BatteryHealthState)healthState;
}

BatteryPluggedType BatterydClient::GetPluggedType()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetPluggedType enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_PLUGGED_TYPE, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_PLUGGED_TYPE failed, return PLUGGED_TYPE_NONE");
        return BatteryPluggedType::PLUGGED_TYPE_NONE;
    }
    int32_t pluggedType = reply.ReadInt32();
    return (BatteryPluggedType)pluggedType;
}

BatteryChargeState BatterydClient::GetChargeState()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetChargeState enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_CHARGE_STATE, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_CHARGE_STATE failed, return CHARGE_STATE_NONE");
        return BatteryChargeState::CHARGE_STATE_NONE;
    }
    int32_t chargeState = reply.ReadInt32();
    return (BatteryChargeState)chargeState;
}

bool BatterydClient::GetPresent()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetPresent enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_PRESENT, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_PRESENT failed, return INVALID_BATT_BOOL_VALUE");
        return INVALID_BATT_BOOL_VALUE;
    }
    bool present = (bool)reply.ReadInt8();
    return present;
}

std::string BatterydClient::GetTechnology()
{
    POWER_HILOGD(MODULE_BATTERYD, "GetTechnology enter");
    MessageParcel data;
    MessageParcel reply;
    ErrCode ret = DoDispatch(CMD_GET_TECHNOLOGY, data, reply);
    if (FAILED(ret)) {
        POWER_HILOGW(MODULE_BATTERYD, "CMD_GET_TECHNOLOGY failed, return INVALID_STRING_VALUE");
        return INVALID_STRING_VALUE;
    }
    std::string technology = reply.ReadCString();
    return technology;
}
} // namespace PowerMgr
} // namespace OHOS
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

#include "battery_srv_proxy.h"
#include "ipc_types.h"
#include "message_parcel.h"
#include "power_common.h"

namespace OHOS {
namespace PowerMgr {
int32_t BatterySrvProxy::GetCapacity()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, INVALID_BATT_INT_VALUE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return INVALID_BATT_INT_VALUE;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_CAPACITY),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return INVALID_BATT_INT_VALUE;
    }
    int32_t capacity = INVALID_BATT_INT_VALUE;
    READ_PARCEL_WITH_RET(reply, Int32, capacity, INVALID_BATT_INT_VALUE);
    return capacity;
}

BatteryChargeState BatterySrvProxy::GetChargingStatus()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, BatteryChargeState::CHARGE_STATE_BUTT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return BatteryChargeState::CHARGE_STATE_BUTT;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_CHARGING_STATUS),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return BatteryChargeState::CHARGE_STATE_BUTT;
    }
    uint32_t chargingState = static_cast<uint32_t>(BatteryChargeState::CHARGE_STATE_BUTT);
    READ_PARCEL_WITH_RET(reply, Uint32, chargingState, BatteryChargeState::CHARGE_STATE_BUTT);
    return static_cast<BatteryChargeState>(chargingState);
}

BatteryHealthState BatterySrvProxy::GetHealthStatus()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, BatteryHealthState::HEALTH_STATE_BUTT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return BatteryHealthState::HEALTH_STATE_BUTT;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_HEALTH_STATUS),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return BatteryHealthState::HEALTH_STATE_BUTT;
    }
    uint32_t healthStatus = static_cast<uint32_t>(BatteryHealthState::HEALTH_STATE_BUTT);
    READ_PARCEL_WITH_RET(reply, Uint32, healthStatus, BatteryHealthState::HEALTH_STATE_BUTT);
    return static_cast<BatteryHealthState>(healthStatus);
}

BatteryPluggedType BatterySrvProxy::GetPluggedType()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, BatteryPluggedType::PLUGGED_TYPE_BUTT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return BatteryPluggedType::PLUGGED_TYPE_BUTT;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_PLUG_TYPE),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return BatteryPluggedType::PLUGGED_TYPE_BUTT;
    }
    uint32_t pluggedType = static_cast<uint32_t>(BatteryPluggedType::PLUGGED_TYPE_BUTT);
    READ_PARCEL_WITH_RET(reply, Uint32, pluggedType, BatteryPluggedType::PLUGGED_TYPE_BUTT);
    return static_cast<BatteryPluggedType>(pluggedType);
}

int32_t BatterySrvProxy::GetVoltage()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, INVALID_BATT_INT_VALUE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return INVALID_BATT_INT_VALUE;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_VOLTAGE),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return INVALID_BATT_INT_VALUE;
    }
    int32_t voltage = INVALID_BATT_INT_VALUE;
    READ_PARCEL_WITH_RET(reply, Int32, voltage, INVALID_BATT_INT_VALUE);
    return voltage;
}

bool BatterySrvProxy::GetPresent()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, INVALID_BATT_BOOL_VALUE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return INVALID_BATT_BOOL_VALUE;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_PRESENT),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return INVALID_BATT_BOOL_VALUE;
    }
    bool present = INVALID_BATT_BOOL_VALUE;
    READ_PARCEL_WITH_RET(reply, Bool, present, INVALID_BATT_BOOL_VALUE);
    return present;
}

std::string BatterySrvProxy::GetTechnology()
{
    std::string technology = INVALID_STRING_VALUE;
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, INVALID_STRING_VALUE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return INVALID_STRING_VALUE;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_TECHNOLOGY),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return INVALID_STRING_VALUE;
    }
    READ_PARCEL_WITH_RET(reply, String, technology, INVALID_STRING_VALUE);
    return technology;
}

int32_t BatterySrvProxy::GetBatteryTemperature()
{
    sptr<IRemoteObject> remote = Remote();
    RETURN_IF_WITH_RET(remote == nullptr, INVALID_BATT_TEMP_VALUE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor())) {
        POWER_HILOGE(MODULE_INNERKIT, "BatterySrvProxy::%{public}s write descriptor failed!", __func__);
        return INVALID_BATT_TEMP_VALUE;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatterySrv::BATT_GET_TEMPERATURE),
        data, reply, option);
    if (ret != ERR_OK) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvProxy::%{public}s SendRequest is failed, error code: %d",
            __func__, ret);
        return INVALID_BATT_TEMP_VALUE;
    }
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    READ_PARCEL_WITH_RET(reply, Int32, temperature, INVALID_BATT_TEMP_VALUE);
    return temperature;
}
} // namespace PowerMgr
} // namespace OHOS

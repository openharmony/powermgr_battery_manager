/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_srv_stub.h"

#include "errors.h"
#include "xcollie/xcollie.h"
#include "xcollie/xcollie_define.h"
#include "ipc_object_stub.h"
#include "message_parcel.h"
#include "battery_info.h"
#include "battery_log.h"
#include "battery_manager_ipc_interface_code.h"
#include "power_mgr_errors.h"
#include "power_common.h"
#include "string_ex.h"

namespace OHOS {
namespace PowerMgr {
int BatterySrvStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = BatterySrvStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "Descriptor is not matched");
        return E_GET_POWER_SERVICE_FAILED;
    }

    const int DFX_DELAY_MS = 10000;
    int id = HiviewDFX::XCollie::GetInstance().SetTimer("BatteryManagerCallbackStub", DFX_DELAY_MS, nullptr, nullptr,
        HiviewDFX::XCOLLIE_FLAG_NOOP);
    int32_t ret = CheckRequestCode(code, data, reply, option);
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
    return ret;
}

int32_t BatterySrvStub::CheckRequestCode(const uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    switch (code) {
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_CAPACITY):
            return GetCapacityStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_CHARGING_STATUS):
            return GetChargingStatusStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_HEALTH_STATUS):
            return GetHealthStatusStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_PLUG_TYPE):
            return GetPluggedTypeStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_VOLTAGE):
            return GetVoltageStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_PRESENT):
            return GetPresentStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_TEMPERATURE):
            return GetBatteryTemperatureStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_LEVEL):
            return GetBatteryCapacityLevelStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_REMAINING_CHARGE_TIME):
            return GetRemainingChargeTimeStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_TECHNOLOGY):
            return GetTechnologyStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_CURRENT_NOW):
            return GetNowCurrentStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_REMAIN_ENERGY):
            return GetRemainEnergyStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_TOTAL_ENERGY):
            return GetTotalEnergyStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_CURRENT_AVERAGE):
            return GetCurrentAverageStub(reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::SET_BATTERY_CONFIG):
            return SetChargeConfigStub(data, reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::GET_BATTERY_CONFIG):
            return GetChargeConfigStub(data, reply);
        case static_cast<int>(PowerMgr::BatterySrvInterfaceCode::SUPPORT_BATTERY_CONFIG):
            return SupportChargeConfigStub(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t BatterySrvStub::GetCapacityStub(MessageParcel& reply)
{
    int32_t ret = GetCapacity();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetChargingStatusStub(MessageParcel& reply)
{
    BatteryChargeState ret = GetChargingStatus();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetHealthStatusStub(MessageParcel& reply)
{
    BatteryHealthState ret = GetHealthStatus();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetPluggedTypeStub(MessageParcel& reply)
{
    BatteryPluggedType ret = GetPluggedType();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetVoltageStub(MessageParcel& reply)
{
    int32_t ret = GetVoltage();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetPresentStub(MessageParcel& reply)
{
    bool ret = GetPresent();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Bool, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetTechnologyStub(MessageParcel& reply)
{
    std::u16string ret = Str8ToStr16(GetTechnology());
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, String16, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetBatteryTemperatureStub(MessageParcel& reply)
{
    int32_t ret = GetBatteryTemperature();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetBatteryCapacityLevelStub(MessageParcel& reply)
{
    BatteryCapacityLevel ret = GetCapacityLevel();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Uint32, static_cast<uint32_t>(ret), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int64_t BatterySrvStub::GetRemainingChargeTimeStub(MessageParcel& reply)
{
    int64_t ret = GetRemainingChargeTime();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int64, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
int32_t BatterySrvStub::GetNowCurrentStub(MessageParcel& reply)
{
    int32_t ret = GetNowCurrent();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
int32_t BatterySrvStub::GetRemainEnergyStub(MessageParcel& reply)
{
    int32_t ret = GetRemainEnergy();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
int32_t BatterySrvStub::GetTotalEnergyStub(MessageParcel& reply)
{
    int32_t ret = GetTotalEnergy();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
int32_t BatterySrvStub::GetCurrentAverageStub(MessageParcel& reply)
{
    int32_t ret = GetCurrentAverage();
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::SetChargeConfigStub(MessageParcel& data, MessageParcel& reply)
{
    std::u16string sceneName;
    RETURN_IF_READ_PARCEL_FAILED_WITH_RET(data, String16, sceneName, E_READ_PARCEL_ERROR);
    std::string tempSceneName = Str16ToStr8(sceneName);

    std::u16string value;
    RETURN_IF_READ_PARCEL_FAILED_WITH_RET(data, String16, value, E_READ_PARCEL_ERROR);
    std::string tempValue = Str16ToStr8(value);

    int32_t ret = SetBatteryConfig(tempSceneName, tempValue);
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Int32, ret, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::GetChargeConfigStub(MessageParcel& data, MessageParcel& reply)
{
    std::u16string sceneName;
    RETURN_IF_READ_PARCEL_FAILED_WITH_RET(data, String16, sceneName, E_READ_PARCEL_ERROR);
    std::string tempSceneName = Str16ToStr8(sceneName);

    std::string result = GetBatteryConfig(tempSceneName);
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, String16, Str8ToStr16(result), E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}

int32_t BatterySrvStub::SupportChargeConfigStub(MessageParcel& data, MessageParcel& reply)
{
    std::u16string sceneName;
    RETURN_IF_READ_PARCEL_FAILED_WITH_RET(data, String16, sceneName, E_READ_PARCEL_ERROR);
    std::string tempSceneName = Str16ToStr8(sceneName);

    bool result = IsBatteryConfigSupported(tempSceneName);
    RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(reply, Bool, result, E_WRITE_PARCEL_ERROR);
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.batteryInfo.proj.hpp"
#include "ohos.batteryInfo.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "battery_srv_errors.h"
#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_log.h"

#include <cstdint>
#include <cstdio>
#include <string>
#include <map>

using namespace taihe;
using namespace ohos::batteryInfo;
using namespace OHOS::PowerMgr;

namespace {
std::map<BatteryError, std::string> g_errorTable = {
    {BatteryError::ERR_CONNECTION_FAIL,   "Connecting to the service failed."},
    {BatteryError::ERR_PERMISSION_DENIED, "Permission is denied"             },
    {BatteryError::ERR_SYSTEM_API_DENIED, "System permission is denied"      },
    {BatteryError::ERR_PARAM_INVALID,     "Invalid input parameter."         }
};
thread_local static BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();

int32_t SetBatteryConfig(taihe::string_view sceneName, taihe::string_view sceneValue)
{
    BatteryError code = g_battClient.SetBatteryConfig(std::string(sceneName), std::string(sceneValue));
    BATTERY_HILOGI(FEATURE_BATT_INFO, "set charge config, sceneName: %{public}s, value: %{public}s, ret: %{public}d",
        sceneName.c_str(), sceneValue.c_str(), static_cast<int32_t>(code));
    if (code != BatteryError::ERR_OK && code != BatteryError::ERR_FAILURE) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
        return static_cast<int32_t>(code);
    }
    return static_cast<int32_t>(code);
}

taihe::string GetBatteryConfig(taihe::string_view sceneName)
{
    BATTERY_HILOGD(COMP_FWK, "get charge config, sceneName: %{public}s", sceneName.c_str());

    std::string result;
    BatteryError code = g_battClient.GetBatteryConfig(std::string(sceneName), result);
    BATTERY_HILOGD(COMP_FWK, "get charge config, sceneValue: %{public}s", result.c_str());
    if (code != BatteryError::ERR_OK && code != BatteryError::ERR_FAILURE) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
        return taihe::string(result);
    }
    return taihe::string(result);
}

bool IsBatteryConfigSupported(taihe::string_view sceneName)
{
    BATTERY_HILOGI(COMP_FWK, "get support charge config, featureName: %{public}s", sceneName.c_str());

    bool result = false;
    BatteryError code = g_battClient.IsBatteryConfigSupported(std::string(sceneName), result);

    BATTERY_HILOGI(COMP_FWK, "get support charge config, sceneValue: %{public}d", static_cast<uint32_t>(result));
    if (code != BatteryError::ERR_OK && code != BatteryError::ERR_FAILURE) {
        taihe::set_business_error(static_cast<int32_t>(code), g_errorTable[code]);
        return result;
    }
    return result;
}

int32_t BatterySOC()
{
    int32_t capacity = g_battClient.GetCapacity();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "capacity %{public}d", capacity);
    return capacity;
}

ohos::batteryInfo::BatteryChargeState ChargingStatus()
{
    ohos::batteryInfo::BatteryChargeState chargingState = ohos::batteryInfo::BatteryChargeState::key_t::NONE;
    OHOS::PowerMgr::BatteryChargeState chargeStateNative = g_battClient.GetChargingStatus();
    switch (chargeStateNative) {
        case OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_NONE:
            chargingState = ohos::batteryInfo::BatteryChargeState::key_t::NONE;
            break;
        case OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE:
            chargingState = ohos::batteryInfo::BatteryChargeState::key_t::ENABLE;
            break;
        case OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE:
            chargingState = ohos::batteryInfo::BatteryChargeState::key_t::DISABLE;
            break;
        case OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL:
            chargingState = ohos::batteryInfo::BatteryChargeState::key_t::FULL;
            break;
        default:
            BATTERY_HILOGE(FEATURE_BATT_INFO, "Unknown chargingState");
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "chargingState %{public}d", static_cast<int32_t>(chargeStateNative));
    return chargingState;
}

ohos::batteryInfo::BatteryHealthState HealthStatus()
{
    ohos::batteryInfo::BatteryHealthState healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::UNKNOWN;
    OHOS::PowerMgr::BatteryHealthState healthStatusNative = g_battClient.GetHealthStatus();
    switch (healthStatusNative) {
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::UNKNOWN;
            break;
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::GOOD;
            break;
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERHEAT:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::OVERHEAT;
            break;
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERVOLTAGE:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::OVERVOLTAGE;
            break;
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::COLD;
            break;
        case OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_DEAD:
            healthStatus = ohos::batteryInfo::BatteryHealthState::key_t::DEAD;
            break;
        default:
            BATTERY_HILOGE(FEATURE_BATT_INFO, "Unknown healthStatus");
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "healthStatus %{public}d", static_cast<int32_t>(healthStatusNative));
    return healthStatus;
}

ohos::batteryInfo::BatteryPluggedType PluggedType()
{
    ohos::batteryInfo::BatteryPluggedType pluggedType = ohos::batteryInfo::BatteryPluggedType::key_t::NONE;
    OHOS::PowerMgr::BatteryPluggedType pluggedTypeNative = g_battClient.GetPluggedType();
    switch (pluggedTypeNative) {
        case OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE:
            pluggedType = ohos::batteryInfo::BatteryPluggedType::key_t::NONE;
            break;
        case OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC:
            pluggedType = ohos::batteryInfo::BatteryPluggedType::key_t::AC;
            break;
        case OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB:
            pluggedType = ohos::batteryInfo::BatteryPluggedType::key_t::USB;
            break;
        case OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_WIRELESS:
            pluggedType = ohos::batteryInfo::BatteryPluggedType::key_t::WIRELESS;
            break;
        default:
            BATTERY_HILOGE(FEATURE_BATT_INFO, "Unknown pluggedType");
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "pluggedType %{public}d", static_cast<int32_t>(pluggedTypeNative));
    return pluggedType;
}

int32_t Voltage()
{
    int32_t voltage = g_battClient.GetVoltage();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "voltage %{public}d", voltage);
    return voltage;
}

taihe::string Technology()
{
    auto technology = g_battClient.GetTechnology();
    const char* technologyStr = technology.c_str();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "technology %{public}s", technologyStr);
    return taihe::string(technology);
}

int32_t BatteryTemperature()
{
    int32_t temperature = g_battClient.GetBatteryTemperature();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "temperature %{public}d", temperature);
    return temperature;
}

bool IsBatteryPresent()
{
    bool present = g_battClient.GetPresent();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "present %{public}d", present);
    return present;
}

ohos::batteryInfo::BatteryCapacityLevel GetCapacityLevel()
{
    ohos::batteryInfo::BatteryCapacityLevel level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_NONE;
    OHOS::PowerMgr::BatteryCapacityLevel levelNative = g_battClient.GetCapacityLevel();
    switch (levelNative) {
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NONE:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_NONE;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_FULL:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_FULL;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_HIGH:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_HIGH;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NORMAL:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_NORMAL;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_LOW:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_LOW;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_WARNING:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_WARNING;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_CRITICAL:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_CRITICAL;
            break;
        case OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_SHUTDOWN:
            level = ohos::batteryInfo::BatteryCapacityLevel::key_t::LEVEL_SHUTDOWN;
            break;
        default:
            BATTERY_HILOGE(FEATURE_BATT_INFO, "Unknown batteryCapacityLevel");
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "batteryCapacityLevel %{public}d", static_cast<int32_t>(levelNative));
    return level;
}

int64_t EstimatedRemainingChargeTime()
{
    int64_t time = g_battClient.GetRemainingChargeTime();
    return time;
}

int32_t TotalEnergy()
{
    int32_t totalEnergy = g_battClient.GetTotalEnergy();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "totalEnergy %{public}d", totalEnergy);
    return totalEnergy;
}

int32_t NowCurrent()
{
    int32_t curNow = g_battClient.GetNowCurrent();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "curNow %{public}d", curNow);
    return curNow;
}

int32_t RemainingEnergy()
{
    int32_t remainEnergy = g_battClient.GetRemainEnergy();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "remainEnergy %{public}d", remainEnergy);
    return remainEnergy;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive
// NOLINTBEGIN
TH_EXPORT_CPP_API_SetBatteryConfig(SetBatteryConfig);
TH_EXPORT_CPP_API_GetBatteryConfig(GetBatteryConfig);
TH_EXPORT_CPP_API_IsBatteryConfigSupported(IsBatteryConfigSupported);
TH_EXPORT_CPP_API_BatterySOC(BatterySOC);
TH_EXPORT_CPP_API_ChargingStatus(ChargingStatus);
TH_EXPORT_CPP_API_HealthStatus(HealthStatus);
TH_EXPORT_CPP_API_PluggedType(PluggedType);
TH_EXPORT_CPP_API_Voltage(Voltage);
TH_EXPORT_CPP_API_Technology(Technology);
TH_EXPORT_CPP_API_BatteryTemperature(BatteryTemperature);
TH_EXPORT_CPP_API_IsBatteryPresent(IsBatteryPresent);
TH_EXPORT_CPP_API_GetCapacityLevel(GetCapacityLevel);
TH_EXPORT_CPP_API_EstimatedRemainingChargeTime(EstimatedRemainingChargeTime);
TH_EXPORT_CPP_API_TotalEnergy(TotalEnergy);
TH_EXPORT_CPP_API_NowCurrent(NowCurrent);
TH_EXPORT_CPP_API_RemainingEnergy(RemainingEnergy);
// NOLINTEND
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
std::map<BatteryError, std::string> errorTable = {
    {BatteryError::ERR_CONNECTION_FAIL,   "Connecting to the service failed."},
    {BatteryError::ERR_PERMISSION_DENIED, "Permission is denied"             },
    {BatteryError::ERR_SYSTEM_API_DENIED, "System permission is denied"      },
    {BatteryError::ERR_PARAM_INVALID,     "Invalid input parameter."         }
};
thread_local static BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();

int32_t SetBatteryConfig(string_view sceneName, string_view sceneValue)
{
    BatteryError code = g_battClient.SetBatteryConfig(std::string(sceneName), std::string(sceneValue));
    BATTERY_HILOGI(FEATURE_BATT_INFO, "set charge config, sceneName: %{public}s, value: %{public}s, ret: %{public}d",
        sceneName.c_str(), sceneValue.c_str(), static_cast<int32_t>(code));
    if (code != BatteryError::ERR_OK) {
        taihe::set_business_error(static_cast<int32_t>(code), errorTable[code]);
        return static_cast<int32_t>(code);
    }
    return static_cast<int32_t>(code);
}

string GetBatteryConfig(string_view sceneName)
{
    BATTERY_HILOGD(COMP_FWK, "get charge config, sceneName: %{public}s", sceneName.c_str());

    std::string result;
    BatteryError code = g_battClient.GetBatteryConfig(std::string(sceneName), result);
    BATTERY_HILOGD(COMP_FWK, "get charge config, sceneValue: %{public}s", result.c_str());
    if (code != BatteryError::ERR_OK) {
        taihe::set_business_error(static_cast<int32_t>(code), errorTable[code]);
        return string(result);
    }
    return string(result);
}

bool IsBatteryConfigSupported(string_view sceneName)
{
    BATTERY_HILOGI(COMP_FWK, "get support charge config, featureName: %{public}s", sceneName.c_str());

    bool result = false;
    BatteryError code = g_battClient.IsBatteryConfigSupported(std::string(sceneName), result);

    BATTERY_HILOGI(COMP_FWK, "get support charge config, sceneValue: %{public}d", static_cast<uint32_t>(result));
    if (code != BatteryError::ERR_OK) {
        taihe::set_business_error(static_cast<int32_t>(code), errorTable[code]);
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
    ohos::batteryInfo::BatteryChargeState chargingState =
        static_cast<ohos::batteryInfo::BatteryChargeState::key_t>(g_battClient.GetChargingStatus());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "chargingState %{public}d", static_cast<int32_t>(chargingState));
    return chargingState;
}

ohos::batteryInfo::BatteryHealthState HealthStatus()
{
    ohos::batteryInfo::BatteryHealthState healthStatus =
        static_cast<ohos::batteryInfo::BatteryHealthState::key_t>(g_battClient.GetHealthStatus());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "healthStatus %{public}d", static_cast<int32_t>(healthStatus));
    return healthStatus;
}

ohos::batteryInfo::BatteryPluggedType PluggedType()
{
    ohos::batteryInfo::BatteryPluggedType pluggedType =
        static_cast<ohos::batteryInfo::BatteryPluggedType::key_t>(g_battClient.GetPluggedType());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "pluggedType %{public}d", static_cast<int32_t>(pluggedType));
    return pluggedType;
}

int32_t Voltage()
{
    int32_t voltage = g_battClient.GetVoltage();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "voltage %{public}d", voltage);
    return voltage;
}

string Technology()
{
    auto technology = g_battClient.GetTechnology();
    const char* technologyStr = technology.c_str();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "technology %{public}s", technologyStr);
    return string(technology);
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
    ohos::batteryInfo::BatteryCapacityLevel batteryCapacityLevel =
        static_cast<ohos::batteryInfo::BatteryCapacityLevel::key_t>(g_battClient.GetCapacityLevel());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "batteryCapacityLevel %{public}d", static_cast<int32_t>(batteryCapacityLevel));
    return batteryCapacityLevel;
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

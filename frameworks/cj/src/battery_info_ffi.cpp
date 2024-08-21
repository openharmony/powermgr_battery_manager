/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "battery_info_ffi.h"
#include "battery_log.h"
#include "battery_srv_client.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace CJSystemapi {
namespace BatteryInfo {
    using namespace OHOS::PowerMgr;

    thread_local static BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();

extern "C" {
int32_t FfiBatteryInfoBatterySOC()
{
    int32_t capacity = g_battClient.GetCapacity();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "capacity %{public}d", capacity);
    return capacity;
}

int32_t FfiBatteryInfoGetChargingState()
{
    int32_t chargingState = static_cast<int32_t>(g_battClient.GetChargingStatus());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "chargingState %{public}d", chargingState);
    return chargingState;
}

int32_t FfiBatteryInfoGetHealthState()
{
    int32_t healthStatus = static_cast<int32_t>(g_battClient.GetHealthStatus());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "healthStatus %{public}d", healthStatus);
    return healthStatus;
}

int32_t FfiBatteryInfoGetPluggedType()
{
    int32_t pluggedType = static_cast<int32_t>(g_battClient.GetPluggedType());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "pluggedType %{public}d", pluggedType);
    return pluggedType;
}

int32_t FfiBatteryInfoGetVoltage()
{
    int32_t voltage = g_battClient.GetVoltage();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "voltage %{public}d", voltage);
    return voltage;
}

int32_t FfiBatteryInfoGetBatteryNowCurrent()
{
    int32_t curNow = g_battClient.GetNowCurrent();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "curNow %{public}d", curNow);
    return curNow;
}

const char* FfiBatteryInfoGetTechnology()
{
    auto technology = g_battClient.GetTechnology();
    if (technology.empty()) {
        return nullptr;
    }
    auto len = technology.length() + 1;
    char *value = static_cast<char *>(malloc(sizeof(char) * len));
    if (value == nullptr) {
        return nullptr;
    }
    std::char_traits<char>::copy(value, technology.c_str(), len);
    BATTERY_HILOGD(FEATURE_BATT_INFO, "technology %{public}s", value);
    return value;
}

int32_t FfiBatteryInfoGetBatteryTemperature()
{
    int32_t temperature = g_battClient.GetBatteryTemperature();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "temperature %{public}d", temperature);
    return temperature;
}

bool FfiBatteryInfoGetBatteryPresent()
{
    bool present = g_battClient.GetPresent();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "present %{public}d", present);
    return present;
}

int32_t FfiBatteryInfoGetCapacityLevel()
{
    int32_t batteryCapacityLevel = static_cast<int32_t>(g_battClient.GetCapacityLevel());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "batteryCapacityLevel %{public}d", batteryCapacityLevel);
    return batteryCapacityLevel;
}
}

} // namespace BatteryInfo
} // namespace CJSystemapi
} // namespace OHOS
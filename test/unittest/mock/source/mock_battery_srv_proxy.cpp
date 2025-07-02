/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "mock_battery_srv_proxy.h"
namespace {
constexpr int32_t ERR_FAIL = -1;
}
namespace OHOS {
namespace PowerMgr {
int32_t MockBatterySrvProxy::GetCapacity(int32_t& capacity)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetChargingStatus(uint32_t& chargeState)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetHealthStatus(uint32_t& healthState)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetPluggedType(uint32_t& pluggedType)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetVoltage(int32_t& voltage)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetPresent(bool& present)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetTechnology(std::string& technology)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetTotalEnergy(int32_t& totalEnergy)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetCurrentAverage(int32_t& curAverage)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetNowCurrent(int32_t& nowCurr)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetRemainEnergy(int32_t& remainEnergy)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetBatteryTemperature(int32_t& temperature)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetCapacityLevel(uint32_t& batteryCapacityLevel)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetRemainingChargeTime(int64_t& remainTime)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::SetBatteryConfig(const std::string& sceneName, const std::string& value,
    int32_t& batteryErr)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::GetBatteryConfig(const std::string& sceneName, std::string& getResult,
    int32_t& batteryErr)
{
    return ERR_FAIL;
}

int32_t MockBatterySrvProxy::IsBatteryConfigSupported(const std::string& featureName, bool& isResult,
    int32_t& batteryErr)
{
    return ERR_FAIL;
}
} // namespace PowerMgr
} // namespace OHOS

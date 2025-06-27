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

#ifndef MOCK_BATTERY_SRV_PROXY_H
#define MOCK_BATTERY_SRV_PROXY_H

#include "battery_srv_proxy.h"

namespace OHOS {
namespace PowerMgr {
class MockBatterySrvProxy : public BatterySrvProxy {
public:
    explicit MockBatterySrvProxy(const sptr<IRemoteObject>& remote)
        : BatterySrvProxy(remote)
    {}
    ~MockBatterySrvProxy() {};
    int32_t GetCapacity(int32_t& capacity) override;
    int32_t GetChargingStatus(uint32_t& chargeState) override;
    int32_t GetHealthStatus(uint32_t& healthState) override;
    int32_t GetPluggedType(uint32_t& pluggedType) override;
    int32_t GetVoltage(int32_t& voltage) override;
    int32_t GetPresent(bool& present) override;
    int32_t GetTechnology(std::string& technology) override;
    int32_t GetTotalEnergy(int32_t& totalEnergy) override;
    int32_t GetCurrentAverage(int32_t& curAverage) override;
    int32_t GetNowCurrent(int32_t& nowCurr) override;
    int32_t GetRemainEnergy(int32_t& remainEnergy) override;
    int32_t GetBatteryTemperature(int32_t& temperature) override;
    int32_t GetCapacityLevel(uint32_t& batteryCapacityLevel) override;
    int32_t GetRemainingChargeTime(int64_t& remainTime) override;
    int32_t SetBatteryConfig(const std::string& sceneName, const std::string& value, int32_t& batteryErr) override;
    int32_t GetBatteryConfig(const std::string& sceneName, std::string& getResult, int32_t& batteryErr) override;
    int32_t IsBatteryConfigSupported(const std::string& featureName, bool& isResult, int32_t& batteryErr) override;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // MOCK_BATTERY_SRV_PROXY_H

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

#ifndef POWERMGR_INNER_API_IBATTERY_SRV_H
#define POWERMGR_INNER_API_IBATTERY_SRV_H

#include <string>
#include "iremote_broker.h"
#include "iremote_object.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class IBatterySrv : public IRemoteBroker {
public:
    virtual int32_t GetCapacity() = 0;
    virtual BatteryChargeState GetChargingStatus() = 0;
    virtual BatteryHealthState GetHealthStatus() = 0;
    virtual BatteryPluggedType GetPluggedType() = 0;
    virtual int32_t GetVoltage() = 0;
    virtual bool GetPresent() = 0;
    virtual std::string GetTechnology() = 0;
    virtual int32_t GetTotalEnergy() = 0;
    virtual int32_t GetCurrentAverage() = 0;
    virtual int32_t GetNowCurrent() = 0;
    virtual int32_t GetRemainEnergy() = 0;
    virtual int32_t GetBatteryTemperature() = 0;
    virtual BatteryCapacityLevel GetCapacityLevel() = 0;
    virtual int64_t GetRemainingChargeTime() = 0;
    virtual int32_t SetBatteryConfig(const std::string& sceneName, const std::string& value)  = 0;
    virtual std::string GetBatteryConfig(const std::string& sceneName)  = 0;
    virtual bool IsBatteryConfigSupported(const std::string& featureName)  = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.powermgr.IBatterySrv");
};
} // space PowerMgr
} // namespace OHOS

#endif // POWERMGR_INNER_API_IBATTERY_SRV_H

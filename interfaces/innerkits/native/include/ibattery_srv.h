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

#ifndef POWERMGR_INNERKITS_IBATTERY_SRV_H
#define POWERMGR_INNERKITS_IBATTERY_SRV_H

#include <string>
#include "iremote_broker.h"
#include "iremote_object.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class IBatterySrv : public IRemoteBroker {
public:
    enum {
        BATT_GET_CAPACITY = 0,
        BATT_GET_CHARGING_STATUS,
        BATT_GET_HEALTH_STATUS,
        BATT_GET_PLUG_TYPE,
        BATT_GET_VOLTAGE,
        BATT_GET_PRESENT,
        BATT_GET_TEMPERATURE,
        BATT_GET_TECHNOLOGY,
        BATT_GET_BATTERY_LEVEL,
        BATT_GET_REMAINING_CHARGE_TIME,
        BATT_GET_BATTERY_TOTAL_ENERGY,
        BATT_GET_BATTERY_CURRENT_AVERAGE,
        BATT_GET_BATTERY_CURRENT_NOW,
        BATT_GET_BATTERY_REMAIN_ENERGY
    };

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
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.powermgr.IBatterySrv");
};
} // space PowerMgr
} // namespace OHOS

#endif // POWERMGR_INNERKITS_IBATTERY_SRV_H

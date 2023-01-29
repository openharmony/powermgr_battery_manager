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

#ifndef POWERMGR_BATTERY_SRV_CLIENT_H
#define POWERMGR_BATTERY_SRV_CLIENT_H

#include <singleton.h>
#include <memory>
#include <mutex>
#include "iremote_object.h"
#include "ibattery_srv.h"

namespace OHOS {
namespace PowerMgr {
class BatterySrvClient final : public DelayedRefSingleton<BatterySrvClient> {
    DECLARE_DELAYED_REF_SINGLETON(BatterySrvClient)

public:
    DISALLOW_COPY_AND_MOVE(BatterySrvClient);

    /**
     * Return the capacity of the battery, in percent.
     */
    int32_t GetCapacity();
    /**
     * Return the charging status, such as CHARGE_STATE_NONE, CHARGE_STATE_ENABLE,
     * CHARGE_STATE_DISABLE, CHARGE_STATE_FULL,...
     */
    BatteryChargeState GetChargingStatus();
    /**
     * Return the Health state of the battery, such as HEALTH_STATE_UNKNOWN,
     * HEALTH_STATE_GOOD, HEALTH_STATE_OVERHEAT,....
     */
    BatteryHealthState GetHealthStatus();
    /**
     * Return the charger type plugged, such as PLUGGED_TYPE_NONE,
     * PLUGGED_TYPE_AC, PLUGGED_TYPE_USB,....
     */
    BatteryPluggedType GetPluggedType();
    /**
     * Return the voltage of the battery, in mv.
     */
    int32_t GetVoltage();
    /**
     * Return the present state of the battery, true or false.
     */
    bool GetPresent();
    /**
     * Return the technology of the battery, such as Li-ion.
     */
    std::string GetTechnology();
    /**
     * Return the temperature of the battery, in 0.1℃.
     */
    int32_t GetBatteryTemperature();
     /**
     * Return the Current of the battery, in mA.
     */
    int32_t GetNowCurrent();
    /**
     * Return the RemainEnergy of the battery, in mAh.
     */
    int32_t GetRemainEnergy();
    /**
     * Return the GetTotalEnergy of the battery, in mAh.
     */
    int32_t GetTotalEnergy();
    /**
     * Return the level of the battery
     */
    BatteryCapacityLevel GetCapacityLevel();
    /**
     * Return the remaining charge time
     */
    int64_t GetRemainingChargeTime();

#ifndef BATTERYMGR_DEATHRECIPIENT_UNITTEST
private:
#endif
    class BatterySrvDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        BatterySrvDeathRecipient() = default;
        ~BatterySrvDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        DISALLOW_COPY_AND_MOVE(BatterySrvDeathRecipient);
    };

    ErrCode Connect();
    sptr<IBatterySrv> proxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    void ResetProxy(const wptr<IRemoteObject>& remote);
    std::mutex mutex_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_SRV_CLIENT_H

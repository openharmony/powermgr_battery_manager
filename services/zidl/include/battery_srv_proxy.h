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

#ifndef BATTERY_SRV_PROXY_H
#define BATTERY_SRV_PROXY_H

#include <cstdint>
#include <functional>
#include <iosfwd>
#include "refbase.h"
#include "nocopyable.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "ibattery_srv.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class BatterySrvProxy : public IRemoteProxy<IBatterySrv> {
public:
    explicit BatterySrvProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBatterySrv>(impl) {}
    ~BatterySrvProxy() = default;
    DISALLOW_COPY_AND_MOVE(BatterySrvProxy);

    virtual int32_t GetCapacity() override;
    virtual BatteryChargeState GetChargingStatus() override;
    virtual BatteryHealthState GetHealthStatus() override;
    virtual BatteryPluggedType GetPluggedType() override;
    virtual int32_t GetVoltage() override;
    virtual bool GetPresent() override;
    virtual std::string GetTechnology() override;
    virtual int32_t GetTotalEnergy() override;
    virtual int32_t GetCurrentAverage() override;
    virtual int32_t GetNowCurrent() override;
    virtual int32_t GetRemainEnergy() override;
    virtual int32_t GetBatteryTemperature() override;
    virtual BatteryCapacityLevel GetCapacityLevel() override;
    virtual int64_t GetRemainingChargeTime() override;
private:
    static inline BrokerDelegator<BatterySrvProxy> delegator_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_SRV_PROXY_H

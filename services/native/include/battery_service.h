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

#ifndef POWERMGR_BATTERY_SERVICE_H
#define POWERMGR_BATTERY_SERVICE_H

#include "sp_singleton.h"
#include "system_ability.h"
#include "iremote_object.h"
#include "ibattery_srv.h"
#include "batteryd_api.h"
#include "batteryd_client.h"
#include "batteryd_subscriber.h"
#include "battery_service_subscriber.h"
#include "battery_srv_stub.h"
#include "batterysrv_event_handler.h"

namespace OHOS {
namespace PowerMgr {
class BatteryService final : public SystemAbility,
    public BatterySrvStub {
DECLARE_SYSTEM_ABILITY(BatteryService)

DECLARE_DELAYED_SP_SINGLETON(BatteryService);
public:
    virtual void OnStart() override;
    virtual void OnStop() override;

    bool IsServiceReady() const
    {
        return ready_;
    }

    std::shared_ptr<BatterysrvEventHandler> GetHandler() const
    {
        return handler_;
    }

    int32_t GetCapacity() override;
    BatteryChargeState GetChargingStatus() override;
    BatteryHealthState GetHealthStatus() override;
    BatteryPluggedType GetPluggedType() override;
    int32_t GetVoltage() override;
    bool GetPresent() override;
    std::string GetTechnology() override;
    int32_t GetBatteryTemperature() override;

private:
    bool Init();
    bool InitBatteryd();
    bool IsCommonEventServiceAbilityExist();
    bool ready_ {false};
    int32_t commEventRetryTimes_ {0};
    std::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner_;
    std::shared_ptr<BatterysrvEventHandler> handler_;
    sptr<BatteryServiceSubscriber> batterydSubscriber_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // POWERMGR_BATTERY_SERVICE_H

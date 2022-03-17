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
#include "battery_service_subscriber.h"
#include "battery_srv_stub.h"
#include "battery_service_event_handler.h"
#include "battery_callback_impl.h"
#include "types.h"
#include "battery_service_subscriber.h"
#include "battery_config.h"
#include "battery_led.h"
#include "window_manager.h"

namespace OHOS {
namespace PowerMgr {
using namespace OHOS::HDI::Battery::V1_0;

class BatteryService final : public SystemAbility,
    public BatterySrvStub {
DECLARE_SYSTEM_ABILITY(BatteryService)

DECLARE_DELAYED_SP_SINGLETON(BatteryService);
public:
    virtual void OnDump() override;
    virtual void OnStart() override;
    virtual void OnStop() override;

    bool IsServiceReady() const
    {
        return ready_;
    }

    std::shared_ptr<BatteryServiceEventHandler> GetHandler() const
    {
        return handler_;
    }

    int32_t Dump(int fd, const std::vector<std::u16string> &args) override;
    int32_t GetCapacity() override;
    BatteryChargeState GetChargingStatus() override;
    BatteryHealthState GetHealthStatus() override;
    BatteryPluggedType GetPluggedType() override;
    int32_t GetVoltage() override;
    bool GetPresent() override;
    std::string GetTechnology() override;
    int32_t GetBatteryTemperature() override;
    int32_t GetBatteryLevel() override;
    int64_t GetRemainingChargeTime() override;
    void ChangePath(const std::string path);
    void InitConfig();
    void WakeupDevice(const int32_t& chargestate);
    void HandleTemperature(const int32_t& temperature);
private:
    bool Init();
    bool InitBatteryd();
    int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_0::BatteryInfo& event);
    void CalculateRemainingChargeTime(int32_t capacity);
    void HandlePopupEvent(const int32_t capacity);
    void HandleCapacity(const int32_t& capacity, const int32_t& chargeState);
    bool ShowDialog(const std::string &params);
    void GetDisplayPosition(int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height, bool& wideScreen);
    bool ready_ {false};
    int32_t commEventRetryTimes_ {0};
    std::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner_;
    std::shared_ptr<BatteryServiceEventHandler> handler_;
    sptr<BatteryServiceSubscriber> batterydSubscriber_;
    std::unique_ptr<HDI::Battery::V1_0::BatteryConfig> batteryConfig_ = nullptr;
    std::unique_ptr<HDI::Battery::V1_0::BatteryLed> batteryLed_ = nullptr;
    int32_t lastCapacity_ = 0;
    int64_t lastTime_ = 0;
    int64_t remainTime_ = 0;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // POWERMGR_BATTERY_SERVICE_H

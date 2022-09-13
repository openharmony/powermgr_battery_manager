/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <mutex>
#include <iosfwd>
#include <string>
#include <memory>
#include <vector>
#include "refbase.h"
#include "event_runner.h"
#include "sp_singleton.h"
#include "system_ability.h"
#include "hdi_service_status_listener.h"
#include "iservmgr_hdi.h"
#include "iservstat_listener_hdi.h"
#include "ibattery_srv.h"
#include "v1_1/ibattery_interface.h"
#include "v1_1/types.h"
#include "battery_info.h"
#include "battery_config.h"
#include "battery_led.h"
#include "battery_service_event_handler.h"
#include "battery_service_subscriber.h"
#include "battery_srv_stub.h"

namespace OHOS {
namespace PowerMgr {
using namespace OHOS::HDI::Battery::V1_1;
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
    int32_t GetTotalEnergy() override;
    int32_t GetCurrentAverage() override;
    int32_t GetNowCurrent() override;
    int32_t GetRemainEnergy() override;
    int32_t GetBatteryTemperature() override;
    BatteryLevel GetBatteryLevel() override;
    int64_t GetRemainingChargeTime() override;
    void ChangePath(const std::string path);
    void InitConfig();
    void WakeupDevice(BatteryChargeState chargeState);
    void HandleTemperature(int32_t temperature);
    void RegisterHdiStatusListener();
    void RegisterBatteryHdiCallback();
    void MockUnplugged(bool isPlugged);
private:
    bool Init();
    int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_1::BatteryInfo& event);
    void UpdateBatteryInfo(const OHOS::HDI::Battery::V1_1::BatteryInfo &event);
    void HandleBatteryInfo();
    void SendEvent(int32_t event, int64_t delayTime);
    void CalculateRemainingChargeTime(int32_t capacity, BatteryChargeState chargeState);
    void HandlePopupEvent(int32_t capacity);
    void HandleCapacity(int32_t capacity, BatteryChargeState chargeState);
    bool ShowDialog(const std::string &params);
    void GetDisplayPosition(int32_t& width, int32_t& height);
    bool ready_ {false};
    int32_t commEventRetryTimes_ {0};
    std::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner_;
    std::shared_ptr<BatteryServiceEventHandler> handler_;
    sptr<BatteryServiceSubscriber> batterydSubscriber_;
    std::unique_ptr<HDI::Battery::V1_1::BatteryConfig> batteryConfig_ = nullptr;
    std::unique_ptr<HDI::Battery::V1_1::BatteryLed> batteryLed_ = nullptr;
    sptr<IBatteryInterface> iBatteryInterface_ = nullptr;
    sptr<OHOS::HDI::ServiceManager::V1_0::IServiceManager> hdiServiceMgr_ { nullptr };
    sptr<HdiServiceStatusListener::IServStatListener> hdiServStatListener_ { nullptr };
    int32_t lastCapacity_ = 0;
    int64_t lastTime_ = 0;
    int64_t remainTime_ = 0;
    bool chargeFlag_ {false};
    int32_t dialogId_ = -1;
    bool isLowPower_ = false;
    BatteryInfo batteryInfo_;
    bool isMockUnplugged_ = false;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // POWERMGR_BATTERY_SERVICE_H

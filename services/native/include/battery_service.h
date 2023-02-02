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
#include "battery_light.h"
#include "battery_service_event_handler.h"
#include "battery_notify.h"
#include "battery_srv_stub.h"

namespace OHOS {
namespace PowerMgr {
class BatteryService final : public SystemAbility,
    public BatterySrvStub {
DECLARE_SYSTEM_ABILITY(BatteryService)

DECLARE_DELAYED_SP_SINGLETON(BatteryService);
public:
    virtual void OnStart() override;
    virtual void OnStop() override;
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

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
    BatteryCapacityLevel GetCapacityLevel() override;
    int64_t GetRemainingChargeTime() override;
    void ChangePath(const std::string path);
    void InitConfig();
    void HandleTemperature(int32_t temperature);
    bool RegisterHdiStatusListener();
    bool RegisterBatteryHdiCallback();
    bool IsMockUnplugged();
    void MockUnplugged(bool isUnplugged);
    bool ShowBatteryDialog();
    bool DestoryBatteryDialog();
private:
    bool Init();
    bool IsLastPlugged();
    bool IsNowPlugged(BatteryPluggedType pluggedType);
    bool IsPlugged(BatteryPluggedType pluggedType);
    bool IsUnplugged(BatteryPluggedType pluggedType);
    void WakeupDevice(BatteryPluggedType pluggedType);
    int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_1::BatteryInfo& event);
    void ConvertingEvent(const OHOS::HDI::Battery::V1_1::BatteryInfo &event);
    void HandleBatteryInfo();
    void SendEvent(int32_t event, int64_t delayTime);
    void CalculateRemainingChargeTime(int32_t capacity, BatteryChargeState chargeState);
    void HandlePopupEvent(int32_t capacity);
    void HandleCapacity(int32_t capacity, BatteryChargeState chargeState);
    bool ready_ { false };
    std::mutex mutex_;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner_ { nullptr };
    std::shared_ptr<BatteryServiceEventHandler> handler_ { nullptr };
    std::unique_ptr<BatteryNotify> batteryNotify_ { nullptr };
    BatteryLight batteryLight_;
    sptr<HDI::Battery::V1_1::IBatteryInterface> iBatteryInterface_ { nullptr };
    sptr<OHOS::HDI::ServiceManager::V1_0::IServiceManager> hdiServiceMgr_ { nullptr };
    sptr<HdiServiceStatusListener::IServStatListener> hdiServStatListener_ { nullptr };
    bool isLowPower_ { false };
    bool isMockUnplugged_ { false };
    bool chargeFlag_ { false };
    int32_t commEventRetryTimes_ { 0 };
    int32_t lastCapacity_ { 0 };
    int32_t dialogId_ { INVALID_BATT_INT_VALUE };
    int32_t warnCapacity_ { INVALID_BATT_INT_VALUE };
    int32_t highTemperature_ { INT32_MAX };
    int32_t lowTemperature_ { INT32_MIN };
    int32_t shutdownCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t criticalCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t warningCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t lowCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t normalCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t highCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int32_t fullCapacityThreshold_ = { INVALID_BATT_INT_VALUE };
    int64_t lastTime_ { 0 };
    int64_t remainTime_ { 0 };
    BatteryInfo batteryInfo_;
    BatteryInfo lastBatteryInfo_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // POWERMGR_BATTERY_SERVICE_H
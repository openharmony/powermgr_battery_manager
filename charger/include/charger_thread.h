/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef CHARGER_THREAD_H
#define CHARGER_THREAD_H

#include <thread>

#include "battery_backlight.h"
#include "battery_led.h"
#include "battery_thread.h"
#include "battery_vibrate.h"
#include "charger_animation.h"
#include <linux/input.h>
#include <input_manager.h>

namespace OHOS {
namespace PowerMgr {
class ChargerThread : public BatteryThread {
private:
    void Init();
    static void UpdateAnimation(const int32_t& chargeState, const int32_t& capacity);
    void Run(void* service) override;
    void UpdateBatteryInfo(void* arg) override;
    void HandleChargingState();
    void HandleScreenState();
    void HandleTemperature(const int32_t& temperature);
    void HandleCapacity(const int32_t& capacity);
    void HandleStates() override;
    int32_t UpdateWaitInterval() override;
    void CycleMatters() override;
    static void InitAnimation();
    void SetKeyWait(struct KeyState& key, int64_t timeout);
    void InputMonitorInit();
    void InputMonitorCancel();
    void HandlePowerKeyState();
    void HandlePowerKey(int32_t keycode, int64_t now);
    void InitLackPowerCapacity();
    void InitBatteryFileSystem();
    void InitVibration();
    void InitBacklight();
    void InitLed();
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
    std::unique_ptr<BatteryVibrate> vibrate_ = nullptr;
    std::unique_ptr<BatteryBacklight> backlight_ = nullptr;
    std::unique_ptr<BatteryLed> led_ = nullptr;

    static const int32_t INVALID = -1;
    int64_t keyWait_ = INVALID;
    int64_t backlightWait_ = INVALID;
    int32_t capacity_ = INVALID;
    int32_t chargeState_ = PowerSupplyProvider::BatteryChargeState::CHARGE_STATE_RESERVED;
    bool started_ = false;

    static std::unique_ptr<ChargerAnimation> animation_;
    static bool isChargeStateChanged_;
    static bool isConfigParse_;
    static int32_t lackPowerCapacity_;
    int32_t inputMonitorId_ {-1};
};

class ChargerThreadInputMonitor : public MMI::IInputEventConsumer {
public:
    virtual void OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const;
    virtual void OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const;
    virtual void OnInputEvent(std::shared_ptr<OHOS::MMI::AxisEvent> axisEvent) const;

private:
    void SetKeyState(int32_t code, int32_t value, int64_t now) const;
};
} // namespace PowerMgr
} // namespace OHOS
#endif

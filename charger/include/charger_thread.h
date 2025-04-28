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
#include "v1_0/iinput_interfaces.h"
#include <linux/input.h>

namespace OHOS {
namespace PowerMgr {
using namespace OHOS::HDI::Input::V1_0;
class ChargerThread : public BatteryThread {
public:
    class HdfInputEventCallback : public IInputCallback {
    public:
        HdfInputEventCallback() = default;
        ~HdfInputEventCallback() override = default;

        int32_t EventPkgCallback(const std::vector<HDI::Input::V1_0::EventPackage> &pkgs, uint32_t devIndex) override;
        int32_t HotPlugCallback(const HotPlugEvent &event) override;

    private:
        void SetKeyState(int32_t code, int32_t value, int64_t now);
        void HandleInputEvent(const struct input_event* iev);
    };

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
    static void InitInput();
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
    static sptr<HDI::Input::V1_0::IInputInterfaces> inputInterface;

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
};
} // namespace PowerMgr
} // namespace OHOS
#endif

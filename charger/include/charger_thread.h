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

#ifndef CHARGER_THREAD_H
#define CHARGER_THREAD_H

#include <thread>

#include <linux/input.h>
#include "input_type.h"
#include "battery_thread.h"
#include "battery_config.h"
#include "battery_vibrate.h"
#include "battery_backlight.h"
#include "battery_led.h"
#include "animation_label.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class ChargerThread : public BatteryThread {
public:
    friend class BatteryThreadTest;
private:
    void Init();
    static void UpdateAnimation(const int32_t& capacity);
    void Run(void* service) override;
    void UpdateBatteryInfo(void* arg, char* msg) override;
    void UpdateBatteryInfo(void* arg) override;
    void HandleChargingState();
    void HandleScreenState();
    void HandleTemperature(const int32_t& temperature);
    void HandleCapacity(const int32_t& capacity);
    void HandleStates() override;
    int UpdateWaitInterval() override;
    void CycleMatters() override;
    void AnimationInit();
    void LoadImgs(AnimationLabel* g_animationLabel);
    static void SetKeyWait(struct KeyState& key, int64_t timeout);
    static int SetKeyState(int code, int value, int64_t now);
    int InputInit();
    static void EventPkgCallback(const EventPackage** pkgs, const uint32_t count, uint32_t devIndex);
    void HandlePowerKeyState();
    void HandlePowerKey(int keycode, int64_t now);
    static void HandleInputEvent(const struct input_event* iev);
    std::unique_ptr<BatteryConfig> batteryConfig_ = nullptr;
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
    std::unique_ptr<BatteryVibrate> vibrate_ = nullptr;
    std::unique_ptr<BatteryBacklight> backlight_ = nullptr;
    std::unique_ptr<BatteryLed> led_ = nullptr;
    int64_t pluginWait_ = -1;
    static int64_t keyWait_;
    static int64_t backlightWait_;
    static int32_t capacity_;
    int32_t chargeState_ = -1;
    bool started_ = false;
};

constexpr int START_X1 = 0;
constexpr int START_Y1 = 850;
constexpr int WIDTH1 = 480;
constexpr int HEIGHT1 = 30;
constexpr int START_X2 = 0;
constexpr int START_Y2 = 900;
constexpr int WIDTH2 = 480;
constexpr int HEIGHT2 = 30;
constexpr int START_X5 = 0;
constexpr int START_Y5 = 900;
constexpr int HEIGHT5 = 100;
constexpr int START_X_SCALE = 8;
constexpr int START_Y_SCALE = 8;
constexpr int WIDTH_SCALE1 = 3;
constexpr int WIDTH_SCALE2 = 4;
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif

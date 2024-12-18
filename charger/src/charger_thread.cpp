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

#include "charger_thread.h"
#include "battery_config.h"
#include "charger_log.h"
#include "charger_animation.h"
#include "init_reboot.h"
#include <cstdint>
#include <input_manager.h>
#include <cinttypes>
#include <linux/netlink.h>
#include <parameters.h>
#include <securec.h>
#include <dlfcn.h>

using namespace OHOS::MMI;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr int32_t SEC_TO_MSEC = 1000;
constexpr int32_t NSEC_TO_MSEC = 1000000;
constexpr int32_t REBOOT_TIME = 2000;
constexpr int32_t BACKLIGHT_OFF_TIME_MS = 10000;
constexpr int32_t VIBRATE_TIME_MS = 75;
const std::string REBOOT_CMD = "";
const std::string SHUTDOWN_CMD = "shutdown";
constexpr int32_t KEY_ACTION_DOWN_VAL = 1;
constexpr int32_t KEY_ACTION_UP_VAL = 0;
} // namespace

std::unique_ptr<ChargerAnimation> ChargerThread::animation_ = nullptr;
bool ChargerThread::isChargeStateChanged_ = false;
bool ChargerThread::isConfigParse_ = false;
int32_t ChargerThread::lackPowerCapacity_ = -1;

struct KeyState {
    bool isUp;
    bool isDown;
    int64_t timestamp;
};
struct KeyState g_keys[KEY_MAX + 1] = {};

static const char* POWER_CHARGE_EXTENSION_PATH = "libpower_charge_ext.z.so";
static const char* CLEAR_POWER_OFF_CHARGE_FLAG_FUNC = "ClearPowerOffChargeFlag";
typedef void(*Func)();

static void ClearPowerOffChargeFlag()
{
    BATTERY_HILOGI(FEATURE_CHARGING, "enter ClearPowerOffChargeFlag");
    void *handler = dlopen(POWER_CHARGE_EXTENSION_PATH, RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "Dlopen failed, reason : %{public}s", dlerror());
        return;
    }

    Func clearPowerOffChargeFlag = (Func)dlsym(handler, CLEAR_POWER_OFF_CHARGE_FLAG_FUNC);
    if (clearPowerOffChargeFlag == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "find function failed, reason : %{public}s", dlerror());
        dlclose(handler);
        return;
    }
    clearPowerOffChargeFlag();
    dlclose(handler);
}

static int64_t GetCurrentTime()
{
    timespec tm {};
    clock_gettime(CLOCK_MONOTONIC, &tm);
    return tm.tv_sec * SEC_TO_MSEC + (tm.tv_nsec / NSEC_TO_MSEC);
}

void ChargerThreadInputMonitor::SetKeyState(int32_t code, int32_t value, int64_t now) const
{
    bool isDown = !!value;

    if (code > KEY_MAX) {
        BATTERY_HILOGW(FEATURE_CHARGING, "code lager than KEY_MAX: %{public}d", code);
        return;
    }

    if (g_keys[code].isDown == isDown) {
        BATTERY_HILOGW(FEATURE_CHARGING, "PowerKey is already down");
        return;
    }

    if (isDown) {
        g_keys[code].timestamp = now;
    }

    g_keys[code].isDown = isDown;
    g_keys[code].isUp = true;
}

void ChargerThread::HandleStates()
{
    HandleChargingState();
    HandlePowerKeyState();
    HandleScreenState();
}

int32_t ChargerThread::UpdateWaitInterval()
{
    int64_t currentTime = GetCurrentTime();
    int64_t nextWait = INT64_MAX;
    int64_t timeout = INVALID;

    if (keyWait_ != INVALID && keyWait_ < nextWait) {
        nextWait = keyWait_;
    }

    if (backlightWait_ != INVALID && backlightWait_ < nextWait) {
        nextWait = backlightWait_;
    }

    if (nextWait != INVALID && nextWait != INT64_MAX) {
        if (nextWait - currentTime > 0) {
            timeout = nextWait - currentTime;
        } else {
            timeout = 0;
        }
    }

    return static_cast<int32_t>(timeout);
}

void ChargerThread::CycleMatters()
{
    if (!started_) {
        started_ = true;
        backlightWait_ = GetCurrentTime() - 1;
    }

    UpdateBatteryInfo(nullptr);
    BATTERY_HILOGD(FEATURE_CHARGING, "chargeState_=%{public}d, capacity_=%{public}d", chargeState_, capacity_);
    UpdateEpollInterval(chargeState_);
}

void ChargerThread::UpdateBatteryInfo(void* arg)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update battery info by provider");
    int32_t temperature = 0;
    provider_->ParseTemperature(&temperature);
    provider_->ParseCapacity(&capacity_);
    int32_t oldChargeState = chargeState_;
    provider_->ParseChargeState(&chargeState_);
    BATTERY_HILOGD(FEATURE_CHARGING, "temperature=%{public}d, capacity_=%{public}d, chargeState_=%{public}d",
        temperature, capacity_, chargeState_);
    if (chargeState_ != oldChargeState) {
        isChargeStateChanged_ = true;
    } else {
        isChargeStateChanged_ = false;
    }

    HandleTemperature(temperature);
    HandleCapacity(capacity_);

    led_->UpdateColor(chargeState_, capacity_);

    if (backlight_->GetScreenState() == BatteryBacklight::SCREEN_ON) {
        UpdateAnimation(chargeState_, capacity_);
    }
}

void ChargerThread::HandleTemperature(const int32_t& temperature)
{
    const int32_t DEFAULT_UPPER_TEMP_CONF = INT32_MAX;
    const int32_t DEFAULT_LOWER_TEMP_CONF = INT32_MIN;
    auto& batteryConfig = BatteryConfig::GetInstance();
    auto highTemp = batteryConfig.GetInt("temperature.high", DEFAULT_UPPER_TEMP_CONF);
    auto lowTemp = batteryConfig.GetInt("temperature.low", DEFAULT_LOWER_TEMP_CONF);
    BATTERY_HILOGD(FEATURE_CHARGING, "temperature=%{public}d, lowTemp=%{public}d, highTemp=%{public}d", temperature,
        lowTemp, highTemp);

    if (((temperature <= lowTemp) || (temperature >= highTemp)) && (lowTemp != highTemp)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "temperature out of range, shutdown device");
        DoReboot(SHUTDOWN_CMD.c_str());
    }
}

void ChargerThread::HandleCapacity(const int32_t& capacity)
{
    if (capacity > lackPowerCapacity_ &&
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_DISABLE ||
            chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "Not Charging, Shutdown system");
        DoReboot(SHUTDOWN_CMD.c_str());
    }
}

void ChargerThread::UpdateAnimation(const int32_t& chargeState, const int32_t& capacity)
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start update animation, capacity=%{public}d", capacity);
    if ((chargeState == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Unknown charge state");
        return;
    }

    if (capacity <= lackPowerCapacity_) {
        if (chargeState == PowerSupplyProvider::CHARGE_STATE_ENABLE) { // Charging state
            BATTERY_HILOGD(FEATURE_CHARGING, "Lack power");
            animation_->AnimationStop();
            animation_->LackPowerNotChargingPromptStop();
            animation_->LackPowerChargingPromptStart();
        } else if (chargeState == PowerSupplyProvider::CHARGE_STATE_DISABLE) { // Not charging state
            BATTERY_HILOGD(FEATURE_CHARGING, "Lack power, please connect charger");
            animation_->AnimationStop();
            animation_->LackPowerChargingPromptStop();
            animation_->LackPowerNotChargingPromptStart();
        } else {
            BATTERY_HILOGD(FEATURE_CHARGING, "capacity=%{public}d, chargeState=%{public}d", capacity, chargeState);
        }
    } else if (chargeState == PowerSupplyProvider::CHARGE_STATE_ENABLE ||
        chargeState == PowerSupplyProvider::CHARGE_STATE_FULL) { // Charging state
        BATTERY_HILOGD(FEATURE_CHARGING, "Display animation according capacity");
        animation_->LackPowerChargingPromptStop();
        animation_->LackPowerNotChargingPromptStop();
        animation_->AnimationStart(capacity);
    }
}

void ChargerThread::InitAnimation()
{
    animation_ = std::make_unique<ChargerAnimation>();
    animation_->InitConfig();
}

void ChargerThread::SetKeyWait(struct KeyState& key, int64_t timeout)
{
    int64_t nextMoment = key.timestamp + timeout;
    if (keyWait_ == INVALID || nextMoment < keyWait_) {
        keyWait_ = nextMoment;
    }
}

void ChargerThread::HandleChargingState()
{
    if ((chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        return;
    }

    if (isChargeStateChanged_) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Charging State has changed");
        backlight_->TurnOnScreen();
        backlightWait_ = GetCurrentTime() - 1;
        UpdateAnimation(chargeState_, capacity_);
        isChargeStateChanged_ = false;
    }
}

void ChargerThread::HandleScreenState()
{
    if (backlightWait_ != INVALID && GetCurrentTime() > backlightWait_ + BACKLIGHT_OFF_TIME_MS) {
        backlight_->TurnOffScreen();
        animation_->AnimationStop();
        animation_->LackPowerChargingPromptStop();
        animation_->LackPowerNotChargingPromptStop();
        backlightWait_ = INVALID;
    }
}

void ChargerThread::HandlePowerKeyState()
{
    auto now = GetCurrentTime();
    HandlePowerKey(KEY_POWER, now);

    BATTERY_HILOGD(FEATURE_CHARGING, "keyWait_=%{public}" PRId64 "", keyWait_);
    if (keyWait_ != INVALID && now > keyWait_) {
        keyWait_ = INVALID;
    }
}

void ChargerThread::HandlePowerKey(int32_t keycode, int64_t now)
{
    if (keycode == KEY_POWER) {
        static bool turnOnByKeydown = false;
        if (g_keys[keycode].isDown) {
            int64_t rebootTime = g_keys[keycode].timestamp + REBOOT_TIME;
            if (now >= rebootTime) {
                BATTERY_HILOGW(FEATURE_CHARGING, "reboot machine");
                backlight_->TurnOffScreen();
                vibrate_->HandleVibration(VIBRATE_TIME_MS);
                ClearPowerOffChargeFlag();
                DoReboot(REBOOT_CMD.c_str());
            } else if (backlight_->GetScreenState() == BatteryBacklight::SCREEN_OFF) {
                SetKeyWait(g_keys[keycode], REBOOT_TIME);
                backlight_->TurnOnScreen();
                UpdateAnimation(chargeState_, capacity_);
                backlightWait_ = now - 1;
                turnOnByKeydown = true;
            }
        } else if (g_keys[keycode].isUp) {
            if (backlight_->GetScreenState() == BatteryBacklight::SCREEN_ON && !turnOnByKeydown) {
                backlight_->TurnOffScreen();
                animation_->AnimationStop();
                animation_->LackPowerChargingPromptStop();
                animation_->LackPowerNotChargingPromptStop();
                backlightWait_ = INVALID;
            } else {
                backlight_->TurnOnScreen();
                backlightWait_ = now - 1;
                UpdateAnimation(chargeState_, capacity_);
            }
            g_keys[keycode].isUp = false;
            turnOnByKeydown = false;
        }
    }
}

void ChargerThreadInputMonitor::OnInputEvent(std::shared_ptr<OHOS::MMI::PointerEvent> pointerEvent) const {};
void ChargerThreadInputMonitor::OnInputEvent(std::shared_ptr<OHOS::MMI::AxisEvent> axisEvent) const {};

void ChargerThreadInputMonitor::OnInputEvent(std::shared_ptr<OHOS::MMI::KeyEvent> keyEvent) const
{
    if (keyEvent->GetKeyCode() == OHOS::MMI::KeyEvent::KEYCODE_POWER) {
        if (keyEvent->GetKeyAction() == MMI::KeyEvent::KEY_ACTION_DOWN) {
            BATTERY_HILOGI(FEATURE_CHARGING, "PowerKey Action Down");
            SetKeyState(KEY_POWER, KEY_ACTION_DOWN_VAL, GetCurrentTime());
        } else if (keyEvent->GetKeyAction() == MMI::KeyEvent::KEY_ACTION_UP) {
            BATTERY_HILOGI(FEATURE_CHARGING, "PowerKey Action Up");
            SetKeyState(KEY_POWER, KEY_ACTION_UP_VAL, GetCurrentTime());
        }
    }
}

void ChargerThread::InputMonitorInit()
{
    BATTERY_HILOGI(FEATURE_CHARGING, "Charger input monitor init");
    std::shared_ptr<ChargerThreadInputMonitor> inputMonitor = std::make_shared<ChargerThreadInputMonitor>();
    if (inputMonitorId_ < 0) {
        inputMonitorId_ =
            InputManager::GetInstance()->AddMonitor(std::static_pointer_cast<IInputEventConsumer>(inputMonitor));
    }
}

void ChargerThread::InputMonitorCancel()
{
    BATTERY_HILOGI(FEATURE_CHARGING, "Charger input monitor cancel");
    InputManager *inputManager = InputManager::GetInstance();
    if (inputMonitorId_ >= 0) {
        inputManager->RemoveMonitor(inputMonitorId_);
        inputMonitorId_ = -1;
    }
}

void ChargerThread::InitLackPowerCapacity()
{
#ifdef HAS_BATTERY_CONFIG_POLICY_PART
    if (!isConfigParse_) {
        isConfigParse_ = BatteryConfig::GetInstance().ParseConfig();
    }
#endif
    auto& batteryConfig = BatteryConfig::GetInstance();
    lackPowerCapacity_ = batteryConfig.GetInt("soc.shutdown");
    BATTERY_HILOGD(FEATURE_CHARGING, "lackPowerCapacity_ = %{public}d", lackPowerCapacity_);
}

void ChargerThread::InitBatteryFileSystem()
{
    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique PowerSupplyProvider return nullptr");
        return;
    }
    provider_->InitBatteryPath();
    provider_->InitPowerSupplySysfs();
}

void ChargerThread::InitVibration()
{
    vibrate_ = std::make_unique<BatteryVibrate>();
    if (vibrate_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryVibrate return nullptr");
        return;
    }

    if (!vibrate_->InitVibration()) {
        BATTERY_HILOGW(FEATURE_CHARGING, "InitVibration failed, vibration does not work");
    }
}

void ChargerThread::InitBacklight()
{
    backlight_ = std::make_unique<BatteryBacklight>();
    if (backlight_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryBacklight return nullptr");
        return;
    }
    backlight_->TurnOnScreen();
}

void ChargerThread::InitLed()
{
    led_ = std::make_unique<BatteryLed>();
    if (led_ == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "make_unique BatteryLed return nullptr");
        return;
    }
#ifdef HAS_BATTERY_CONFIG_POLICY_PART
    if (!isConfigParse_) {
        isConfigParse_ = BatteryConfig::GetInstance().ParseConfig();
    }
#endif
    led_->InitLight();
    led_->TurnOff();
}

void ChargerThread::Init()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init charger thread");
    InitLackPowerCapacity();
    InitBatteryFileSystem();
    InitVibration();
    InitBacklight();
    InitLed();
    InitAnimation();
    InputMonitorInit();
}

void ChargerThread::Run(void* service)
{
    BATTERY_HILOGI(FEATURE_CHARGING, "start run charger thread");
    Init();
    std::make_unique<std::thread>([this, service] { this->LoopingThreadEntry(service); })->join();
}
} // namespace PowerMgr
} // namespace OHOS

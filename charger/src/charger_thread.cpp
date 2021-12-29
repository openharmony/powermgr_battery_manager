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

#include "charger_thread.h"

#include <parameters.h>
#include <securec.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "utils/hdf_log.h"
#include "updater_ui.h"
#include "text_label.h"
#include "view.h"
#include "input_manager.h"
#include "power_mgr_client.h"

#define HDF_LOG_TAG Charger

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
struct KeyState {
    bool up;
    bool down;
    int64_t timestamp;
};

constexpr int SHUTDOWN_TIME_MS = 2000;
constexpr long long MAX_INT64 = 9223372036854775807;
constexpr int SEC_TO_MSEC = 1000;
constexpr int NSEC_TO_MSEC = 1000000;
constexpr int REBOOT_TIME = 2000;
constexpr int BACKLIGHT_OFF_TIME_MS = 10000;
constexpr uint32_t INIT_DEFAULT_VALUE = 255;
constexpr int VIBRATE_TIME_MS = 75;
constexpr int MAX_IMGS = 62;
constexpr int MAX_IMGS_NAME_SIZE = 255;
constexpr int LOOP_TOP_PICTURES = 10;

Frame* g_hosFrame;
Frame* g_updateFrame;
AnimationLabel* g_animationLabel;
TextLabel* g_updateInfoLabel;
TextLabel* g_logLabel;
TextLabel* g_logResultLabel;
IInputInterface* g_inputInterface;
InputEventCb g_callback;
struct KeyState g_keys[KEY_MAX + 1] = {};

int64_t ChargerThread::keyWait_ = -1;
int64_t ChargerThread::backlightWait_ = -1;
int32_t ChargerThread::capacity_ = -1;

static int64_t GetCurrentTime()
{
    HDF_LOGI("%{public}s enter", __func__);
    timespec tm {};
    clock_gettime(CLOCK_MONOTONIC, &tm);

    HDF_LOGI("%{public}s exit", __func__);
    return tm.tv_sec * SEC_TO_MSEC + (tm.tv_nsec / NSEC_TO_MSEC);
}

void ChargerThread::HandleStates()
{
    HDF_LOGI("%{public}s enter", __func__);
    HandleChargingState();
    HandlePowerKeyState();
    HandleScreenState();

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

int ChargerThread::UpdateWaitInterval()
{
    HDF_LOGI("%{public}s enter", __func__);
    int64_t currentTime = GetCurrentTime();
    int64_t nextWait = MAX_INT64;
    int64_t timeout;

    if (pluginWait_ != -1) {
        nextWait = pluginWait_ - currentTime;
    }

    if (keyWait_ != -1 && keyWait_ < nextWait) {
        nextWait = keyWait_;
    }

    if (backlightWait_ != -1 && backlightWait_ < nextWait) {
        nextWait = backlightWait_;
    }

    if (nextWait != -1 && nextWait != MAX_INT64) {
        if (nextWait - currentTime > 0) {
            timeout = nextWait - currentTime;
        } else {
            timeout = 0;
        }
    } else {
        timeout = -1;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return timeout;
}

void ChargerThread::AnimationInit()
{
    HDF_LOGI("%{public}s enter", __func__);
    constexpr char alpha = 0xff;
    int screenH = 0;
    int screenW = 0;
    auto* sfDev = new SurfaceDev(SurfaceDev::DevType::DRM_DEVICE);
    sfDev->GetScreenSize(screenW, screenH);
    View::BRGA888Pixel bgColor {0x00, 0x00, 0x00, alpha};

    g_hosFrame = new Frame(screenW, screenH, View::PixelFormat::BGRA888, sfDev);
    g_hosFrame->SetBackgroundColor(&bgColor);

    g_animationLabel = new AnimationLabel(90, 240, 360, 960 >> 1, g_hosFrame);
    g_animationLabel->SetBackgroundColor(&bgColor);
    LoadImgs(g_animationLabel);

    g_updateInfoLabel = new TextLabel(screenW / 3, 340, screenW / 3, HEIGHT5, g_hosFrame);
    g_updateInfoLabel->SetOutLineBold(false, false);
    g_updateInfoLabel->SetBackgroundColor(&bgColor);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::LoadImgs(AnimationLabel* g_animationLabel)
{
    HDF_LOGD("%{public}s enter", __func__);
    char nameBuf[MAX_IMGS_NAME_SIZE];
    for (int i = 0; i < MAX_IMGS; i++) {
        if (memset_s(nameBuf, MAX_IMGS_NAME_SIZE + 1, 0, MAX_IMGS_NAME_SIZE) != EOK) {
            HDF_LOGD("%{public}s: Memset_s failed", __func__);
            return;
        }

        if (i < LOOP_TOP_PICTURES) {
            if (snprintf_s(nameBuf, MAX_IMGS_NAME_SIZE, MAX_IMGS_NAME_SIZE - 1,
                "/system/etc/resources/loop0000%d.png", i) == -1) {
                return;
            }
        } else {
            if (snprintf_s(nameBuf, MAX_IMGS_NAME_SIZE, MAX_IMGS_NAME_SIZE - 1,
                "/system/etc/resources/loop000%d.png", i) == -1) {
                return;
            }
        }

        g_animationLabel->AddImg(nameBuf);
    }
    g_animationLabel->AddStaticImg(nameBuf);
}

void ChargerThread::UpdateAnimation(const int32_t& capacity)
{
    HDF_LOGI("%{public}s enter", __func__);
    AnimationLabel::needStop_ = false;

    struct FocusInfo info {false, false};
    struct Bold bold {false, false};
    View::BRGA888Pixel bgColor {0x00, 0x00, 0x00, 0xff};
    std::string displaySoc = "  " + std::to_string(capacity) + "%";
    TextLabelInit(g_updateInfoLabel, displaySoc, bold, info, bgColor);
    g_animationLabel->UpdateLoop();

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::CycleMatters()
{
    HDF_LOGI("%{public}s enter", __func__);
    if (!started_) {
        started_ = true;
        backlightWait_ = GetCurrentTime() - 1;
    }

    provider_->ParseCapacity(&capacity_);
    provider_->ParseChargeState(&chargeState_);
    HDF_LOGI("%{public}s: chargeState_ = %{public}d, %{public}d", __func__, chargeState_, capacity_);

    UpdateEpollInterval(chargeState_);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::UpdateBatteryInfo(void* arg, char* msg)
{
    HDF_LOGI("%{public}s enter", __func__);
    std::unique_ptr<BatterydInfo> batteryInfo = std::make_unique<BatterydInfo>();
    if (batteryInfo == nullptr) {
        HDF_LOGE("%{public}s: instantiate batteryInfo error", __func__);
        return;
    }

    provider_->ParseUeventToBatterydInfo(msg, batteryInfo.get());

    capacity_ = batteryInfo->capacity_;
    chargeState_ = batteryInfo->chargeState_;

    HandleCapacity(capacity_);
    HandleTemperature(batteryInfo->temperature_);

    led_->UpdateLedColor(chargeState_, capacity_);
    if (backlight_->GetScreenState()) {
        UpdateAnimation(capacity_);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::UpdateBatteryInfo(void* arg)
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t temperature = 0;
    provider_->ParseTemperature(&temperature);
    provider_->ParseCapacity(&capacity_);
    provider_->ParseChargeState(&chargeState_);
    HDF_LOGD("%{public}s: temperature=%{public}d, capacity_=%{public}d, chargeState_=%{public}d", \
        __func__, temperature, capacity_, chargeState_);

    HandleTemperature(temperature);
    HandleCapacity(capacity_);

    led_->UpdateLedColor(chargeState_, capacity_);
    if (backlight_->GetScreenState()) {
        UpdateAnimation(capacity_);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandleCapacity(const int32_t& capacity)
{
    HDF_LOGI("%{public}s enter", __func__);
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if ((capacity <= batteryConfig_->GetCapacityConf()) &&
        ((chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_RESERVED))) {
        std::string reason = "LowCapacity";
        powerMgrClient.ShutDownDevice(reason);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandleTemperature(const int32_t& temperature)
{
    HDF_LOGI("%{public}s enter", __func__);
    auto tempConf = batteryConfig_->GetTempConf();
    HDF_LOGD("%{public}s: temperature=%{public}d, tempConf.lower=%{public}d, tempConf.upper=%{public}d",
        __func__, temperature, tempConf.lower, tempConf.upper);

    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if (((temperature <= tempConf.lower) || (temperature >= tempConf.upper)) &&
        (tempConf.lower != tempConf.upper)) {
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::SetKeyWait(struct KeyState& key, int64_t timeout)
{
    HDF_LOGI("%{public}s enter", __func__);
    int64_t nextMoment = key.timestamp + timeout;
    if (keyWait_ == -1 || nextMoment < keyWait_) {
        keyWait_ = nextMoment;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandleChargingState()
{
    HDF_LOGI("%{public}s enter", __func__);
    int64_t now = GetCurrentTime();
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();

    if ((chargeState_ == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargeState_ == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        if (pluginWait_ == -1) {
            backlightWait_ = now - 1;
            backlight_->TurnOnScreen();
            led_->TurnOffLed();
            AnimationLabel::needStop_ = true;
            pluginWait_ = now + SHUTDOWN_TIME_MS;
        } else if (now >= pluginWait_) {
            std::string reason = "charger unplugged";
            powerMgrClient.ShutDownDevice(reason);
        } else {
            HDF_LOGD("%{public}s: ShutDownDevice timer already in scheduled.", __func__);
        }
    } else {
        if (pluginWait_ != -1) {
            backlightWait_ = now - 1;
            backlight_->TurnOnScreen();
            led_->UpdateLedColor(chargeState_, capacity_);
            AnimationLabel::needStop_ = true;
            UpdateAnimation(capacity_);
        }
        pluginWait_ = -1;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandleScreenState()
{
    HDF_LOGI("%{public}s enter", __func__);
    if (backlightWait_ != -1 && GetCurrentTime() > backlightWait_ + BACKLIGHT_OFF_TIME_MS) {
        backlight_->TurnOffScreen();
        AnimationLabel::needStop_ = true;
        backlightWait_ = -1;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

int ChargerThread::SetKeyState(int code, int value, int64_t now)
{
    HDF_LOGI("%{public}s enter", __func__);
    bool down;
    if (!!value) {
        down = true;
    } else {
        down = false;
    }

    if (code > KEY_MAX) {
        return -1;
    }

    if (g_keys[code].down == down) {
        return 0;
    }

    if (down) {
        g_keys[code].timestamp = now;
    }

    g_keys[code].down = down;
    g_keys[code].up = true;

    HDF_LOGI("%{public}s exit", __func__);
    return 0;
}

void ChargerThread::HandlePowerKeyState()
{
    HDF_LOGD("%{public}s enter", __func__);
    auto now = GetCurrentTime();
    HandlePowerKey(KEY_POWER, now);

    if (keyWait_ != -1 && now > keyWait_) {
        keyWait_ = -1;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandlePowerKey(int keycode, int64_t now)
{
    HDF_LOGI("%{public}s enter", __func__);
    auto& powerMgrClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    KeyState key = g_keys[keycode];
    if (keycode == KEY_POWER) {
        if (key.down) {
            int64_t rebootTime = key.timestamp + REBOOT_TIME;
            if (now >= rebootTime) {
                HDF_LOGD("%{public}s: reboot machine.", __func__);
                backlight_->TurnOffScreen();
                AnimationLabel::needStop_ = true;
                vibrate_->HandleVibrate(VIBRATE_TIME_MS);
                std::string reason = "Reboot";
                powerMgrClient.RebootDevice(reason);
            } else {
                SetKeyWait(key, REBOOT_TIME);
                backlight_->TurnOnScreen();
                AnimationLabel::needStop_ = true;
                UpdateAnimation(capacity_);
                backlightWait_ = now - 1;
                HDF_LOGD("%{public}s: turn on the screen.", __func__);
            }
        } else {
            if (key.up) {
                backlight_->TurnOnScreen();
                AnimationLabel::needStop_ = true;
                UpdateAnimation(capacity_);
                backlightWait_ = now - 1;
            }
        }
    }
    key.up = false;

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::HandleInputEvent(const struct input_event* iev)
{
    HDF_LOGD("%{public}s enter", __func__);
    input_event ev {};
    ev.type = iev->type;
    ev.code = iev->code;
    ev.value = iev->value;
    HDF_LOGD("%{public}s: ev.type=%{public}d, ev.code=%{public}d, ev.value=%{public}d", \
        __func__, ev.type, ev.code, ev.value);

    if (ev.type != EV_KEY) {
        return;
    }
    SetKeyState(ev.code, ev.value, GetCurrentTime());

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void ChargerThread::EventPkgCallback(const EventPackage** pkgs, const uint32_t count, uint32_t devIndex)
{
    HDF_LOGD("%{public}s enter", __func__);
    if (pkgs == nullptr || *pkgs == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        struct input_event ev = {
            .type = static_cast<__u16>(pkgs[i]->type),
            .code = static_cast<__u16>(pkgs[i]->code),
            .value = pkgs[i]->value,
        };
        HandleInputEvent(&ev);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}


int ChargerThread::InputInit()
{
    HDF_LOGD("%{public}s enter", __func__);
    int ret = GetInputInterface(&g_inputInterface);
    if (ret != INPUT_SUCCESS) {
        HDF_LOGD("%{public}s: get input driver interface failed.", __func__);
        return ret;
    }

    ret = g_inputInterface->iInputManager->OpenInputDevice(1);
    if (ret) {
        HDF_LOGD("%{public}s: open device1 failed.", __func__);
        return ret;
    }

    uint32_t devType = 0;
    ret = g_inputInterface->iInputController->GetDeviceType(1, &devType);
    if (ret) {
        HDF_LOGD("%{public}s: get device1's type failed.", __func__);
        return ret;
    }

    g_callback.EventPkgCallback = EventPkgCallback;
    ret  = g_inputInterface->iInputReporter->RegisterReportCallback(1, &g_callback);
    if (ret) {
        HDF_LOGD("%{public}s: register callback failed for device 1.", __func__);
        return ret;
    }

    devType = INIT_DEFAULT_VALUE;
    ret = g_inputInterface->iInputController->GetDeviceType(1, &devType);

    HDF_LOGI("%{public}s exit", __func__);
    return 0;
}

void ChargerThread::Init()
{
    HDF_LOGI("%{public}s enter", __func__);
    batteryConfig_ = std::make_unique<BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        HDF_LOGD("%{public}s: init batteryconfig error.", __func__);
        return;
    }
    batteryConfig_->Init();

    provider_ = std::make_unique<PowerSupplyProvider>();
    if (provider_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate PowerSupplyProvider error.", __func__);
        return;
    }
    provider_->InitBatteryPath();
    provider_->InitPowerSupplySysfs();

    vibrate_ = std::make_unique<BatteryVibrate>();
    if (vibrate_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate BatteryVibrate error.", __func__);
        return;
    }

    if (vibrate_->VibrateInit() < 0) {
        HDF_LOGE("%{public}s: VibrateInit failed, vibration does not work.", __func__);
    }

    backlight_ = std::make_unique<BatteryBacklight>();
    if (backlight_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate BatteryBacklight error.", __func__);
        return;
    }
    backlight_->InitBacklightSysfs();
    backlight_->TurnOnScreen();

    led_ = std::make_unique<BatteryLed>();
    if (led_ == nullptr) {
        HDF_LOGE("%{public}s: instantiate BatteryLed error.", __func__);
        return;
    }
    led_->InitLedsSysfs();
    led_->TurnOffLed();

    AnimationInit();
    InputInit();
}

void ChargerThread::Run(void* service)
{
    HDF_LOGI("%{public}s enter", __func__);
    Init();

    std::make_unique<std::thread>(&ChargerThread::LoopingThreadEntry, this, service)->join();
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

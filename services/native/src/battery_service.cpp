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

#include "battery_service.h"
#include <unistd.h>
#include "battery_dump.h"
#include "file_ex.h"
#include "system_ability_definition.h"
#include "battery_log.h"
#include "power_mgr_client.h"
#include "v1_0/battery_interface_proxy.h"
#include "display_manager.h"
#include "ui_service_mgr_client.h"
#include "wm_common.h"

namespace OHOS {
namespace PowerMgr {
namespace {
const std::string BATTERY_SERVICE_NAME = "BatteryService";
const std::string BATTERY_LOW_CAPACITY_PARAMS = "{\"cancelButton\":\"LowCapacity\"}";
constexpr int32_t HELP_DMUP_PARAM = 2;
constexpr int32_t BATTERY_LOW_CAPACITY = 10;
constexpr int32_t UI_DIALOG_POWER_WIDTH_NARROW = 400;
constexpr int32_t UI_DIALOG_POWER_HEIGHT_NARROW = 240;
constexpr int32_t UI_DEFAULT_WIDTH = 2560;
constexpr int32_t UI_DEFAULT_HEIGHT = 1600;
constexpr int32_t UI_DEFAULT_BUTTOM_CLIP = 50 * 2;
constexpr int32_t UI_WIDTH_780DP = 780 * 2;
constexpr int32_t UI_HALF = 2;
constexpr int32_t BATTERY_FULL_CAPACITY = 100;
constexpr int32_t SEC_TO_MSEC = 1000;
constexpr int32_t NSEC_TO_MSEC = 1000000;
constexpr int32_t BATTERY_EMERGENCY_THRESHOLD = 5;
constexpr int32_t BATTERY_LOW_THRESHOLD = 20;
constexpr int32_t BATTERY_NORMAL_THRESHOLD = 90;
constexpr int32_t BATTERY_HIGH_THRESHOLD = 95;
sptr<BatteryService> g_service;
int32_t g_lastChargeState = 0;
bool g_initConfig = true;
bool g_lastLowCapacity = false;
}

const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    DelayedSpSingleton<BatteryService>::GetInstance().GetRefPtr());
sptr<IBatteryInterface> ibatteryInterface;

BatteryService::BatteryService()
    : SystemAbility(POWER_MANAGER_BATT_SERVICE_ID, true)
{
}

BatteryService::~BatteryService() {}

static int64_t GetCurrentTime()
{
    timespec tm {};
    clock_gettime(CLOCK_MONOTONIC, &tm);

    return tm.tv_sec * SEC_TO_MSEC + (tm.tv_nsec / NSEC_TO_MSEC);
}

void BatteryService::OnDump()
{
    BATTERY_HILOGD(COMP_SVC, "OnDump");
}

void BatteryService::OnStart()
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    if (ready_) {
        BATTERY_HILOGD(COMP_SVC, "Service is ready, nothing to do");
        return;
    }
    if (!(Init())) {
        BATTERY_HILOGE(COMP_SVC, "Call init failed");
        return;
    }
    if (!(InitBatteryd())) {
        BATTERY_HILOGE(COMP_SVC, "Call initBatteryd failed");
        return;
    }
    if (!Publish(this)) {
        BATTERY_HILOGE(COMP_SVC, "Register to system ability manager failed");
        return;
    }
    ready_ = true;
    BATTERY_HILOGI(COMP_SVC, "Success");
}

bool BatteryService::Init()
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    if (!eventRunner_) {
        eventRunner_ = AppExecFwk::EventRunner::Create(BATTERY_SERVICE_NAME);
        if (eventRunner_ == nullptr) {
            BATTERY_HILOGE(COMP_SVC, "Init failed due to create EventRunner");
            return false;
        }
    }
    if (!handler_) {
        handler_ = std::make_shared<BatteryServiceEventHandler>(eventRunner_,
            DelayedSpSingleton<BatteryService>::GetInstance());
        if (handler_ == nullptr) {
            BATTERY_HILOGE(COMP_SVC, "Init failed due to create handler error");
            return false;
        }
    }

    BATTERY_HILOGI(COMP_SVC, "Success");
    return true;
}

bool BatteryService::InitBatteryd()
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    sptr<OHOS::HDI::Battery::V1_0::IBatteryCallback> callback =  new OHOS::HDI::Battery::V1_0::BatteryCallbackImpl();
    ibatteryInterface = OHOS::HDI::Battery::V1_0::IBatteryInterface::Get();
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "ibatteryInterface is nullptr");
        return false;
    }
    ErrCode ret = ibatteryInterface->Register(callback);

    BatteryCallbackImpl::BatteryEventCallback eventCb =
        std::bind(&BatteryService::HandleBatteryCallbackEvent, this, std::placeholders::_1);
    BatteryCallbackImpl::RegisterBatteryEvent(eventCb);

    BATTERY_HILOGD(COMP_SVC, "InitBatteryd ret: %{public}d", ret);
    return SUCCEEDED(ret);
}

void BatteryService::InitConfig()
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    batteryConfig_ = std::make_unique<HDI::Battery::V1_0::BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        BATTERY_HILOGD(COMP_SVC, "instantiate batteryconfig error.");
        return;
    }
    batteryConfig_->Init();

    batteryLed_ = std::make_unique<HDI::Battery::V1_0::BatteryLed>();
    if (batteryLed_ == nullptr) {
        BATTERY_HILOGD(COMP_SVC, "instantiate BatteryLed error.");
        return;
    }
    batteryLed_->InitLedsSysfs();
    BATTERY_HILOGI(COMP_SVC, "Success");
}

int32_t BatteryService::HandleBatteryCallbackEvent(const CallbackInfo& event)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    BatteryInfo batteryInfo;
    BATTERY_HILOGD(COMP_SVC,
        "capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
        "technology=%{public}s", event.capacity, event.voltage,
        event.temperature, event.healthState, event.pluggedType,
        event.pluggedMaxCurrent, event.pluggedMaxVoltage, event.chargeState,
        event.chargeCounter, event.present, event.technology.c_str());

    batteryInfo.SetCapacity(event.capacity);
    batteryInfo.SetVoltage(event.voltage);
    batteryInfo.SetTemperature(event.temperature);
    batteryInfo.SetHealthState(BatteryHealthState(event.healthState));
    batteryInfo.SetPluggedType(BatteryPluggedType(event.pluggedType));
    batteryInfo.SetPluggedMaxCurrent(event.pluggedMaxCurrent);
    batteryInfo.SetPluggedMaxVoltage(event.pluggedMaxVoltage);
    batteryInfo.SetChargeState(BatteryChargeState(event.chargeState));
    batteryInfo.SetChargeCounter(event.chargeCounter);
    batteryInfo.SetPresent(event.present);
    batteryInfo.SetTechnology(event.technology);

    if (g_initConfig) {
        InitConfig();
        g_initConfig = false;
    }
    batteryLed_->UpdateLedColor(event.chargeState, event.capacity);
    WakeupDevice(event.chargeState);
    HandlePopupEvent(event.capacity);
    CalculateRemainingChargeTime(event.capacity);

    BatteryServiceSubscriber::Update(batteryInfo);
    HandleTemperature(event.temperature);
    HandleCapacity(event.capacity, event.chargeState);
    return ERR_OK;
}

void BatteryService::OnStop()
{
    BATTERY_HILOGW(COMP_SVC, "Enter");
    if (!ready_) {
        return;
    }
    eventRunner_.reset();
    handler_.reset();
    ready_ = false;

    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "ibatteryInterface is nullptr");
        return;
    }
    ibatteryInterface->UnRegister();
    BATTERY_HILOGW(COMP_SVC, "Success");
}

void BatteryService::WakeupDevice(const int32_t& chargestate)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    if ((g_lastChargeState == CHARGE_STATE_NONE || g_lastChargeState == CHARGE_STATE_RESERVED) &&
        (chargestate != CHARGE_STATE_NONE && chargestate !=CHARGE_STATE_RESERVED)) {
        auto& powerMgrClient = PowerMgrClient::GetInstance();
        powerMgrClient.WakeupDevice();
    }
    g_lastChargeState = chargestate;

    BATTERY_HILOGD(COMP_SVC, "Exit");
    return;
}

void BatteryService::HandlePopupEvent(const int32_t capacity)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    bool ret = false;
    if ((capacity < BATTERY_LOW_CAPACITY) && (g_lastLowCapacity == false)) {
        ret = ShowDialog(BATTERY_LOW_CAPACITY_PARAMS);
        if (!ret) {
            BATTERY_HILOGI(COMP_SVC, "failed to popup");
            return;
        }
        g_lastLowCapacity = true;
    }

    if (capacity >= BATTERY_LOW_CAPACITY) {
        g_lastLowCapacity = false;
    }
}

bool BatteryService::ShowDialog(const std::string &params)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    int pos_x;
    int pos_y;
    int width;
    int height;
    bool wideScreen;

    GetDisplayPosition(pos_x, pos_y, width, height, wideScreen);

    if (params.empty()) {
        return false;
    }

    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        "battery_dialog",
        params,
        OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        pos_x,
        pos_y,
        width,
        height,
        [this](int32_t id, const std::string& event, const std::string& params) {
            BATTERY_HILOGI(COMP_SVC, "Dialog callback: %{public}s, %{public}s", event.c_str(), params.c_str());
            if (event == "EVENT_CANCEL") {
                Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
            }
        });
    return true;
}

void BatteryService::GetDisplayPosition(
    int32_t& offsetX, int32_t& offsetY, int32_t& width, int32_t& height, bool& wideScreen)
{
    wideScreen = true;
    auto display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        BATTERY_HILOGI(COMP_SVC, "dialog GetDefaultDisplay fail, try again.");
        display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    }

    if (display != nullptr) {
        if (display->GetWidth() < UI_WIDTH_780DP) {
            BATTERY_HILOGI(COMP_SVC, "share dialog narrow.");
            wideScreen = false;
            width = UI_DIALOG_POWER_WIDTH_NARROW;
            height = UI_DIALOG_POWER_HEIGHT_NARROW;
        }
        offsetX = (display->GetWidth() - width) / UI_HALF;
        offsetY = display->GetHeight() - height - UI_DEFAULT_BUTTOM_CLIP;
    } else {
        BATTERY_HILOGI(COMP_SVC, "dialog get display fail, use default wide.");
        offsetX = (UI_DEFAULT_WIDTH - width) / UI_HALF;
        offsetY = UI_DEFAULT_HEIGHT - height - UI_DEFAULT_BUTTOM_CLIP;
    }
}

void BatteryService::HandleTemperature(const int32_t& temperature)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    auto tempConf = batteryConfig_->GetTempConf();
    BATTERY_HILOGI(COMP_SVC, "temperature=%{public}d, tempConf.lower=%{public}d, tempConf.upper=%{public}d",
        temperature, tempConf.lower, tempConf.upper);

    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if (((temperature <= tempConf.lower) || (temperature >= tempConf.upper)) && (tempConf.lower != tempConf.upper)) {
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }

    BATTERY_HILOGD(COMP_SVC, "Exit");
    return;
}

void BatteryService::HandleCapacity(const int32_t& capacity, const int32_t& chargeState)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if ((capacity <= batteryConfig_->GetCapacityConf()) &&
        ((chargeState == CHARGE_STATE_NONE) || (chargeState == CHARGE_STATE_RESERVED))) {
        std::string reason = "LowCapacity";
        powerMgrClient.ShutDownDevice(reason);
    }

    BATTERY_HILOGD(COMP_SVC, "Exit");
    return;
}

int32_t BatteryService::GetCapacity()
{
    int capacity;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetCapacity(capacity);
    return capacity;
}

void BatteryService::ChangePath(const std::string path)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return;
    }
    ibatteryInterface->ChangePath(path);
    return;
}

BatteryChargeState BatteryService::GetChargingStatus()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryChargeState chargeState = OHOS::HDI::Battery::V1_0::BatteryChargeState(0);

    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryChargeState(chargeState);
    }

    ibatteryInterface->GetChargeState(chargeState);
    return OHOS::PowerMgr::BatteryChargeState(chargeState);
}

BatteryHealthState BatteryService::GetHealthStatus()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryHealthState healthState = OHOS::HDI::Battery::V1_0::BatteryHealthState(0);

    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryHealthState(healthState);
    }

    ibatteryInterface->GetHealthState(healthState);
    return OHOS::PowerMgr::BatteryHealthState(healthState);
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryPluggedType pluggedType = OHOS::HDI::Battery::V1_0::BatteryPluggedType(0);

    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
    }

    ibatteryInterface->GetPluggedType(pluggedType);
    return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
}

int32_t BatteryService::GetVoltage()
{
    int voltage;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetVoltage(voltage);
    return voltage;
}

bool BatteryService::GetPresent()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    bool present = false;

    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return present;
    }

    ibatteryInterface->GetPresent(present);
    return present;
}

std::string BatteryService::GetTechnology()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return "";
    }

    std::string technology;
    ibatteryInterface->GetTechnology(technology);
    return technology;
}

int32_t BatteryService::GetBatteryTemperature()
{
    int temperature;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (ibatteryInterface == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetTemperature(temperature);
    return temperature;
}

void BatteryService::CalculateRemainingChargeTime(int32_t capacity)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (capacity > BATTERY_FULL_CAPACITY) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "capacity error");
        return;
    }

    int64_t onceTime = 0;
    if (((capacity - lastCapacity_) >= 1) && (lastCapacity_ != 0)) {
        onceTime = (GetCurrentTime() - lastTime_) / (capacity - lastCapacity_);
        remainTime_ = (BATTERY_FULL_CAPACITY - capacity) * onceTime;
    }

    lastCapacity_ = capacity;
    lastTime_ = GetCurrentTime();
}

int64_t BatteryService::GetRemainingChargeTime()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    return remainTime_;
}

int32_t BatteryService::GetBatteryLevel()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    int32_t batteryLevel;
    int32_t capacity = GetCapacity();
    if (capacity < BATTERY_EMERGENCY_THRESHOLD) {
        batteryLevel = static_cast<int32_t>(BatteryLevel::LEVEL_EMERGENCY);
    } else if (capacity <= BATTERY_LOW_THRESHOLD) {
        batteryLevel = static_cast<int32_t>(BatteryLevel::LEVEL_LOW);
    } else if (capacity <= BATTERY_NORMAL_THRESHOLD) {
        batteryLevel = static_cast<int32_t>(BatteryLevel::LEVEL_NORMAL);
    } else if (capacity <= BATTERY_HIGH_THRESHOLD) {
        batteryLevel = static_cast<int32_t>(BatteryLevel::LEVEL_HIGH);
    } else {
        batteryLevel = static_cast<int32_t>(BatteryLevel::LEVEL_NONE);
    }

    return batteryLevel;
}

int32_t BatteryService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    if ((args.empty()) || (args[0].size() != HELP_DMUP_PARAM)) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "param cannot be empty or the length is not 2");
        dprintf(fd, "cmd param number is not equal to 2\n");
        batteryDump.DumpHelp(fd);
        return ERR_NO_INIT;
    }

    bool helpRet = batteryDump.DumpBatteryHelp(fd, args);
    bool getBatteryInfo = batteryDump.GetBatteryInfo(fd, g_service, args);
    bool total = helpRet + getBatteryInfo;
    if (!total) {
        dprintf(fd, "cmd param is error\n");
        batteryDump.DumpHelp(fd);
        return ERR_NO_INIT;
    }

    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS

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

#include "battery_service.h"

#include <unistd.h>

#include "display_manager.h"
#include "file_ex.h"
#include "power_mgr_client.h"
#include "system_ability_definition.h"
#include "ui_service_mgr_client.h"
#include "wm_common.h"

#include "battery_dump.h"
#include "battery_log.h"
#include "power_common.h"

using namespace OHOS::HDI::Battery;

namespace OHOS {
namespace PowerMgr {
namespace {
const std::string BATTERY_SERVICE_NAME = "BatteryService";
const std::string HDI_SERVICE_NAME = "battery_interface_service";
const std::string BATTERY_LOW_CAPACITY_PARAMS = "{\"cancelButton\":\"LowCapacity\"}";
constexpr int32_t HELP_DMUP_PARAM = 2;
constexpr int32_t BATTERY_LOW_CAPACITY = 10;
constexpr int32_t UI_DIALOG_POWER_WIDTH_NARROW = 400;
constexpr int32_t UI_DIALOG_POWER_HEIGHT_NARROW = 240;
constexpr int32_t UI_DEFAULT_WIDTH = 2560;
constexpr int32_t UI_DEFAULT_HEIGHT = 1600;
constexpr int32_t UI_DEFAULT_BUTTOM_CLIP = 50 * 2;
constexpr int32_t UI_HALF = 2;
constexpr int32_t BATTERY_FULL_CAPACITY = 100;
constexpr int32_t SEC_TO_MSEC = 1000;
constexpr int32_t NSEC_TO_MSEC = 1000000;
constexpr int32_t BATTERY_EMERGENCY_THRESHOLD = 5;
constexpr int32_t BATTERY_LOW_THRESHOLD = 20;
constexpr int32_t BATTERY_NORMAL_THRESHOLD = 90;
constexpr int32_t BATTERY_HIGH_THRESHOLD = 95;
constexpr int32_t BATTERY_HIGH_FULL = 100;
constexpr uint32_t RETRY_TIME = 1000;
sptr<BatteryService> g_service;
int32_t g_lastChargeState = 0;
bool g_initConfig = true;
bool g_lastLowCapacity = false;
}

const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    DelayedSpSingleton<BatteryService>::GetInstance().GetRefPtr());

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
    RegisterHdiStatusListener();
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

void BatteryService::RegisterBatteryHdiCallback()
{
    BATTERY_HILOGD(COMP_SVC, "register battery hdi callback");
    if (iBatteryInterface_ == nullptr) {
        iBatteryInterface_ = IBatteryInterface::Get();
        RETURN_IF_WITH_LOG(iBatteryInterface_ == nullptr, "failed to get battery hdi interface");
    }
    sptr<V1_0::IBatteryCallback> callback = new V1_0::BatteryCallbackImpl();
    ErrCode ret = iBatteryInterface_->Register(callback);

    BatteryCallbackImpl::BatteryEventCallback eventCb =
        std::bind(&BatteryService::HandleBatteryCallbackEvent, this, std::placeholders::_1);
    BatteryCallbackImpl::RegisterBatteryEvent(eventCb);

    BATTERY_HILOGD(COMP_SVC, "register battery hdi callback end ret: %{public}d", ret);
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

int32_t BatteryService::HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_0::BatteryInfo& event)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    BatteryInfo batteryInfo;
    BATTERY_HILOGD(COMP_SVC,
        "capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, " \
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
        "technology=%{public}s, currnow=%{public}d", event.capacity, event.voltage,
        event.temperature, event.healthState, event.pluggedType,
        event.pluggedMaxCurrent, event.pluggedMaxVoltage, event.chargeState,
        event.chargeCounter, event.present, event.technology.c_str(), event.curNow);

    BATTERY_HILOGD(COMP_SVC,
        "totalEnergy=%{public}d,curAverage=%{public}d,remainEngery=%{public}d", \
        event.totalEnergy, event.curAverage, event.remainEnergy);

    batteryInfo.SetCapacity(event.capacity);
    batteryInfo.SetVoltage(event.voltage);
    batteryInfo.SetTemperature(event.temperature);
    batteryInfo.SetHealthState(BatteryHealthState(event.healthState));
    batteryInfo.SetPluggedType(BatteryPluggedType(event.pluggedType));
    batteryInfo.SetPluggedMaxCurrent(event.pluggedMaxCurrent);
    batteryInfo.SetPluggedMaxVoltage(event.pluggedMaxVoltage);
    batteryInfo.SetChargeState(BatteryChargeState(event.chargeState));
    batteryInfo.SetChargeCounter(event.chargeCounter);
    batteryInfo.SetTotalEnergy(event.totalEnergy);
    batteryInfo.SetCurAverage(event.curAverage);
    batteryInfo.SetRemainEnergy(event.remainEnergy);
    batteryInfo.SetPresent(event.present);
    batteryInfo.SetTechnology(event.technology);
    batteryInfo.SetNowCurrent(event.curNow);

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

void BatteryService::RegisterHdiStatusListener()
{
    BATTERY_HILOGD(COMP_SVC, "battery rigister Hdi status listener");
    hdiServiceMgr_ = OHOS::HDI::ServiceManager::V1_0::IServiceManager::Get();
    if (hdiServiceMgr_ == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "hdi service manager is nullptr, Try again after %{public}u second", RETRY_TIME);
        SendEvent(BatteryServiceEventHandler::EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER, RETRY_TIME);
        return;
    }

    hdiServStatListener_ = new HdiServiceStatusListener(HdiServiceStatusListener::StatusCallback(
        [&](const OHOS::HDI::ServiceManager::V1_0::ServiceStatus &status) {
            RETURN_IF(status.serviceName != HDI_SERVICE_NAME || status.deviceClass != DEVICE_CLASS_DEFAULT);

            if (status.status == SERVIE_STATUS_START) {
                SendEvent(BatteryServiceEventHandler::EVENT_REGISTER_BATTERY_HDI_CALLBACK, 0);        
                BATTERY_HILOGD(COMP_SVC, "battery interface service start");
            } else if (status.status == SERVIE_STATUS_STOP && iBatteryInterface_) {
                iBatteryInterface_->UnRegister();
                iBatteryInterface_ = nullptr;
                BATTERY_HILOGW(COMP_SVC, "battery interface service stop, unregister interface");
            }
        }
    ));

    int32_t status = hdiServiceMgr_->RegisterServiceStatusListener(hdiServStatListener_, DEVICE_CLASS_DEFAULT);
    if (status != ERR_OK) {
        BATTERY_HILOGW(COMP_SVC, "Register hdi failed, Try again after %{public}u second", RETRY_TIME);
        SendEvent(BatteryServiceEventHandler::EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER, RETRY_TIME);
    }
}

void BatteryService::SendEvent(int32_t event, int64_t delayTime)
{
    RETURN_IF_WITH_LOG(handler_ == nullptr, "handler is nullptr");
    handler_->RemoveEvent(event);
    handler_->SendEvent(event, 0, delayTime);
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

    if (iBatteryInterface_ != nullptr) {
        iBatteryInterface_->UnRegister();
        iBatteryInterface_ = nullptr;    
    }
    if (hdiServiceMgr_ != nullptr) {
        hdiServiceMgr_->UnregisterServiceStatusListener(hdiServStatListener_);
        hdiServiceMgr_ = nullptr;
    }
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
        BATTERY_HILOGI(COMP_SVC, "display size: %{public}d x %{public}d",
            display->GetWidth(), display->GetHeight());
        if (display->GetWidth() < display->GetHeight()) {
            BATTERY_HILOGI(COMP_SVC, "share dialog narrow.");
            const int NARROW_WIDTH_N = 3;
            const int NARROW_WIDTH_D = 4;
            const int NARROW_HEIGHT_RATE = 8;
            wideScreen = false;
            width = display->GetWidth() * NARROW_WIDTH_N / NARROW_WIDTH_D;
            height = display->GetHeight() / NARROW_HEIGHT_RATE;
        } else {
            BATTERY_HILOGI(COMP_SVC, "share dialog wide.");
            const int NARROW_WIDTH_N = 1;
            const int NARROW_WIDTH_D = 3;
            const int NARROW_HEIGHT_RATE = 6;
            wideScreen = true;
            width = display->GetWidth() * NARROW_WIDTH_N / NARROW_WIDTH_D;
            height = display->GetHeight() / NARROW_HEIGHT_RATE;
        }
        offsetX = (display->GetWidth() - width) / UI_HALF;
        offsetY = display->GetHeight() - height - UI_DEFAULT_BUTTOM_CLIP;
    } else {
        BATTERY_HILOGI(COMP_SVC, "dialog get display fail, use default wide.");
        wideScreen = false;
        width = UI_DIALOG_POWER_WIDTH_NARROW;
        height = UI_DIALOG_POWER_HEIGHT_NARROW;
        offsetX = (UI_DEFAULT_WIDTH - width) / UI_HALF;
        offsetY = UI_DEFAULT_HEIGHT - height - UI_DEFAULT_BUTTOM_CLIP;
    }
    BATTERY_HILOGI(COMP_SVC, "GetDisplayPosition: %{public}d, %{public}d (%{public}d x %{public}d)",
        offsetX, offsetY, width, height);
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
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetCapacity(capacity);
    return capacity;
}

void BatteryService::ChangePath(const std::string path)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }
    iBatteryInterface_->ChangePath(path);
    return;
}

BatteryChargeState BatteryService::GetChargingStatus()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryChargeState chargeState = OHOS::HDI::Battery::V1_0::BatteryChargeState(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return OHOS::PowerMgr::BatteryChargeState(chargeState);
    }

    iBatteryInterface_->GetChargeState(chargeState);
    return OHOS::PowerMgr::BatteryChargeState(chargeState);
}

BatteryHealthState BatteryService::GetHealthStatus()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryHealthState healthState = OHOS::HDI::Battery::V1_0::BatteryHealthState(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return OHOS::PowerMgr::BatteryHealthState(healthState);
    }

    iBatteryInterface_->GetHealthState(healthState);
    return OHOS::PowerMgr::BatteryHealthState(healthState);
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    OHOS::HDI::Battery::V1_0::BatteryPluggedType pluggedType = OHOS::HDI::Battery::V1_0::BatteryPluggedType(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
    }

    iBatteryInterface_->GetPluggedType(pluggedType);
    return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
}

int32_t BatteryService::GetVoltage()
{
    int voltage;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }

    iBatteryInterface_->GetVoltage(voltage);
    return voltage;
}

bool BatteryService::GetPresent()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    bool present = false;

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return present;
    }

    iBatteryInterface_->GetPresent(present);
    return present;
}

std::string BatteryService::GetTechnology()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return "";
    }

    std::string technology;
    iBatteryInterface_->GetTechnology(technology);
    return technology;
}

int32_t BatteryService::GetBatteryTemperature()
{
    int temperature;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetTemperature(temperature);
    return temperature;
}

int32_t BatteryService::GetTotalEnergy()
{
    int totalEnergy;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetTotalEnergy(totalEnergy);
    return totalEnergy;
}

int32_t BatteryService::GetCurrentAverage()
{
    int curAverage;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetCurrentAverage(curAverage);
    return curAverage;
}

int32_t BatteryService::GetNowCurrent()
{
    int nowCurr;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetCurrentNow(nowCurr);
    return nowCurr;
}

int32_t BatteryService::GetRemainEnergy()
{
    int remainEnergy;
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetRemainEnergy(remainEnergy);
    return remainEnergy;
}

void BatteryService::CalculateRemainingChargeTime(int32_t capacity)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (capacity > BATTERY_FULL_CAPACITY) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "capacity error");
        return;
    }

    BatteryChargeState chargeStatus = GetChargingStatus();
    if (chargeStatus != BatteryChargeState::CHARGE_STATE_ENABLE) {
        remainTime_ = 0;
        chargeFlag_ = false;
        return;
    }

    if (chargeStatus == BatteryChargeState::CHARGE_STATE_ENABLE
        && !chargeFlag_) {
        lastCapacity_ = capacity;
        lastTime_ = GetCurrentTime();
        chargeFlag_ = true;
    }

    if (capacity < lastCapacity_) {
        lastCapacity_ = capacity;
    }

    int64_t onceTime = 0;
    if (((capacity - lastCapacity_) >= 1) && (lastCapacity_ >= 0) && chargeFlag_) {
        onceTime = (GetCurrentTime() - lastTime_) / (capacity - lastCapacity_);
        remainTime_ = (BATTERY_FULL_CAPACITY - capacity) * onceTime;
        lastCapacity_ = capacity;
        lastTime_ = GetCurrentTime();
    }
}

int64_t BatteryService::GetRemainingChargeTime()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    return remainTime_;
}

BatteryLevel BatteryService::GetBatteryLevel()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    BatteryLevel batteryLevel;
    int32_t capacity = GetCapacity();
    if (capacity < BATTERY_EMERGENCY_THRESHOLD) {
        batteryLevel = BatteryLevel::LEVEL_CRITICAL;
    } else if (capacity <= BATTERY_LOW_THRESHOLD) {
        batteryLevel = BatteryLevel::LEVEL_LOW;
    } else if (capacity <= BATTERY_NORMAL_THRESHOLD) {
        batteryLevel = BatteryLevel::LEVEL_NORMAL;
    } else if (capacity <= BATTERY_HIGH_THRESHOLD) {
        batteryLevel = BatteryLevel::LEVEL_HIGH;
    } else if (capacity == BATTERY_HIGH_FULL) {
        batteryLevel = BatteryLevel::LEVEL_FULL;
    } else {
        batteryLevel = BatteryLevel::LEVEL_NONE;
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

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
#include "battery_callback.h"
#include "battery_dump.h"
#include "battery_log.h"
#include "permission.h"
#include "power_common.h"

using namespace OHOS::HDI::Battery;
using namespace OHOS::HDI::Battery::V1_1;

namespace OHOS {
namespace PowerMgr {
namespace {
const std::string BATTERY_SERVICE_NAME = "BatteryService";
const std::string HDI_SERVICE_NAME = "battery_interface_service";
const std::string BATTERY_LOW_CAPACITY_PARAMS = "{\"lowPower\":\"Low Power\", \"cancelButton\":\"Cancel\"}";
constexpr int32_t HELP_DUMP_PARAM = 2;
constexpr int32_t BATTERY_LOW_CAPACITY = 10;
constexpr int32_t UI_DIALOG_POWER_WIDTH_NARROW = 400;
constexpr int32_t UI_DIALOG_POWER_HEIGHT_NARROW = 240;
constexpr int32_t BATTERY_FULL_CAPACITY = 100;
constexpr int32_t SEC_TO_MSEC = 1000;
constexpr int32_t NSEC_TO_MSEC = 1000000;
constexpr int32_t BATTERY_EMERGENCY_THRESHOLD = 5;
constexpr int32_t BATTERY_LOW_THRESHOLD = 20;
constexpr int32_t BATTERY_NORMAL_THRESHOLD = 90;
constexpr int32_t BATTERY_HIGH_THRESHOLD = 99;
constexpr int32_t BATTERY_HIGH_FULL = 100;
constexpr uint32_t RETRY_TIME = 1000;
sptr<BatteryService> g_service;
BatteryChargeState g_lastChargeState = BatteryChargeState::CHARGE_STATE_NONE;
bool g_initConfig = false;
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
    sptr<IBatteryCallback> callback = new BatteryCallback();
    ErrCode ret = iBatteryInterface_->Register(callback);

    BatteryCallback::BatteryEventCallback eventCb =
        std::bind(&BatteryService::HandleBatteryCallbackEvent, this, std::placeholders::_1);
    BatteryCallback::RegisterBatteryEvent(eventCb);

    BATTERY_HILOGD(COMP_SVC, "register battery hdi callback end ret: %{public}d", ret);
}

void BatteryService::InitConfig()
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    batteryConfig_ = std::make_unique<HDI::Battery::V1_1::BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        BATTERY_HILOGD(COMP_SVC, "instantiate batteryconfig error.");
        return;
    }
    batteryConfig_->Init();

    batteryLed_ = std::make_unique<HDI::Battery::V1_1::BatteryLed>();
    if (batteryLed_ == nullptr) {
        BATTERY_HILOGD(COMP_SVC, "instantiate BatteryLed error.");
        return;
    }
    batteryLed_->InitLedsSysfs();
    BATTERY_HILOGI(COMP_SVC, "Success");
}

int32_t BatteryService::HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_1::BatteryInfo& event)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    if (isMockUnplugged_) {
        return ERR_OK;
    }

    if (!g_initConfig) {
        InitConfig();
        g_initConfig = true;
    }

    UpdateBatteryInfo(event);
    HandleBatteryInfo();
    return ERR_OK;
}

void BatteryService::UpdateBatteryInfo(const OHOS::HDI::Battery::V1_1::BatteryInfo& event)
{
    batteryInfo_.SetCapacity(event.capacity);
    batteryInfo_.SetVoltage(event.voltage);
    batteryInfo_.SetTemperature(event.temperature);
    batteryInfo_.SetHealthState(BatteryHealthState(event.healthState));
    batteryInfo_.SetPluggedType(BatteryPluggedType(event.pluggedType));
    batteryInfo_.SetPluggedMaxCurrent(event.pluggedMaxCurrent);
    batteryInfo_.SetPluggedMaxVoltage(event.pluggedMaxVoltage);
    batteryInfo_.SetChargeState(BatteryChargeState(event.chargeState));
    batteryInfo_.SetChargeCounter(event.chargeCounter);
    batteryInfo_.SetTotalEnergy(event.totalEnergy);
    batteryInfo_.SetCurAverage(event.curAverage);
    batteryInfo_.SetRemainEnergy(event.remainEnergy);
    batteryInfo_.SetPresent(event.present);
    batteryInfo_.SetTechnology(event.technology);
    batteryInfo_.SetNowCurrent(event.curNow);
}

void BatteryService::HandleBatteryInfo()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, "
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, "
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, "
        "technology=%{public}s, currNow=%{public}d, totalEnergy=%{public}d, curAverage=%{public}d, "
        "remainEnergy=%{public}d", batteryInfo_.GetCapacity(), batteryInfo_.GetVoltage(), batteryInfo_.GetTemperature(),
        batteryInfo_.GetHealthState(), batteryInfo_.GetPluggedType(), batteryInfo_.GetPluggedMaxCurrent(),
        batteryInfo_.GetPluggedMaxVoltage(), batteryInfo_.GetChargeState(), batteryInfo_.GetChargeCounter(),
        batteryInfo_.IsPresent(), batteryInfo_.GetTechnology().c_str(), batteryInfo_.GetNowCurrent(),
        batteryInfo_.GetTotalEnergy(), batteryInfo_.GetCurAverage(), batteryInfo_.GetRemainEnergy());

    batteryLed_->UpdateLedColor(static_cast<int32_t>(batteryInfo_.GetChargeState()), batteryInfo_.GetCapacity());
    WakeupDevice(batteryInfo_.GetChargeState());
    HandlePopupEvent(batteryInfo_.GetCapacity());
    CalculateRemainingChargeTime(batteryInfo_.GetCapacity(), batteryInfo_.GetChargeState());

    BatteryServiceSubscriber::Update(batteryInfo_);
    HandleTemperature(batteryInfo_.GetTemperature());
    HandleCapacity(batteryInfo_.GetCapacity(), batteryInfo_.GetChargeState());
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

void BatteryService::WakeupDevice(BatteryChargeState chargeState)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    if ((g_lastChargeState == BatteryChargeState::CHARGE_STATE_NONE ||
         g_lastChargeState == BatteryChargeState::CHARGE_STATE_BUTT) &&
        (chargeState != BatteryChargeState::CHARGE_STATE_NONE &&
         chargeState != BatteryChargeState::CHARGE_STATE_BUTT)) {
        PowerMgrClient::GetInstance().WakeupDevice();
    }
    g_lastChargeState = chargeState;

    BATTERY_HILOGD(COMP_SVC, "Exit");
}

void BatteryService::HandlePopupEvent(int32_t capacity)
{
    bool ret = false;
    if ((capacity < BATTERY_LOW_CAPACITY) && !isLowPower_) {
        ret = ShowDialog(BATTERY_LOW_CAPACITY_PARAMS);
        if (!ret) {
            BATTERY_HILOGI(COMP_SVC, "failed to popup");
            return;
        }
        isLowPower_ = true;
    }

    if (capacity >= BATTERY_LOW_CAPACITY && isLowPower_) {
        Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
        dialogId_ = -1;
        isLowPower_ = false;
    }
}

bool BatteryService::ShowDialog(const std::string &params)
{
    int width;
    int height;

    GetDisplayPosition(width, height);

    if (params.empty()) {
        return false;
    }

    int32_t errCode = Ace::UIServiceMgrClient::GetInstance()->ShowDialog(
        "battery_dialog",
        params,
        OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        0,
        0,
        width,
        height,
        [this](int32_t id, const std::string& event, const std::string& params) {
            BATTERY_HILOGI(COMP_SVC, "Dialog callback: %{public}s, %{public}s", event.c_str(), params.c_str());
            if (event == "EVENT_CANCEL") {
                Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
                dialogId_ = -1;
            }
        }, &dialogId_);
    BATTERY_HILOGI(COMP_SVC, "Show dialog errCode %{public}d, dialogId=%{public}d", errCode, dialogId_);
    return !errCode;
}

void BatteryService::GetDisplayPosition(int32_t& width, int32_t& height)
{
    auto display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        BATTERY_HILOGI(COMP_SVC, "dialog GetDefaultDisplay fail, try again.");
        display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    }

    if (display != nullptr) {
        BATTERY_HILOGI(COMP_SVC, "display size: %{public}d x %{public}d",
            display->GetWidth(), display->GetHeight());
        width = display->GetWidth();
        height = display->GetHeight();
    } else {
        BATTERY_HILOGI(COMP_SVC, "dialog get display fail, use default wide.");
        width = UI_DIALOG_POWER_WIDTH_NARROW;
        height = UI_DIALOG_POWER_HEIGHT_NARROW;
    }
}

void BatteryService::HandleTemperature(int32_t temperature)
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
}

void BatteryService::HandleCapacity(int32_t capacity, BatteryChargeState chargeState)
{
    BATTERY_HILOGD(COMP_SVC, "Enter");
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if ((capacity <= batteryConfig_->GetCapacityConf()) &&
        ((chargeState == BatteryChargeState::CHARGE_STATE_NONE) ||
         (chargeState == BatteryChargeState::CHARGE_STATE_BUTT))) {
        std::string reason = "LowCapacity";
        powerMgrClient.ShutDownDevice(reason);
    }

    BATTERY_HILOGD(COMP_SVC, "Exit");
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
    OHOS::HDI::Battery::V1_1::BatteryChargeState chargeState = OHOS::HDI::Battery::V1_1::BatteryChargeState(0);

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
    OHOS::HDI::Battery::V1_1::BatteryHealthState healthState = OHOS::HDI::Battery::V1_1::BatteryHealthState(0);

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
    OHOS::HDI::Battery::V1_1::BatteryPluggedType pluggedType = OHOS::HDI::Battery::V1_1::BatteryPluggedType(0);

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
    int32_t totalEnergy = -1;
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }

    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetTotalEnergy totalEnergy: %{public}d", totalEnergy);
        return totalEnergy;
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
    int32_t nowCurr = -1;
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }

    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetNowCurrent nowCurr: %{public}d", nowCurr);
        return nowCurr;
    }
    iBatteryInterface_->GetCurrentNow(nowCurr);
    return nowCurr;
}

int32_t BatteryService::GetRemainEnergy()
{
    int32_t remainEnergy = -1;
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }

    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetRemainEnergy remainEnergy: %{public}d", remainEnergy);
        return remainEnergy;
    }
    iBatteryInterface_->GetRemainEnergy(remainEnergy);
    return remainEnergy;
}

void BatteryService::CalculateRemainingChargeTime(int32_t capacity, BatteryChargeState chargeState)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    if (capacity > BATTERY_FULL_CAPACITY) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "capacity error");
        return;
    }

    if (chargeState != BatteryChargeState::CHARGE_STATE_ENABLE) {
        remainTime_ = 0;
        chargeFlag_ = false;
        return;
    }

    if (!chargeFlag_) {
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
    if ((args.empty()) || (args[0].size() != HELP_DUMP_PARAM)) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "param cannot be empty or the length is not 2");
        dprintf(fd, "cmd param number is not equal to 2\n");
        batteryDump.DumpHelp(fd);
        return ERR_NO_INIT;
    }

    bool helpRet = batteryDump.DumpBatteryHelp(fd, args);
    bool getBatteryInfo = batteryDump.GetBatteryInfo(fd, g_service, args);
    bool unplugged = batteryDump.MockUnplugged(fd, g_service, args);
    bool reset = batteryDump.ResetPlugged(fd, g_service, args);
    bool total = helpRet + getBatteryInfo + unplugged + reset;
    if (!total) {
        dprintf(fd, "cmd param is error\n");
        batteryDump.DumpHelp(fd);
        return ERR_NO_INIT;
    }

    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    return ERR_OK;
}

void BatteryService::MockUnplugged(bool isUnplugged)
{
    if (isUnplugged) {
        batteryInfo_.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
        batteryInfo_.SetPluggedMaxCurrent(0);
        batteryInfo_.SetPluggedMaxVoltage(0);
        batteryInfo_.SetChargeState(BatteryChargeState::CHARGE_STATE_NONE);
        HandleBatteryInfo();
        isMockUnplugged_ = true;
    } else {
        isMockUnplugged_ = false;
        OHOS::HDI::Battery::V1_1::BatteryInfo event;
        iBatteryInterface_->GetBatteryInfo(event);
        HandleBatteryCallbackEvent(event);
    }
}
} // namespace PowerMgr
} // namespace OHOS

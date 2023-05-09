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

#include <ctime>
#include <cstdio>
#include <functional>
#include <ipc_skeleton.h>
#include "ability_manager_client.h"
#include "errors.h"
#include "new"
#include "permission.h"
#include "system_ability_definition.h"
#include "iremote_object.h"
#include "v1_1/ibattery_callback.h"
#include "hdf_io_service_if.h"
#include "hdf_service_status.h"
#include "battery_callback.h"
#include "battery_config.h"
#include "battery_dump.h"
#include "battery_log.h"
#include "power_mgr_client.h"
#include "power_common.h"
#include "watchdog.h"

using namespace OHOS::HDI::Battery;
using namespace OHOS::AAFwk;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* BATTERY_SERVICE_NAME = "BatteryService";
constexpr const char* BATTERY_HDI_NAME = "battery_interface_service";
constexpr int32_t HELP_DUMP_PARAM = 2;
constexpr int32_t BATTERY_FULL_CAPACITY = 100;
constexpr uint32_t RETRY_TIME = 1000;
sptr<BatteryService> g_service;
BatteryPluggedType g_lastPluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
static PowerMgrClient& g_powerMgrClient = PowerMgrClient::GetInstance();
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
    constexpr int32_t SEC_TO_MSEC = 1000;
    constexpr int32_t NSEC_TO_MSEC = 1000000;
    timespec tm {};
    clock_gettime(CLOCK_MONOTONIC, &tm);

    return tm.tv_sec * SEC_TO_MSEC + (tm.tv_nsec / NSEC_TO_MSEC);
}

void BatteryService::OnStart()
{
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
    AddSystemAbilityListener(MISCDEVICE_SERVICE_ABILITY_ID);
    ready_ = true;
}

bool BatteryService::Init()
{
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
        HiviewDFX::Watchdog::GetInstance().AddThread("BatteryServiceEventHandler", handler_);
    }

    InitConfig();
    if (!batteryNotify_) {
        batteryNotify_ = std::make_unique<BatteryNotify>();
    }
    return true;
}

void BatteryService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    BATTERY_HILOGI(COMP_SVC, "systemAbilityId=%{public}d, deviceId=%{private}s", systemAbilityId, deviceId.c_str());
    if (systemAbilityId == MISCDEVICE_SERVICE_ABILITY_ID) {
        batteryLight_.InitLight();
    }
}

bool BatteryService::RegisterBatteryHdiCallback()
{
    if (iBatteryInterface_ == nullptr) {
        iBatteryInterface_ = V1_1::IBatteryInterface::Get();
        BATTERY_HILOGE(COMP_SVC, "failed to get battery hdi interface");
        RETURN_IF_WITH_RET(iBatteryInterface_ == nullptr, false);
    }
    sptr<V1_1::IBatteryCallback> callback = new BatteryCallback();
    ErrCode ret = iBatteryInterface_->Register(callback);
    if (ret < 0) {
        BATTERY_HILOGE(COMP_SVC, "register callback failed");
        return false;
    }

    BatteryCallback::BatteryEventCallback eventCb =
        std::bind(&BatteryService::HandleBatteryCallbackEvent, this, std::placeholders::_1);
    BatteryCallback::RegisterBatteryEvent(eventCb);
    return true;
}

void BatteryService::InitConfig()
{
    auto& batteryConfig = BatteryConfig::GetInstance();
    batteryConfig.ParseConfig();

    warnCapacity_ = batteryConfig.GetInt("soc.warning", warnCapacity_);
    highTemperature_ = batteryConfig.GetInt("temperature.high", highTemperature_);
    lowTemperature_ = batteryConfig.GetInt("temperature.low", lowTemperature_);
    shutdownCapacityThreshold_ = batteryConfig.GetInt("soc.shutdown", shutdownCapacityThreshold_);
    criticalCapacityThreshold_ = batteryConfig.GetInt("soc.critical", criticalCapacityThreshold_);
    warningCapacityThreshold_ = batteryConfig.GetInt("soc.warning", warningCapacityThreshold_);
    lowCapacityThreshold_ = batteryConfig.GetInt("soc.low", lowCapacityThreshold_);
    normalCapacityThreshold_ = batteryConfig.GetInt("soc.normal", normalCapacityThreshold_);
    highCapacityThreshold_ = batteryConfig.GetInt("soc.high", highCapacityThreshold_);
    fullCapacityThreshold_ = batteryConfig.GetInt("soc.full", fullCapacityThreshold_);
    BATTERY_HILOGI(COMP_SVC, "warnCapacity_=%{public}d, highTemperature_=%{public}d,\
        lowTemperature_=%{public}d, shutdownCapacityThreshold_=%{public}d,\
        criticalCapacityThreshold_=%{public}d, warningCapacityThreshold_=%{public}d, lowCapacityThreshold_=%{public}d,\
        normalCapacityThreshold_=%{public}d, highCapacityThreshold_=%{public}d, fullCapacityThreshold_=%{public}d",
        warnCapacity_, highTemperature_, lowTemperature_, shutdownCapacityThreshold_, criticalCapacityThreshold_,
        warningCapacityThreshold_, lowCapacityThreshold_, normalCapacityThreshold_, highCapacityThreshold_,
        fullCapacityThreshold_);
}

int32_t BatteryService::HandleBatteryCallbackEvent(const V1_1::BatteryInfo& event)
{
    if (isMockUnplugged_) {
        return ERR_OK;
    }

    ConvertingEvent(event);
    RETURN_IF_WITH_RET(lastBatteryInfo_ == batteryInfo_, ERR_OK);
    HandleBatteryInfo();
    return ERR_OK;
}

void BatteryService::ConvertingEvent(const V1_1::BatteryInfo& event)
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

    batteryLight_.UpdateColor(batteryInfo_.GetChargeState(), batteryInfo_.GetCapacity());
    WakeupDevice(batteryInfo_.GetPluggedType());
    HandlePopupEvent(batteryInfo_.GetCapacity());
    CalculateRemainingChargeTime(batteryInfo_.GetCapacity(), batteryInfo_.GetChargeState());

    batteryNotify_->PublishEvents(batteryInfo_);
    HandleTemperature(batteryInfo_.GetTemperature());
    HandleCapacity(batteryInfo_.GetCapacity(), batteryInfo_.GetChargeState());
    lastBatteryInfo_ = batteryInfo_;
}

bool BatteryService::RegisterHdiStatusListener()
{
    hdiServiceMgr_ = OHOS::HDI::ServiceManager::V1_0::IServiceManager::Get();
    if (hdiServiceMgr_ == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "hdi service manager is nullptr, Try again after %{public}u second", RETRY_TIME);
        SendEvent(BatteryServiceEventHandler::EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER, RETRY_TIME);
        return false;
    }

    hdiServStatListener_ = new HdiServiceStatusListener(HdiServiceStatusListener::StatusCallback(
        [&](const OHOS::HDI::ServiceManager::V1_0::ServiceStatus &status) {
            RETURN_IF(status.serviceName != BATTERY_HDI_NAME || status.deviceClass != DEVICE_CLASS_DEFAULT);

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
        return false;
    }
    return true;
}

void BatteryService::SendEvent(int32_t event, int64_t delayTime)
{
    RETURN_IF_WITH_LOG(handler_ == nullptr, "handler is nullptr");
    handler_->RemoveEvent(event);
    handler_->SendEvent(event, 0, delayTime);
}

void BatteryService::OnStop()
{
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
}

bool BatteryService::IsLastPlugged()
{
    if (g_lastPluggedType != BatteryPluggedType::PLUGGED_TYPE_NONE &&
        g_lastPluggedType != BatteryPluggedType::PLUGGED_TYPE_BUTT) {
        return true;
    }
    return false;
}

bool BatteryService::IsNowPlugged(BatteryPluggedType pluggedType)
{
    if (pluggedType != BatteryPluggedType::PLUGGED_TYPE_NONE &&
        pluggedType != BatteryPluggedType::PLUGGED_TYPE_BUTT) {
        return true;
    }
    return false;
}

bool BatteryService::IsPlugged(BatteryPluggedType pluggedType)
{
    if (!IsLastPlugged() && IsNowPlugged(pluggedType)) {
        return true;
    }
    return false;
}

bool BatteryService::IsUnplugged(BatteryPluggedType pluggedType)
{
    if (IsLastPlugged() && !IsNowPlugged(pluggedType)) {
        return true;
    }
    return false;
}

void BatteryService::WakeupDevice(BatteryPluggedType pluggedType)
{
    if (IsPlugged(pluggedType) || IsUnplugged(pluggedType)) {
        PowerMgrClient::GetInstance().WakeupDevice();
    }
    g_lastPluggedType = pluggedType;
}

void BatteryService::HandlePopupEvent(int32_t capacity)
{
    if ((capacity < warnCapacity_) && !isLowPower_) {
        ShowBatteryDialog();
        g_powerMgrClient.RefreshActivity(UserActivityType::USER_ACTIVITY_TYPE_ATTENTION);
        isLowPower_ = true;
        return;
        }
    if (capacity >= warnCapacity_ && isLowPower_) {
        DestoryBatteryDialog();
        isLowPower_ = false;
        return;
    }
}

bool BatteryService::ShowBatteryDialog()
{
    BATTERY_HILOGD(COMP_SVC, "ShowBatteryDialog start.");
    auto client = AbilityManagerClient::GetInstance();
    if (client == nullptr) {
        return false;
    }
    AAFwk::Want want;
    want.SetElementName("com.ohos.powerdialog", "BatteryServiceExtAbility");
    int32_t result = client->StartAbility(want);
    if (result != 0) {
        BATTERY_HILOGE(COMP_SVC, "ShowBatteryDialog failed, result = %{public}d", result);
        return false;
    }
    BATTERY_HILOGD(COMP_SVC, "ShowBatteryDialog success.");
    return true;
}

bool BatteryService::DestoryBatteryDialog()
{
    BATTERY_HILOGD(COMP_SVC, "DestoryBatteryDialog start.");
    auto client = AbilityManagerClient::GetInstance();
    if (client == nullptr) {
        return false;
    }
    AAFwk::Want want;
    want.SetElementName("com.ohos.powerdialog", "BatteryServiceExtAbility");
    int32_t result = client->StopServiceAbility(want);
    if (result != 0) {
        BATTERY_HILOGE(COMP_SVC, "DestoryBatteryDialog failed, result = %{public}d", result);
        return false;
    }
    BATTERY_HILOGD(COMP_SVC, "DestoryBatteryDialog success.");
    return true;
}

void BatteryService::HandleTemperature(int32_t temperature)
{
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if (((temperature <= lowTemperature_) || (temperature >= highTemperature_)) &&
        (highTemperature_ != lowTemperature_)) {
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }
}

void BatteryService::HandleCapacity(int32_t capacity, BatteryChargeState chargeState)
{
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if ((capacity <= shutdownCapacityThreshold_) &&
        ((chargeState == BatteryChargeState::CHARGE_STATE_NONE) ||
         (chargeState == BatteryChargeState::CHARGE_STATE_BUTT))) {
        std::string reason = "LowCapacity";
        powerMgrClient.ShutDownDevice(reason);
    }
}

int32_t BatteryService::GetCapacity()
{
    int capacity = BATTERY_FULL_CAPACITY;
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
    V1_1::BatteryChargeState chargeState = V1_1::BatteryChargeState(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return BatteryChargeState(chargeState);
    }

    iBatteryInterface_->GetChargeState(chargeState);
    return BatteryChargeState(chargeState);
}

BatteryHealthState BatteryService::GetHealthStatus()
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    V1_1::BatteryHealthState healthState = V1_1::BatteryHealthState(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return BatteryHealthState(healthState);
    }

    iBatteryInterface_->GetHealthState(healthState);
    return BatteryHealthState(healthState);
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    V1_1::BatteryPluggedType pluggedType = V1_1::BatteryPluggedType(0);

    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return BatteryPluggedType(pluggedType);
    }

    iBatteryInterface_->GetPluggedType(pluggedType);
    return BatteryPluggedType(pluggedType);
}

int32_t BatteryService::GetVoltage()
{
    int voltage;
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }

    iBatteryInterface_->GetVoltage(voltage);
    return voltage;
}

bool BatteryService::GetPresent()
{
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

    if (((capacity - lastCapacity_) >= 1) && (lastCapacity_ >= 0) && chargeFlag_) {
        int64_t onceTime = (GetCurrentTime() - lastTime_) / (capacity - lastCapacity_);
        remainTime_ = (BATTERY_FULL_CAPACITY - capacity) * onceTime;
        lastCapacity_ = capacity;
        lastTime_ = GetCurrentTime();
    }
}

int64_t BatteryService::GetRemainingChargeTime()
{
    if (!Permission::IsSystem()) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "system permission denied.");
        return INVALID_REMAINING_CHARGE_TIME_VALUE;
    }
    return remainTime_;
}

bool IsCapacityLevelDefined(int32_t capacityThreshold)
{
    return capacityThreshold != INVALID_BATT_INT_VALUE;
}

BatteryCapacityLevel BatteryService::GetCapacityLevel()
{
    BatteryCapacityLevel batteryCapacityLevel = BatteryCapacityLevel::LEVEL_NONE;
    int32_t capacity = GetCapacity();
    if (IsCapacityLevelDefined(shutdownCapacityThreshold_) && capacity > 0 && capacity <= shutdownCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_SHUTDOWN;
    } else if (IsCapacityLevelDefined(criticalCapacityThreshold_) && capacity > shutdownCapacityThreshold_ &&
        capacity <= criticalCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_CRITICAL;
    } else if (IsCapacityLevelDefined(warningCapacityThreshold_) && capacity > criticalCapacityThreshold_ &&
        capacity <= warningCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_WARNING;
    } else if (IsCapacityLevelDefined(lowCapacityThreshold_) && capacity > warningCapacityThreshold_ &&
        capacity <= lowCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_LOW;
    } else if (IsCapacityLevelDefined(normalCapacityThreshold_) && capacity > lowCapacityThreshold_ &&
        capacity <= normalCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_NORMAL;
    } else if (IsCapacityLevelDefined(highCapacityThreshold_) && capacity > normalCapacityThreshold_ &&
        capacity <= highCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_HIGH;
    } else if (IsCapacityLevelDefined(fullCapacityThreshold_) && capacity > highCapacityThreshold_ &&
        capacity <= fullCapacityThreshold_) {
        batteryCapacityLevel = BatteryCapacityLevel::LEVEL_FULL;
    }
    return batteryCapacityLevel;
}

int32_t BatteryService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    if (!Permission::IsSystem()) {
        return ERR_PERMISSION_DENIED;
    }
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    if ((args.empty()) || (args[0].size() != HELP_DUMP_PARAM)) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "param cannot be empty or the length is not 2");
        dprintf(fd, "cmd param number is not equal to 2\n");
        batteryDump.DumpHelp(fd);
        return ERR_NO_INIT;
    }
    if (args[0].compare(u"-d") == 0) {
        ShowBatteryDialog();
        dprintf(fd, "show low power dialog \n");
        return ERR_OK;
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

    return ERR_OK;
}

void BatteryService::MockUnplugged(bool isUnplugged)
{
    V1_1::BatteryInfo event;
    if (!iBatteryInterface_) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }

    iBatteryInterface_->GetBatteryInfo(event);
    ConvertingEvent(event);
    if (isUnplugged) {
        batteryInfo_.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
        batteryInfo_.SetPluggedMaxCurrent(0);
        batteryInfo_.SetPluggedMaxVoltage(0);
        batteryInfo_.SetChargeState(BatteryChargeState::CHARGE_STATE_NONE);
        HandleBatteryInfo();
        isMockUnplugged_ = true;
    } else {
        isMockUnplugged_ = false;
        HandleBatteryInfo();
    }
}

bool BatteryService::IsMockUnplugged()
{
    return isMockUnplugged_;
}
} // namespace PowerMgr
} // namespace OHOS

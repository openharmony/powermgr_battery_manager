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

#include "battery_service.h"

#include <cstdio>
#include <ctime>
#include <functional>
#include <new>

#include "ability_manager_client.h"
#include "errors.h"
#include "hdf_device_class.h"
#include "hdf_service_status.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "permission.h"
#include "power_common.h"
#include "power_mgr_client.h"
#include "ffrt_utils.h"
#include "sysparam.h"
#include "system_ability_definition.h"
#include "xcollie/watchdog.h"

#include "battery_callback.h"
#include "battery_config.h"
#include "battery_dump.h"
#include "battery_log.h"
#include "power_vibrator.h"
#include "v2_0/ibattery_callback.h"

using namespace OHOS::HDI::Battery;
using namespace OHOS::AAFwk;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* BATTERY_SERVICE_NAME = "BatteryService";
constexpr const char* BATTERY_HDI_NAME = "battery_interface_service";
constexpr int32_t BATTERY_FULL_CAPACITY = 100;
constexpr uint32_t RETRY_TIME = 1000;
constexpr uint32_t SHUTDOWN_DELAY_TIME_MS = 60000;
const std::string BATTERY_VIBRATOR_CONFIG_FILE = "etc/battery/battery_vibrator.json";
const std::string VENDOR_BATTERY_VIBRATOR_CONFIG_FILE = "/vendor/etc/battery/battery_vibrator.json";
const std::string SYSTEM_BATTERY_VIBRATOR_CONFIG_FILE = "/system/etc/battery/battery_vibrator.json";
sptr<BatteryService> g_service = DelayedSpSingleton<BatteryService>::GetInstance();
FFRTQueue g_queue("battery_service");
FFRTHandle g_lowCapacityShutdownHandle = nullptr;
BatteryPluggedType g_lastPluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
static PowerMgrClient& g_powerMgrClient = PowerMgrClient::GetInstance();
SysParam::BootCompletedCallback g_bootCompletedCallback;
}
std::atomic_bool BatteryService::isBootCompleted_ = false;

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
    InitBatteryInfo();
    ready_ = true;
}

bool BatteryService::Init()
{
    InitConfig();
    if (!batteryNotify_) {
        batteryNotify_ = std::make_unique<BatteryNotify>();
    }
    VibratorInit();
    RegisterBootCompletedCallback();
    return true;
}

void BatteryService::RegisterBootCompletedCallback()
{
    g_bootCompletedCallback = []() {
        isBootCompleted_ = true;
    };
    SysParam::RegisterBootCompletedCallback(g_bootCompletedCallback);
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
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        iBatteryInterface_ = V2_0::IBatteryInterface::Get();
        BATTERY_HILOGE(COMP_SVC, "failed to get battery hdi interface");
        RETURN_IF_WITH_RET(iBatteryInterface_ == nullptr, false);
    }
    sptr<V2_0::IBatteryCallback> callback = new BatteryCallback();
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

int32_t BatteryService::HandleBatteryCallbackEvent(const V2_0::BatteryInfo& event)
{
    if (isMockUnplugged_ || isMockCapacity_) {
        return ERR_OK;
    }

    ConvertingEvent(event);
    RETURN_IF_WITH_RET(lastBatteryInfo_ == batteryInfo_, ERR_OK);
    HandleBatteryInfo();
    return ERR_OK;
}

void BatteryService::ConvertingEvent(const V2_0::BatteryInfo& event)
{
    if (!isMockCapacity_) {
        batteryInfo_.SetCapacity(event.capacity);
    }
    if (!isMockUnplugged_) {
        batteryInfo_.SetPluggedType(BatteryPluggedType(event.pluggedType));
        batteryInfo_.SetPluggedMaxCurrent(event.pluggedMaxCurrent);
        batteryInfo_.SetPluggedMaxVoltage(event.pluggedMaxVoltage);
        batteryInfo_.SetChargeState(BatteryChargeState(event.chargeState));
    }
    batteryInfo_.SetVoltage(event.voltage);
    batteryInfo_.SetTemperature(event.temperature);
    batteryInfo_.SetHealthState(BatteryHealthState(event.healthState));
    batteryInfo_.SetChargeCounter(event.chargeCounter);
    batteryInfo_.SetTotalEnergy(event.totalEnergy);
    batteryInfo_.SetCurAverage(event.curAverage);
    batteryInfo_.SetRemainEnergy(event.remainEnergy);
    batteryInfo_.SetPresent(event.present);
    batteryInfo_.SetTechnology(event.technology);
    batteryInfo_.SetNowCurrent(event.curNow);
    batteryInfo_.SetChargeType(GetChargeType());
    batteryInfo_.SetUevent(event.uevent);
}

void BatteryService::InitBatteryInfo()
{
    batteryInfo_.SetCapacity(GetCapacity());
    batteryInfo_.SetPluggedType(GetPluggedType());
    batteryInfo_.SetChargeState(GetChargingStatus());
    batteryInfo_.SetVoltage(GetVoltage());
    batteryInfo_.SetTemperature(GetBatteryTemperature());
    batteryInfo_.SetHealthState(GetHealthStatus());
    batteryInfo_.SetTotalEnergy(GetTotalEnergy());
    batteryInfo_.SetCurAverage(GetCurrentAverage());
    batteryInfo_.SetRemainEnergy(GetRemainEnergy());
    batteryInfo_.SetPresent(GetPresent());
    batteryInfo_.SetTechnology(GetTechnology());
    batteryInfo_.SetNowCurrent(GetNowCurrent());
    batteryInfo_.SetChargeType(GetChargeType());
    HandleBatteryInfo();
}

void BatteryService::HandleBatteryInfo()
{
    BATTERY_HILOGI(FEATURE_BATT_INFO, "capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, "
        "healthState=%{public}d, pluggedType=%{public}d, pluggedMaxCurrent=%{public}d, "
        "pluggedMaxVoltage=%{public}d, chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, "
        "technology=%{public}s, currNow=%{public}d, totalEnergy=%{public}d, curAverage=%{public}d, "
        "remainEnergy=%{public}d, chargeType=%{public}d, event=%{public}s", batteryInfo_.GetCapacity(),
        batteryInfo_.GetVoltage(), batteryInfo_.GetTemperature(), batteryInfo_.GetHealthState(),
        batteryInfo_.GetPluggedType(), batteryInfo_.GetPluggedMaxCurrent(), batteryInfo_.GetPluggedMaxVoltage(),
        batteryInfo_.GetChargeState(), batteryInfo_.GetChargeCounter(), batteryInfo_.IsPresent(),
        batteryInfo_.GetTechnology().c_str(), batteryInfo_.GetNowCurrent(), batteryInfo_.GetTotalEnergy(),
        batteryInfo_.GetCurAverage(), batteryInfo_.GetRemainEnergy(), batteryInfo_.GetChargeType(),
        batteryInfo_.GetUevent().c_str());

    batteryLight_.UpdateColor(batteryInfo_.GetChargeState(), batteryInfo_.GetCapacity());
    WakeupDevice(batteryInfo_.GetPluggedType());
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
        FFRTTask retryTask = [this] {
            return RegisterHdiStatusListener();
        };
        FFRTUtils::SubmitDelayTask(retryTask, RETRY_TIME, g_queue);
        return false;
    }

    hdiServStatListener_ = new HdiServiceStatusListener(HdiServiceStatusListener::StatusCallback(
        [this](const OHOS::HDI::ServiceManager::V1_0::ServiceStatus &status) {
            RETURN_IF(status.serviceName != BATTERY_HDI_NAME || status.deviceClass != DEVICE_CLASS_DEFAULT);

            std::lock_guard<std::shared_mutex> lock(mutex_);
            if (status.status == SERVIE_STATUS_START) {
                FFRTTask task = [this] {
                    (void)RegisterBatteryHdiCallback();
#ifdef BATTERY_MANAGER_SET_LOW_CAPACITY_THRESHOLD
                    SetLowCapacityThreshold();
#endif
                    return;
                };
                FFRTUtils::SubmitTask(task);
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
        FFRTTask retryTask = [this] {
            return RegisterHdiStatusListener();
        };
        FFRTUtils::SubmitDelayTask(retryTask, RETRY_TIME, g_queue);
        return false;
    }
    return true;
}

void BatteryService::OnStop()
{
    if (!ready_) {
        return;
    }
    ready_ = false;
    isBootCompleted_ = false;

    std::lock_guard<std::shared_mutex> lock(mutex_);
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

bool BatteryService::IsCharging(BatteryChargeState chargeState)
{
    return chargeState == BatteryChargeState::CHARGE_STATE_ENABLE;
}

void BatteryService::WakeupDevice(BatteryPluggedType pluggedType)
{
    if (IsPlugged(pluggedType) || IsUnplugged(pluggedType)) {
        PowerMgrClient::GetInstance().WakeupDevice();
    }
    g_lastPluggedType = pluggedType;
}

void BatteryService::HandleTemperature(int32_t temperature)
{
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if (((temperature <= lowTemperature_) || (temperature >= highTemperature_)) &&
        (highTemperature_ != lowTemperature_)) {
        powerMgrClient.ShutDownDevice("TemperatureOutOfRange");
    }
}

void BatteryService::HandleCapacity(int32_t capacity, BatteryChargeState chargeState)
{
    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if ((capacity <= shutdownCapacityThreshold_) &&
        (g_lowCapacityShutdownHandle == nullptr) &&
        ((chargeState == BatteryChargeState::CHARGE_STATE_NONE) ||
         (chargeState == BatteryChargeState::CHARGE_STATE_BUTT))) {
        BATTERY_HILOGI(COMP_SVC, "HandleCapacity begin to submit task");
        FFRTTask task = [&] {
            BATTERY_HILOGI(COMP_SVC, "HandleCapacity begin to shutdown");
            powerMgrClient.ShutDownDevice("LowCapacity");
        };
        g_lowCapacityShutdownHandle = FFRTUtils::SubmitDelayTask(task, SHUTDOWN_DELAY_TIME_MS, g_queue);
    }

    if (IsCharging(chargeState) && g_lowCapacityShutdownHandle != nullptr) {
        BATTERY_HILOGI(COMP_SVC, "HandleCapacity cancel shutdown task");
        FFRTUtils::CancelTask(g_lowCapacityShutdownHandle, g_queue);
        g_lowCapacityShutdownHandle = nullptr;
    }
}

int32_t BatteryService::GetCapacity()
{
    if (isMockCapacity_) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Return mock battery capacity");
        return batteryInfo_.GetCapacity();
    }
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    int32_t capacity = BATTERY_FULL_CAPACITY;
    iBatteryInterface_->GetCapacity(capacity);
    return capacity;
}

bool BatteryService::ChangePath(const std::string path)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter");
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return false;
    }
    iBatteryInterface_->ChangePath(path);
    return true;
}

#ifdef BATTERY_MANAGER_SET_LOW_CAPACITY_THRESHOLD
void BatteryService::SetLowCapacityThreshold()
{
    const std::string thers = "low_battery_thers";
    if (iBatteryInterface_ == nullptr) {
            BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }
    BATTERY_HILOGI(FEATURE_BATT_INFO, "set low capacity thres: shutdownCapacityThreshold_ = %{public}d",
        shutdownCapacityThreshold_);
    iBatteryInterface_->SetBatteryConfig(thers, std::to_string(shutdownCapacityThreshold_));
}
#endif

int32_t BatteryService::SetBatteryConfig(const std::string& sceneName, const std::string& value)
{
    if (!Permission::IsSystem()) {
        BATTERY_HILOGI(FEATURE_BATT_INFO, "SetBatteryConfig failed, System permission intercept");
        return INVALID_BATT_INT_VALUE;
    }

    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter SetBatteryConfig");
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    return iBatteryInterface_->SetBatteryConfig(sceneName, value);
}

std::string BatteryService::GetBatteryConfig(const std::string& sceneName)
{
    if (!Permission::IsSystem()) {
        BATTERY_HILOGI(FEATURE_BATT_INFO, "GetBatteryConfig failed, System permission intercept");
        return "";
    }

    BATTERY_HILOGD(FEATURE_BATT_INFO, "Enter GetBatteryConfig");
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return "";
    }

    std::string result;
    int32_t ret = iBatteryInterface_->GetBatteryConfig(sceneName, result);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "get charge config failed, key:%{public}s", sceneName.c_str());
        return "";
    }

    return result;
}

bool BatteryService::IsBatteryConfigSupported(const std::string& sceneName)
{
    if (!Permission::IsSystem()) {
        BATTERY_HILOGI(FEATURE_BATT_INFO, "IsBatteryConfigSupported failed, System permission intercept");
        return false;
    }

    BATTERY_HILOGI(FEATURE_BATT_INFO, "Enter IsBatteryConfigSupported");
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return false;
    }

    bool result = false;
    int32_t ret = iBatteryInterface_->IsBatteryConfigSupported(sceneName, result);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "get support charge config failed, key:%{public}s", sceneName.c_str());
        return false;
    }
    return result;
}

BatteryChargeState BatteryService::GetChargingStatus()
{
    if (isMockUnplugged_) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Return mock charge status");
        return batteryInfo_.GetChargeState();
    }
    std::shared_lock<std::shared_mutex> lock(mutex_);
    V2_0::BatteryChargeState chargeState = V2_0::BatteryChargeState(0);
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
    std::shared_lock<std::shared_mutex> lock(mutex_);
    V2_0::BatteryHealthState healthState = V2_0::BatteryHealthState(0);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return BatteryHealthState(healthState);
    }

    iBatteryInterface_->GetHealthState(healthState);
    return BatteryHealthState(healthState);
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    if (isMockUnplugged_) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Return mock plugged type");
        return batteryInfo_.GetPluggedType();
    }
    std::shared_lock<std::shared_mutex> lock(mutex_);
    V2_0::BatteryPluggedType pluggedType = V2_0::BatteryPluggedType(0);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return BatteryPluggedType(pluggedType);
    }
    iBatteryInterface_->GetPluggedType(pluggedType);
    return BatteryPluggedType(pluggedType);
}

int32_t BatteryService::GetVoltage()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    int32_t voltage = INVALID_BATT_INT_VALUE;
    iBatteryInterface_->GetVoltage(voltage);
    return voltage;
}

bool BatteryService::GetPresent()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
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
    std::shared_lock<std::shared_mutex> lock(mutex_);
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
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    int32_t temperature = INVALID_BATT_INT_VALUE;
    iBatteryInterface_->GetTemperature(temperature);
    return temperature;
}

int32_t BatteryService::GetTotalEnergy()
{
    int32_t totalEnergy = INVALID_BATT_INT_VALUE;
    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetTotalEnergy totalEnergy: %{public}d", totalEnergy);
        return totalEnergy;
    }
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetTotalEnergy(totalEnergy);
    return totalEnergy;
}

int32_t BatteryService::GetCurrentAverage()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    int32_t curAverage = INVALID_BATT_INT_VALUE;
    iBatteryInterface_->GetCurrentAverage(curAverage);
    return curAverage;
}

int32_t BatteryService::GetNowCurrent()
{
    int32_t nowCurr = INVALID_BATT_INT_VALUE;
    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetNowCurrent nowCurr: %{public}d", nowCurr);
        return nowCurr;
    }

    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetCurrentNow(nowCurr);
    return nowCurr;
}

int32_t BatteryService::GetRemainEnergy()
{
    int32_t remainEnergy = INVALID_BATT_INT_VALUE;
    if (!Permission::IsSystem()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "GetRemainEnergy remainEnergy: %{public}d", remainEnergy);
        return remainEnergy;
    }
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ERR_NO_INIT;
    }
    iBatteryInterface_->GetRemainEnergy(remainEnergy);
    return remainEnergy;
}

ChargeType BatteryService::GetChargeType()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    V2_0::ChargeType chargeType = V2_0::ChargeType::CHARGE_TYPE_NONE;
    if (iBatteryInterface_ == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return ChargeType(chargeType);
    }

    iBatteryInterface_->GetChargeType(chargeType);
    return ChargeType(chargeType);
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
    if (!isBootCompleted_) {
        return ERR_NO_INIT;
    }
    if (!Permission::IsSystem()) {
        return ERR_PERMISSION_DENIED;
    }

    BatteryDump& batteryDump = BatteryDump::GetInstance();
    if ((args.empty()) || (args[0].compare(u"-h") == 0)) {
        batteryDump.DumpBatteryHelp(fd);
        return ERR_OK;
    }
    bool getBatteryInfo = batteryDump.GetBatteryInfo(fd, g_service, args);
    bool unplugged = batteryDump.MockUnplugged(fd, g_service, args);
    bool mockedCapacity = batteryDump.MockCapacity(fd, g_service, args);
    bool reset = batteryDump.Reset(fd, g_service, args);
    bool total = getBatteryInfo + unplugged + mockedCapacity + reset;
    if (!total) {
        dprintf(fd, "cmd param is invalid\n");
        batteryDump.DumpBatteryHelp(fd);
        return ERR_NO_INIT;
    }

    return ERR_OK;
}

void BatteryService::MockUnplugged()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (!iBatteryInterface_) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }
    isMockUnplugged_ = true;
    V2_0::BatteryInfo event;
    iBatteryInterface_->GetBatteryInfo(event);
    ConvertingEvent(event);
    batteryInfo_.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    batteryInfo_.SetPluggedMaxCurrent(0);
    batteryInfo_.SetPluggedMaxVoltage(0);
    batteryInfo_.SetChargeState(BatteryChargeState::CHARGE_STATE_NONE);
    HandleBatteryInfo();
}

bool BatteryService::IsMockUnplugged()
{
    return isMockUnplugged_;
}

void BatteryService::MockCapacity(int32_t capacity)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (!iBatteryInterface_) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }
    isMockCapacity_ = true;
    V2_0::BatteryInfo event;
    iBatteryInterface_->GetBatteryInfo(event);
    ConvertingEvent(event);
    batteryInfo_.SetCapacity(capacity);
    HandleBatteryInfo();
}

bool BatteryService::IsMockCapacity()
{
    return isMockCapacity_;
}

void BatteryService::Reset()
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (!iBatteryInterface_) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "iBatteryInterface_ is nullptr");
        return;
    }
    isMockUnplugged_ = false;
    isMockCapacity_ = false;
    V2_0::BatteryInfo event;
    iBatteryInterface_->GetBatteryInfo(event);
    ConvertingEvent(event);
    HandleBatteryInfo();
}

void BatteryService::VibratorInit()
{
    std::shared_ptr<PowerVibrator> vibrator = PowerVibrator::GetInstance();
    vibrator->LoadConfig(BATTERY_VIBRATOR_CONFIG_FILE,
        VENDOR_BATTERY_VIBRATOR_CONFIG_FILE, SYSTEM_BATTERY_VIBRATOR_CONFIG_FILE);
}

} // namespace PowerMgr
} // namespace OHOS

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
#include "file_ex.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "battery_log.h"
#include "battery_interface_proxy.h"
#include "power_mgr_client.h"

namespace OHOS {
namespace PowerMgr {
namespace {
const std::string BATTERY_SERVICE_NAME = "BatteryService";
constexpr int32_t COMMEVENT_REGISTER_RETRY_TIMES = 10;
constexpr int32_t COMMEVENT_REGISTER_WAIT_DELAY_US = 20000;
int32_t g_lastChargeState = 0;
bool g_initConfig = true;
}

const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    DelayedSpSingleton<BatteryService>::GetInstance().GetRefPtr());
sptr<IBatteryInterface> ibatteryInterface;

BatteryService::BatteryService()
    : SystemAbility(POWER_MANAGER_BATT_SERVICE_ID, true)
{
}

BatteryService::~BatteryService() {}

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

    while (commEventRetryTimes_ <= COMMEVENT_REGISTER_RETRY_TIMES) {
        if (!IsCommonEventServiceAbilityExist()) {
            commEventRetryTimes_++;
            usleep(COMMEVENT_REGISTER_WAIT_DELAY_US);
        } else {
            commEventRetryTimes_ = 0;
            break;
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
    HandleTemperature(event.temperature);
    batteryLed_->UpdateLedColor(event.chargeState, event.capacity);
    WakeupDevice(event.chargeState);

    BatteryServiceSubscriber::Update(batteryInfo);
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

bool BatteryService::IsCommonEventServiceAbilityExist()
{
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sysMgr) {
        BATTERY_HILOGI(COMP_SVC,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sysMgr->CheckSystemAbility(COMMON_EVENT_SERVICE_ID);
    if (!remote) {
        BATTERY_HILOGE(COMP_SVC, "No CesServiceAbility");
        return false;
    }

    return true;
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
} // namespace PowerMgr
} // namespace OHOS

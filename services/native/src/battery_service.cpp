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
#include "power_common.h"
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
    POWER_HILOGI(MODULE_BATT_SERVICE, "OnStart enter");
    if (ready_) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "OnStart is ready, nothing to do");
        return;
    }
    if (!(Init())) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "OnStart call init fail");
        return;
    }
    if (!(InitBatteryd())) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "OnStart call initBatteryd fail");
        return;
    }
    if (!Publish(this)) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "OnStart register to system ability manager failed");
        return;
    }
    ready_ = true;
    POWER_HILOGI(MODULE_BATT_SERVICE, "OnStart and add system ability success");
}

bool BatteryService::Init()
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "Init start");
    if (!eventRunner_) {
        eventRunner_ = AppExecFwk::EventRunner::Create(BATTERY_SERVICE_NAME);
        if (eventRunner_ == nullptr) {
            POWER_HILOGE(MODULE_BATT_SERVICE, "Init failed due to create EventRunner");
            return false;
        }
    }
    if (!handler_) {
        handler_ = std::make_shared<BatteryServiceEventHandler>(eventRunner_,
            DelayedSpSingleton<BatteryService>::GetInstance());
        if (handler_ == nullptr) {
            POWER_HILOGE(MODULE_BATT_SERVICE, "Init failed due to create handler error");
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
    POWER_HILOGI(MODULE_BATT_SERVICE, "Init success");
    return true;
}

bool BatteryService::InitBatteryd()
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "enter.");
    sptr<hdi::battery::v1_0::IBatteryCallback> callback =  new hdi::battery::v1_0::BatteryCallbackService();
    ibatteryInterface = hdi::battery::v1_0::IBatteryInterface::Get();
    if (ibatteryInterface == nullptr) {
        POWER_HILOGI(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return false;
    }
    ErrCode ret = ibatteryInterface->Register(callback);

    BatteryCallbackService::BatteryEventCallback eventCb =
        std::bind(&BatteryService::HandleBatteryCallbackEvent, this, std::placeholders::_1);
    BatteryCallbackService::RegisterBatteryEvent(eventCb);

    POWER_HILOGD(MODULE_BATT_SERVICE, "InitBatteryd ret: %{public}d", ret);
    return SUCCEEDED(ret);
}

void BatteryService::InitConfig()
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "enter.");
    batteryConfig_ = std::make_unique<HDI::Battery::V1_0::BatteryConfig>();
    if (batteryConfig_ == nullptr) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "instantiate batteryconfig error.");
        return;
    }
    batteryConfig_->Init();

    batteryLed_ = std::make_unique<HDI::Battery::V1_0::BatteryLed>();
    if (batteryLed_ == nullptr) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "instantiate BatteryLed error.");
        return;
    }
    batteryLed_->InitLedsSysfs();
}

int32_t BatteryService::HandleBatteryCallbackEvent(const CallbackInfo& event)
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "HandleBatteryCallbackEvent enter.");
    BatteryInfo batteryInfo;
    POWER_HILOGD(MODULE_BATT_SERVICE,
        "HandleCallbackInfo capacity=%{public}d, voltage=%{public}d, temperature=%{public}d, " \
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
    POWER_HILOGI(MODULE_BATT_SERVICE, "stop service");
    if (!ready_) {
        return;
    }
    eventRunner_.reset();
    handler_.reset();
    ready_ = false;

    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return;
    }
    ibatteryInterface->UnRegister();
}

bool BatteryService::IsCommonEventServiceAbilityExist()
{
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sysMgr) {
        POWER_HILOGI(MODULE_BATT_SERVICE,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sysMgr->CheckSystemAbility(COMMON_EVENT_SERVICE_ID);
    if (!remote) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "No CesServiceAbility");
        return false;
    }

    return true;
}
void BatteryService::WakeupDevice(const int32_t& chargestate)
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "enter");
    if ((g_lastChargeState == CHARGE_STATE_NONE || g_lastChargeState == CHARGE_STATE_RESERVED) &&
        (chargestate != CHARGE_STATE_NONE && chargestate !=CHARGE_STATE_RESERVED)) {
        auto& powerMgrClient = PowerMgrClient::GetInstance();
        powerMgrClient.WakeupDevice();
    }
    g_lastChargeState = chargestate;

    POWER_HILOGI(MODULE_BATT_SERVICE, "exit");
    return;
}

void BatteryService::HandleTemperature(const int32_t& temperature)
{
    POWER_HILOGI(MODULE_BATT_SERVICE, "enter");
    auto tempConf = batteryConfig_->GetTempConf();
    POWER_HILOGI(MODULE_BATT_SERVICE, "temperature=%{public}d, tempConf.lower=%{public}d, tempConf.upper=%{public}d",
        temperature, tempConf.lower, tempConf.upper);

    auto& powerMgrClient = PowerMgrClient::GetInstance();
    if (((temperature <= tempConf.lower) || (temperature >= tempConf.upper)) && (tempConf.lower != tempConf.upper)) {
        std::string reason = "TemperatureOutOfRange";
        powerMgrClient.ShutDownDevice(reason);
    }

    POWER_HILOGI(MODULE_BATT_SERVICE, "exit");
    return;
}

int32_t BatteryService::GetCapacity()
{
    int capacity;
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetCapacity(capacity);
    return capacity;
}

void BatteryService::ChangePath(const std::string path)
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return;
    }
    ibatteryInterface->ChangePath(path);
    return;
}

BatteryChargeState BatteryService::GetChargingStatus()
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    hdi::battery::v1_0::BatteryChargeState chargeState = hdi::battery::v1_0::BatteryChargeState(0);

    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryChargeState(chargeState);
    }

    ibatteryInterface->GetChargeState(chargeState);
    return OHOS::PowerMgr::BatteryChargeState(chargeState);
}

BatteryHealthState BatteryService::GetHealthStatus()
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    hdi::battery::v1_0::BatteryHealthState healthState = hdi::battery::v1_0::BatteryHealthState(0);

    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryHealthState(healthState);
    }

    ibatteryInterface->GetHealthState(healthState);
    return OHOS::PowerMgr::BatteryHealthState(healthState);
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    hdi::battery::v1_0::BatteryPluggedType pluggedType = hdi::battery::v1_0::BatteryPluggedType(0);

    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
    }

    ibatteryInterface->GetPluggedType(pluggedType);
    return OHOS::PowerMgr::BatteryPluggedType(pluggedType);
}

int32_t BatteryService::GetVoltage()
{
    int voltage;
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetVoltage(voltage);
    return voltage;
}

bool BatteryService::GetPresent()
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    bool present = false;

    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return present;
    }

    ibatteryInterface->GetPresent(present);
    return present;
}

std::string BatteryService::GetTechnology()
{
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return "";
    }

    std::string technology;
    ibatteryInterface->GetTechnology(technology);
    return technology;
}

int32_t BatteryService::GetBatteryTemperature()
{
    int temperature;
    POWER_HILOGE(MODULE_BATT_SERVICE, "enter");
    if (ibatteryInterface == nullptr) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "ibatteryInterface is nullptr");
        return ERR_NO_INIT;
    }
    ibatteryInterface->GetTemperature(temperature);
    return temperature;
}
} // namespace PowerMgr
} // namespace OHOS

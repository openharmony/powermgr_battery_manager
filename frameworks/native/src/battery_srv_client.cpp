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

#include "battery_srv_client.h"
#include "string_ex.h"
#include "datetime_ex.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "power_common.h"

using namespace OHOS::HDI::Battery;

namespace OHOS {
namespace PowerMgr {
BatterySrvClient::BatterySrvClient() {}
BatterySrvClient::~BatterySrvClient() {}

ErrCode BatterySrvClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == nullptr) {
        BATTERY_HILOGW(COMP_FWK, "Failed to get Registry");
        return E_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remoteObject_ = sysMgr->CheckSystemAbility(POWER_MANAGER_BATT_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        BATTERY_HILOGW(COMP_FWK, "GetSystemAbility failed");
        return E_GET_POWER_SERVICE_FAILED;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new BatterySrvDeathRecipient());
    if (deathRecipient_ == nullptr) {
        BATTERY_HILOGW(COMP_FWK, "Failed to create BatterySrvDeathRecipient");
        return ERR_NO_MEMORY;
    }
    if ((remoteObject_->IsProxyObject()) && (!remoteObject_->AddDeathRecipient(deathRecipient_))) {
        BATTERY_HILOGW(COMP_FWK, "Add death recipient to BatterySrv failed");
        return E_ADD_DEATH_RECIPIENT_FAILED;
    }

    proxy_ = iface_cast<IBatterySrv>(remoteObject_);
    BATTERY_HILOGI(COMP_FWK, "Connect BatterySrv ok");
    return ERR_OK;
}

void BatterySrvClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    BATTERY_HILOGW(COMP_FWK, "Enter");
    std::lock_guard<std::mutex> lock(mutex_);
    RETURN_IF(proxy_ == nullptr);
    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
}

void BatterySrvClient::BatterySrvDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    BATTERY_HILOGW(COMP_FWK, "Enter");
    if (remote == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "remote is nullptr");
        return;
    }
    BatterySrvClient::GetInstance().ResetProxy(remote);
    BATTERY_HILOGW(COMP_FWK, "Success");
}

int32_t BatterySrvClient::GetCapacity()
{
    int32_t capacity = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, capacity);
    capacity = proxy_->GetCapacity();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "capacity %{public}d", capacity);
    return capacity;
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    BatteryChargeState chargingState = BatteryChargeState::CHARGE_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, chargingState);
    chargingState = proxy_->GetChargingStatus();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "chargingState %{public}d", chargingState);
    return chargingState;
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    BatteryHealthState healthStatus = BatteryHealthState::HEALTH_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, healthStatus);
    healthStatus = proxy_->GetHealthStatus();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "healthStatus %{public}d", healthStatus);
    return healthStatus;
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, pluggedType);
    pluggedType = proxy_->GetPluggedType();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "pluggedType %{public}d", pluggedType);
    return pluggedType;
}

int32_t BatterySrvClient::GetVoltage()
{
    int32_t voltage = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, voltage);
    voltage = proxy_->GetVoltage();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "voltage %{public}d", voltage);
    return voltage;
}

bool BatterySrvClient::GetPresent()
{
    bool present = INVALID_BATT_BOOL_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, present);
    present = proxy_->GetPresent();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "present %{public}d", present);
    return present;
}

std::string BatterySrvClient::GetTechnology()
{
    std::string technology;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, technology);
    technology = proxy_->GetTechnology();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "technology %{public}s", technology.c_str());
    return technology;
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, temperature);
    temperature = proxy_->GetBatteryTemperature();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "temperature %{public}d", temperature);
    return temperature;
}

int32_t BatterySrvClient::GetBatteryLevel()
{
    int32_t level = INVALID_BATT_LEVEL_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, level);
    level = proxy_->GetBatteryLevel();
    return level;
}

int64_t BatterySrvClient::GetRemainingChargeTime()
{
    int64_t time = INVALID_REMAINING_CHARGE_TIME_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, time);
    time = proxy_->GetRemainingChargeTime();
    return time;
}
}  // namespace PowerMgr
}  // namespace OHOS

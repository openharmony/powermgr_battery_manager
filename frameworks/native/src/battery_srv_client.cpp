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

#include "battery_srv_client.h"

#include "new"
#include "refbase.h"
#include "errors.h"
#include "iremote_broker.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "battery_info.h"
#include "battery_log.h"
#include "power_mgr_errors.h"
#include "power_common.h"

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
    return ERR_OK;
}

void BatterySrvClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
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
    BATTERY_HILOGW(COMP_FWK, "BateryService Died");
    if (remote == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "remote is nullptr");
        return;
    }
    BatterySrvClient::GetInstance().ResetProxy(remote);
}

int32_t BatterySrvClient::GetCapacity()
{
    int32_t capacity = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, capacity);
    capacity = proxy_->GetCapacity();
    return capacity;
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    BatteryChargeState chargingState = BatteryChargeState::CHARGE_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, chargingState);
    chargingState = proxy_->GetChargingStatus();
    return chargingState;
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    BatteryHealthState healthStatus = BatteryHealthState::HEALTH_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, healthStatus);
    healthStatus = proxy_->GetHealthStatus();
    return healthStatus;
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, pluggedType);
    pluggedType = proxy_->GetPluggedType();
    return pluggedType;
}

int32_t BatterySrvClient::GetVoltage()
{
    int32_t voltage = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, voltage);
    voltage = proxy_->GetVoltage();
    return voltage;
}

bool BatterySrvClient::GetPresent()
{
    bool present = INVALID_BATT_BOOL_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, present);
    present = proxy_->GetPresent();
    return present;
}

std::string BatterySrvClient::GetTechnology()
{
    std::string technology;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, technology);
    technology = proxy_->GetTechnology();
    return technology;
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, temperature);
    temperature = proxy_->GetBatteryTemperature();
    return temperature;
}

int32_t BatterySrvClient::GetNowCurrent()
{
    int32_t nowCurrent = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, nowCurrent);
    nowCurrent = proxy_->GetNowCurrent();
    return nowCurrent;
}

int32_t BatterySrvClient::GetRemainEnergy()
{
    int32_t remainEnergy = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, remainEnergy);
    remainEnergy = proxy_->GetRemainEnergy();
    return remainEnergy;
}

int32_t BatterySrvClient::GetTotalEnergy()
{
    int32_t totalEnergy = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, totalEnergy);
    totalEnergy = proxy_->GetTotalEnergy();
    return totalEnergy;
}

BatteryCapacityLevel BatterySrvClient::GetCapacityLevel()
{
    BatteryCapacityLevel level = BatteryCapacityLevel::LEVEL_NONE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, level);
    level = proxy_->GetCapacityLevel();
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

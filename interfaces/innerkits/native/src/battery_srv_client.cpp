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
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sm == nullptr) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "%{public}s:fail to get Registry", __func__);
        return E_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remoteObject_ = sm->CheckSystemAbility(POWER_MANAGER_BATT_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "GetSystemAbility failed.");
        return E_GET_POWER_SERVICE_FAILED;
    }

    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new BatterySrvDeathRecipient());
    if (deathRecipient_ == nullptr) {
        POWER_HILOGE(MODULE_INNERKIT, "%{public}s :Failed to create BatterySrvDeathRecipient!", __func__);
        return ERR_NO_MEMORY;
    }
    if ((remoteObject_->IsProxyObject()) && (!remoteObject_->AddDeathRecipient(deathRecipient_))) {
        POWER_HILOGE(MODULE_INNERKIT, "%{public}s :Add death recipient to BatterySrv failed.", __func__);
        return E_ADD_DEATH_RECIPIENT_FAILED;
    }

    proxy_ = iface_cast<IBatterySrv>(remoteObject_);
    POWER_HILOGI(MODULE_BATT_INNERKIT, "%{public}s :Connect BatterySrv ok.", __func__);
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
    if (remote == nullptr) {
        POWER_HILOGE(MODULE_BATT_INNERKIT, "BatterySrvDeathRecipient::OnRemoteDied failed, remote is nullptr.");
        return;
    }
    BatterySrvClient::GetInstance().ResetProxy(remote);
    POWER_HILOGI(MODULE_BATT_INNERKIT, "BatterySrvDeathRecipient::Recv death notice.");
}

int32_t BatterySrvClient::GetCapacity()
{
    int32_t capacity = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, capacity);
    capacity = proxy_->GetCapacity();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetCapacity Success!");
    return capacity;
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    BatteryChargeState chargingstate = BatteryChargeState::CHARGE_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, chargingstate);
    chargingstate = proxy_->GetChargingStatus();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetChargingStatus Success!");
    return chargingstate;
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, healthState);
    healthState = proxy_->GetHealthStatus();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetHealthStatus Success!");
    return healthState;
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, pluggedType);
    pluggedType = proxy_->GetPluggedType();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetPluggedType Success!");
    return pluggedType;
}

int32_t BatterySrvClient::GetVoltage()
{
    int32_t voltage = INVALID_BATT_INT_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, voltage);
    voltage = proxy_->GetVoltage();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetVoltage Success!");
    return voltage;
}

bool BatterySrvClient::GetPresent()
{
    bool present = INVALID_BATT_BOOL_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, present);
    present = proxy_->GetPresent();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetPresent Success!");
    return present;
}

std::string BatterySrvClient::GetTechnology()
{
    std::string technology;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, technology);
    technology = proxy_->GetTechnology();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetTechnology Success!");
    return technology;
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    RETURN_IF_WITH_RET(Connect() != ERR_OK, temperature);
    temperature = proxy_->GetBatteryTemperature();
    POWER_HILOGI(MODULE_BATT_INNERKIT, " Calling GetBatteryTemperature Success!");
    return temperature;
}
}  // namespace PowerMgr
}  // namespace OHOS
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

sptr<IBatterySrv> BatterySrvClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return proxy_;
    }
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "Failed to get Registry");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject_ = sysMgr->CheckSystemAbility(POWER_MANAGER_BATT_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "GetSystemAbility failed");
        return nullptr;
    }
    sptr<IRemoteObject::DeathRecipient> drt = new(std::nothrow) BatterySrvDeathRecipient(*this);
    if (drt == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "Failed to create BatterySrvDeathRecipient");
        return nullptr;
    }
    if ((remoteObject_->IsProxyObject()) && (!remoteObject_->AddDeathRecipient(drt))) {
        BATTERY_HILOGE(COMP_FWK, "Add death recipient to BatterySrv failed");
        return nullptr;
    }

    proxy_ = iface_cast<IBatterySrv>(remoteObject_);
    deathRecipient_ = drt;
    BATTERY_HILOGI(COMP_FWK, "Connecting PowerMgrService success");
    return proxy_;
}

void BatterySrvClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        BATTERY_HILOGE(COMP_FWK, "OnRemoteDied failed, remote is nullptr");
        return;
    }

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
    BATTERY_HILOGW(COMP_FWK, "Recv death notice, BateryService Died");
    client_.ResetProxy(remote);
}

int32_t BatterySrvClient::GetCapacity()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->GetCapacity();
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryChargeState::CHARGE_STATE_BUTT);
    return proxy->GetChargingStatus();
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryHealthState::HEALTH_STATE_BUTT);
    return proxy->GetHealthStatus();
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryPluggedType::PLUGGED_TYPE_BUTT);
    return proxy->GetPluggedType();
}

int32_t BatterySrvClient::GetVoltage()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->GetVoltage();
}

bool BatterySrvClient::GetPresent()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_BOOL_VALUE);
    return proxy->GetPresent();
}

std::string BatterySrvClient::GetTechnology()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, "");
    return proxy->GetTechnology();
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_TEMP_VALUE);
    return proxy->GetBatteryTemperature();
}

int32_t BatterySrvClient::GetNowCurrent()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->GetNowCurrent();
}

int32_t BatterySrvClient::GetRemainEnergy()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->GetRemainEnergy();
}

int32_t BatterySrvClient::GetTotalEnergy()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->GetTotalEnergy();
}

BatteryCapacityLevel BatterySrvClient::GetCapacityLevel()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryCapacityLevel::LEVEL_NONE);
    return proxy->GetCapacityLevel();
}

int64_t BatterySrvClient::GetRemainingChargeTime()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_REMAINING_CHARGE_TIME_VALUE);
    return proxy->GetRemainingChargeTime();
}

int32_t BatterySrvClient::SetBatteryConfig(const std::string& sceneName, const std::string& value)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    return proxy->SetBatteryConfig(sceneName, value);
}

std::string BatterySrvClient::GetBatteryConfig(const std::string& sceneName)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, "");
    return proxy->GetBatteryConfig(sceneName);
}

bool BatterySrvClient::IsBatteryConfigSupported(const std::string& sceneName)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, false);
    return proxy->IsBatteryConfigSupported(sceneName);
}
}  // namespace PowerMgr
}  // namespace OHOS

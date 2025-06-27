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
    BATTERY_HILOGI(COMP_FWK, "Connecting PowerMgrService success, pid=%{public}d", getpid());
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
    int32_t capacity = INVALID_BATT_INT_VALUE;
    auto ret = proxy->GetCapacity(capacity);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetCapacity ret = %{public}d", ret);
        return INVALID_BATT_INT_VALUE;
    }
    return capacity;
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryChargeState::CHARGE_STATE_BUTT);
    uint32_t chargeState = static_cast<uint32_t>(BatteryChargeState::CHARGE_STATE_BUTT);
    auto ret = proxy->GetChargingStatus(chargeState);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetChargingStatus ret = %{public}d", ret);
        return BatteryChargeState::CHARGE_STATE_BUTT;
    }
    return static_cast<BatteryChargeState>(chargeState);
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryHealthState::HEALTH_STATE_BUTT);
    uint32_t healthState = static_cast<uint32_t>(BatteryHealthState::HEALTH_STATE_BUTT);
    auto ret = proxy->GetHealthStatus(healthState);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetHealthStatus ret = %{public}d", ret);
        return BatteryHealthState::HEALTH_STATE_BUTT;
    }
    return static_cast<BatteryHealthState>(healthState);
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryPluggedType::PLUGGED_TYPE_BUTT);
    uint32_t pluggedType = static_cast<uint32_t>(BatteryPluggedType::PLUGGED_TYPE_BUTT);
    auto ret = proxy->GetPluggedType(pluggedType);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetPluggedType ret = %{public}d", ret);
        return BatteryPluggedType::PLUGGED_TYPE_BUTT;
    }
    return static_cast<BatteryPluggedType>(pluggedType);
}

int32_t BatterySrvClient::GetVoltage()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    int32_t voltage = INVALID_BATT_INT_VALUE;
    auto ret = proxy->GetVoltage(voltage);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetVoltage ret = %{public}d", ret);
        return INVALID_BATT_INT_VALUE;
    }
    return voltage;
}

bool BatterySrvClient::GetPresent()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_BOOL_VALUE);
    bool present = INVALID_BATT_BOOL_VALUE;
    auto ret = proxy->GetPresent(present);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetPresent ret = %{public}d", ret);
        return INVALID_BATT_BOOL_VALUE;
    }
    return present;
}

std::string BatterySrvClient::GetTechnology()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, "");
    std::string technology = INVALID_STRING_VALUE;
    auto ret = proxy->GetTechnology(technology);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetTechnology ret = %{public}d", ret);
        return INVALID_STRING_VALUE;
    }
    return technology;
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_TEMP_VALUE);
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    auto ret = proxy->GetBatteryTemperature(temperature);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetBatteryTemperature ret = %{public}d", ret);
        return INVALID_BATT_TEMP_VALUE;
    }
    return temperature;
}

int32_t BatterySrvClient::GetNowCurrent()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    int32_t nowCurr = INVALID_BATT_INT_VALUE;
    auto ret = proxy->GetNowCurrent(nowCurr);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetNowCurrent ret = %{public}d", ret);
        return INVALID_BATT_INT_VALUE;
    }
    return nowCurr;
}

int32_t BatterySrvClient::GetRemainEnergy()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    int32_t remainEnergy = INVALID_BATT_INT_VALUE;
    auto ret = proxy->GetRemainEnergy(remainEnergy);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetRemainEnergy ret = %{public}d", ret);
        return INVALID_BATT_INT_VALUE;
    }
    return remainEnergy;
}

int32_t BatterySrvClient::GetTotalEnergy()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_BATT_INT_VALUE);
    int32_t totalEnergy = INVALID_BATT_INT_VALUE;
    auto ret = proxy->GetTotalEnergy(totalEnergy);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetTotalEnergy ret = %{public}d", ret);
        return INVALID_BATT_INT_VALUE;
    }
    return totalEnergy;
}

BatteryCapacityLevel BatterySrvClient::GetCapacityLevel()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryCapacityLevel::LEVEL_NONE);
    uint32_t batteryCapacityLevel = static_cast<uint32_t>(BatteryCapacityLevel::LEVEL_NONE);
    auto ret = proxy->GetCapacityLevel(batteryCapacityLevel);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetCapacityLevel ret = %{public}d", ret);
        return BatteryCapacityLevel::LEVEL_NONE;
    }
    return static_cast<BatteryCapacityLevel>(batteryCapacityLevel);
}

int64_t BatterySrvClient::GetRemainingChargeTime()
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, INVALID_REMAINING_CHARGE_TIME_VALUE);
    int64_t remainTime = INVALID_REMAINING_CHARGE_TIME_VALUE;
    auto ret = proxy->GetRemainingChargeTime(remainTime);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetRemainingChargeTime ret = %{public}d", ret);
        return INVALID_REMAINING_CHARGE_TIME_VALUE;
    }
    return remainTime;
}

BatteryError BatterySrvClient::SetBatteryConfig(const std::string& sceneName, const std::string& value)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryError::ERR_CONNECTION_FAIL);
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_CONNECTION_FAIL);
    auto ret = proxy->SetBatteryConfig(sceneName, value, batteryErr);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "SetBatteryConfig ret = %{public}d", ret);
        return BatteryError::ERR_CONNECTION_FAIL;
    }
    return static_cast<BatteryError>(batteryErr);
}

BatteryError BatterySrvClient::GetBatteryConfig(const std::string& sceneName, std::string& result)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryError::ERR_CONNECTION_FAIL);
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_CONNECTION_FAIL);
    auto ret = proxy->GetBatteryConfig(sceneName, result, batteryErr);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "GetBatteryConfig ret = %{public}d", ret);
        return BatteryError::ERR_CONNECTION_FAIL;
    }
    return static_cast<BatteryError>(batteryErr);
}

BatteryError BatterySrvClient::IsBatteryConfigSupported(const std::string& sceneName, bool& result)
{
    auto proxy = Connect();
    RETURN_IF_WITH_RET(proxy == nullptr, BatteryError::ERR_CONNECTION_FAIL);
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_CONNECTION_FAIL);
    auto ret = proxy->IsBatteryConfigSupported(sceneName, result, batteryErr);
    if (ret != ERR_OK) {
        BATTERY_HILOGE(COMP_FWK, "IsBatteryConfigSupported ret = %{public}d", ret);
        return BatteryError::ERR_CONNECTION_FAIL;
    }
    return static_cast<BatteryError>(batteryErr);
}
}  // namespace PowerMgr
}  // namespace OHOS

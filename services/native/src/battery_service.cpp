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

namespace OHOS {
namespace PowerMgr {
namespace {
const std::string BATTERY_SERVICE_NAME = "BatteryService";
constexpr int32_t COMMEVENT_REGISTER_RETRY_TIMES = 10;
constexpr int32_t COMMEVENT_REGISTER_WAIT_DELAY_US = 20000;
}

const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(
    DelayedSpSingleton<BatteryService>::GetInstance().GetRefPtr());

BatteryService::BatteryService()
    : SystemAbility(POWER_MANAGER_BATT_SERVICE_ID, true) {}

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
        POWER_HILOGE(MODULE_BATT_SERVICE, "OnStart register to system ability manager failed!");
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
        handler_ = std::make_shared<BatterysrvEventHandler>(eventRunner_,
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
    batterydSubscriber_ = new BatteryServiceSubscriber();
    ErrCode ret = BatterydClient::BindBatterydSubscriber(batterydSubscriber_);
    POWER_HILOGD(MODULE_BATT_SERVICE, "InitBatteryd ret: %{public}d", ret);
    return SUCCEEDED(ret);
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
    BatterydClient::UnbindBatterydSubscriber();
}

bool BatteryService::IsCommonEventServiceAbilityExist()
{
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sm) {
        POWER_HILOGI(MODULE_BATT_SERVICE,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sm->CheckSystemAbility(COMMON_EVENT_SERVICE_ABILITY_ID);
    if (!remote) {
        POWER_HILOGE(MODULE_BATT_SERVICE, "No CesServiceAbility");
        return false;
    }
    return true;
}

int32_t BatteryService::GetCapacity()
{
    return BatterydClient::GetCapacity();
}

BatteryChargeState BatteryService::GetChargingStatus()
{
    return BatterydClient::GetChargeState();
}

BatteryHealthState BatteryService::GetHealthStatus()
{
    return BatterydClient::GetHealthState();
}

BatteryPluggedType BatteryService::GetPluggedType()
{
    return BatterydClient::GetPluggedType();
}

int32_t BatteryService::GetVoltage()
{
    return BatterydClient::GetVoltage();
}

bool BatteryService::GetPresent()
{
    return BatterydClient::GetPresent();
}

std::string BatteryService::GetTechnology()
{
    return BatterydClient::GetTechnology();
}

int32_t BatteryService::GetBatteryTemperature()
{
    return BatterydClient::GetTemperature();
}
} // namespace PowerMgr
} // namespace OHOS

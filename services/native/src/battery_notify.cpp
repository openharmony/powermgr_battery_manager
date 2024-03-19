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

#include "battery_notify.h"
#include <regex>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_publish_info.h"
#include "common_event_support.h"
#include "errors.h"
#include "hisysevent.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "battery_config.h"
#include "battery_log.h"
#include "battery_service.h"
#include "power_vibrator.h"
#include "power_mgr_client.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace PowerMgr {
bool g_batteryLowOnce = false;
bool g_batteryOkOnce = false;
bool g_batteryConnectOnce = false;
bool g_batteryDisconnectOnce = false;
bool g_batteryChargingOnce = false;
bool g_batteryDischargingOnce = false;
bool g_commonEventInitSuccess = false;
OHOS::PowerMgr::BatteryCapacityLevel g_lastCapacityLevel = OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NONE;
const std::string POWER_SUPPLY = "SUBSYSTEM=power_supply";
const std::string SHUTDOWN = "shutdown";
const std::string REBOOT = "reboot";
const std::string SEND_COMMONEVENT = "sendcommonevent";
sptr<BatteryService> g_service = DelayedSpSingleton<BatteryService>::GetInstance();

BatteryNotify::BatteryNotify()
{
    const int32_t DEFAULT_LOW_CAPACITY = 20;
    lowCapacity_ = BatteryConfig::GetInstance().GetInt("soc.low", DEFAULT_LOW_CAPACITY);
    BATTERY_HILOGI(COMP_SVC, "Low broadcast power=%{public}d", lowCapacity_);
}

int32_t BatteryNotify::PublishEvents(BatteryInfo& info)
{
    if (g_commonEventInitSuccess) {
        BATTERY_HILOGI(COMP_SVC, "common event service ability init success");
    } else {
        if (!IsCommonEventServiceAbilityExist()) {
            return ERR_NO_INIT;
        }
    }
    if (info.GetUevent() != POWER_SUPPLY && info.GetUevent() != "") {
        HandleUevent(info);
        return ERR_OK;
    }

    bool isAllSuccess = true;
    bool ret = PublishChangedEvent(info);
    isAllSuccess &= ret;
    ret = PublishChangedEventInner(info);
    isAllSuccess &= ret;
    ret = PublishLowEvent(info);
    isAllSuccess &= ret;
    ret = PublishOkayEvent(info);
    isAllSuccess &= ret;
    ret = PublishPowerConnectedEvent(info);
    isAllSuccess &= ret;
    ret = PublishPowerDisconnectedEvent(info);
    isAllSuccess &= ret;
    ret = PublishChargingEvent(info);
    isAllSuccess &= ret;
    ret = PublishDischargingEvent(info);
    isAllSuccess &= ret;
    ret = PublishChargeTypeChangedEvent(info);
    isAllSuccess &= ret;

    return isAllSuccess ? ERR_OK : ERR_NO_INIT;
}

void BatteryNotify::HandleUevent(BatteryInfo& info)
{
    std::string uevent = info.GetUevent();
    auto pos = uevent.rfind('$');
    if (pos != std::string::npos) {
        std::string ueventName = uevent.substr(0, pos);
        std::string ueventAct = uevent.substr(++pos);
        BATTERY_HILOGI(COMP_SVC, "%{public}s decision %{public}s",
            ueventName.c_str(), ueventAct.c_str());
        if (ueventAct == SHUTDOWN) {
            PowerMgrClient::GetInstance().ShutDownDevice(ueventName);
        } else if (ueventAct == REBOOT) {
            PowerMgrClient::GetInstance().RebootDevice(ueventName);
        } else if (ueventAct == SEND_COMMONEVENT) {
            info.SetUevent(ueventName);
            PublishChangedEvent(info);
        } else {
            BATTERY_HILOGE(COMP_SVC, "undefine uevent act %{public}s", ueventAct.c_str());
        }
    }
    BATTERY_HILOGI(COMP_SVC, "handle uevent info %{public}s", uevent.c_str());
}

bool BatteryNotify::PublishChargeTypeChangedEvent(const BatteryInfo& info)
{
    ChargeType chargeType = info.GetChargeType();
    bool isSuccess = true;
    if (batteryInfoChargeType_ == chargeType) {
        BATTERY_HILOGD(COMP_SVC, "No need to send chargetype event");
        return isSuccess;
    }
    batteryInfoChargeType_ = chargeType;
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_CHARGE_TYPE_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);

    data.SetCode(static_cast<int32_t>(chargeType));
    BATTERY_HILOGD(COMP_SVC, "publisher chargeType=%{public}d", chargeType);
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(COMP_SVC, "failed to publish battery charge type event");
    }

    return isSuccess;
}

bool BatteryNotify::IsCommonEventServiceAbilityExist() const
{
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sysMgr) {
        BATTERY_HILOGE(COMP_SVC,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sysMgr->CheckSystemAbility(COMMON_EVENT_SERVICE_ID);
    if (!remote) {
        BATTERY_HILOGE(COMP_SVC, "No CesServiceAbility");
        return false;
    }

    g_commonEventInitSuccess = true;
    return true;
}

bool BatteryNotify::PublishChangedEvent(const BatteryInfo& info) const
{
    Want want;
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_CAPACITY, info.GetCapacity());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_VOLTAGE, info.GetVoltage());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_TEMPERATURE, info.GetTemperature());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_HEALTH_STATE, static_cast<int32_t>(info.GetHealthState()));
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PLUGGED_TYPE, static_cast<int32_t>(info.GetPluggedType()));
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_CHARGE_STATE, static_cast<int32_t>(info.GetChargeState()));
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PRESENT, info.IsPresent());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_TECHNOLOGY, info.GetTechnology());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_UEVENT, info.GetUevent());

    auto capacityLevel = g_service->GetCapacityLevel();
    if (capacityLevel != g_lastCapacityLevel) {
        want.SetParam(BatteryInfo::COMMON_EVENT_KEY_CAPACITY_LEVEL, static_cast<int32_t>(capacityLevel));
        g_lastCapacityLevel = capacityLevel;
    }

    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    bool isSuccess = true;

    HiSysEventWrite(HiSysEvent::Domain::BATTERY, "CHANGED", HiSysEvent::EventType::STATISTIC,
        "LEVEL", info.GetCapacity(), "CHARGER", static_cast<int32_t>(info.GetPluggedType()),
        "VOLTAGE", info.GetVoltage(), "TEMPERATURE", info.GetTemperature(),
        "HEALTH", static_cast<int32_t>(info.GetHealthState()), "CURRENT", info.GetNowCurrent());
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "failed to publish BATTERY_CHANGED event");
    }
    return isSuccess;
}

bool BatteryNotify::PublishChangedEventInner(const BatteryInfo& info) const
{
    Want want;
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PLUGGED_MAX_CURRENT, info.GetPluggedMaxCurrent());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PLUGGED_MAX_VOLTAGE, info.GetPluggedMaxVoltage());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PLUGGED_NOW_CURRENT, info.GetNowCurrent());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_CHARGE_COUNTER, info.GetChargeCounter());

    want.SetAction(BatteryInfo::COMMON_EVENT_BATTERY_CHANGED_INNER);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);

    bool isSuccess = true;
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "failed to publish BATTERY_CHANGED_INNER event");
    }
    return isSuccess;
}

bool BatteryNotify::PublishLowEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if (info.GetCapacity() > lowCapacity_) {
        g_batteryLowOnce = false;
        return isSuccess;
    }

    if (g_batteryLowOnce) {
        return isSuccess;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_LOW);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(info.GetCapacity());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher capacity=%{public}d", info.GetCapacity());
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "failed to publish battery_low event");
    }
    g_batteryLowOnce = true;
    return isSuccess;
}

bool BatteryNotify::PublishOkayEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if (info.GetCapacity() <= lowCapacity_) {
        g_batteryOkOnce = false;
        return isSuccess;
    }

    if (g_batteryOkOnce) {
        return isSuccess;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_OKAY);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(info.GetCapacity());
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher capacity=%{public}d", info.GetCapacity());
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "failed to publish battery_okay event");
    }
    g_batteryOkOnce = true;
    return isSuccess;
}

bool BatteryNotify::PublishPowerConnectedEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if ((info.GetPluggedType() == BatteryPluggedType::PLUGGED_TYPE_NONE) ||
        (info.GetPluggedType() == BatteryPluggedType::PLUGGED_TYPE_BUTT)) {
        g_batteryConnectOnce = false;
        return isSuccess;
    }

    if (g_batteryConnectOnce) {
        return isSuccess;
    }
    StartVibrator();
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(static_cast<int32_t>(info.GetPluggedType()));
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher pluggedtype=%{public}u",
        static_cast<uint32_t>(info.GetPluggedType()));
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "failed to publish power_connected event");
    }

    g_batteryConnectOnce = true;
    return isSuccess;
}

void BatteryNotify::StartVibrator() const
{
    std::shared_ptr<PowerVibrator> vibrator = PowerVibrator::GetInstance();
    std::string scene = "start_charge";
    vibrator->StartVibrator(scene);
}

bool BatteryNotify::PublishPowerDisconnectedEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if ((info.GetPluggedType() != BatteryPluggedType::PLUGGED_TYPE_NONE) &&
        (info.GetPluggedType() != BatteryPluggedType::PLUGGED_TYPE_BUTT)) {
        g_batteryDisconnectOnce = false;
        return isSuccess;
    }

    if (g_batteryDisconnectOnce) {
        return isSuccess;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(static_cast<int32_t>(info.GetPluggedType()));
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher pluggedtype=%{public}u",
        static_cast<uint32_t>(info.GetPluggedType()));
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "failed to publish power_disconnected event");
    }

    g_batteryDisconnectOnce = true;
    return isSuccess;
}

bool BatteryNotify::PublishChargingEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if ((info.GetChargeState() != BatteryChargeState::CHARGE_STATE_ENABLE) &&
        (info.GetChargeState() != BatteryChargeState::CHARGE_STATE_FULL)) {
        g_batteryChargingOnce = false;
        return isSuccess;
    }

    if (g_batteryChargingOnce) {
        return isSuccess;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_CHARGING);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(static_cast<int32_t>(info.GetChargeState()));
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher chargeState=%{public}u",
        static_cast<uint32_t>(info.GetChargeState()));
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "failed to publish battery charing event");
    }

    g_batteryChargingOnce = true;
    return isSuccess;
}

bool BatteryNotify::PublishDischargingEvent(const BatteryInfo& info) const
{
    bool isSuccess = true;

    if ((info.GetChargeState() == BatteryChargeState::CHARGE_STATE_ENABLE) ||
        (info.GetChargeState() == BatteryChargeState::CHARGE_STATE_FULL)) {
        g_batteryDischargingOnce = false;
        return isSuccess;
    }

    if (g_batteryDischargingOnce) {
        return isSuccess;
    }

    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_DISCHARGING);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    data.SetCode(static_cast<int32_t>(info.GetChargeState()));
    BATTERY_HILOGD(FEATURE_BATT_INFO, "publisher chargeState=%{public}u",
        static_cast<uint32_t>(info.GetChargeState()));
    isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "failed to publish battery charing event");
    }

    g_batteryDischargingOnce = true;
    return isSuccess;
}
} // namespace PowerMgr
} // namespace OHOS

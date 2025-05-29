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

#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
#include "ffrt_utils.h"
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
#include "memory_guard.h"
#endif
#endif
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_publish_info.h"
#include "common_event_support.h"
#include "errors.h"
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
#include "hisysevent.h"
#endif
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include <hookmgr.h>
#include "battery_hookmgr.h"

#include "battery_config.h"
#include "battery_log.h"
#include "battery_service.h"
#include "power_vibrator.h"
#include "power_mgr_client.h"
#include <dlfcn.h>

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
using namespace OHOS::HiviewDFX;
#endif
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
const std::string SEND_CUSTOMEVENT = "sendcustomevent";
const std::string SEND_POPUP = "sendpopup";
const std::string BATTERY_CUSTOM_EVENT_PREFIX = "usual.event.";
sptr<BatteryService> g_service = DelayedSpSingleton<BatteryService>::GetInstance();

BatteryNotify::BatteryNotify()
{
    const int32_t DEFAULT_LOW_CAPACITY = 20;
    lowCapacity_ = BatteryConfig::GetInstance().GetInt("soc.low", DEFAULT_LOW_CAPACITY);
    BATTERY_HILOGI(COMP_SVC, "Low broadcast power=%{public}d", lowCapacity_);
}

int32_t BatteryNotify::PublishEvents(BatteryInfo& info)
{
    if (!g_commonEventInitSuccess) {
        if (!IsCommonEventServiceAbilityExist()) {
            return ERR_NO_INIT;
        }
    }
    if (info.GetUevent() != POWER_SUPPLY && info.GetUevent() != "" &&
        info.GetUevent() != INVALID_STRING_VALUE) {
        HandleUevent(info);
        return ERR_OK;
    }

    bool isAllSuccess = true;
    bool ret = PublishChangedEvent(info);
    isAllSuccess &= ret;
    ret = PublishChangedEventInner(info);
    isAllSuccess &= ret;
    std::lock_guard<std::mutex> lock(mutex_);
    ret = PublishLowEvent(info);
    isAllSuccess &= ret;
    ret = PublishOkayEvent(info);
    isAllSuccess &= ret;

    //hooks
#ifdef BATTERY_MANAGER_ENABLE_WIRELESS_CHARGE
    PublishEventContext context {.pluggedType = info.GetPluggedType(),
        .lastPowerPluggedType = lastPowerPluggedType_,
        .wirelessChargerEnable = BatteryConfig::GetInstance().GetWirelessChargerConf()};
    HookMgrExecute(GetBatteryHookMgr(), static_cast<int32_t>(BatteryHookStage::PUBLISH_EVENT), &context, nullptr);
#endif
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
    lastPowerPluggedType_ = info.GetPluggedType();
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
            const std::string reason = "POWEROFF_CHARGE_DISABLE";
            PowerMgrClient::GetInstance().ShutDownDevice(reason);
        } else if (ueventAct == REBOOT) {
            PowerMgrClient::GetInstance().RebootDevice(ueventName);
        } else if (ueventAct == SEND_COMMONEVENT) {
            info.SetUevent(ueventName);
            PublishChangedEvent(info);
        } else if (ueventAct.compare(0, BATTERY_CUSTOM_EVENT_PREFIX.size(), BATTERY_CUSTOM_EVENT_PREFIX) == 0) {
            info.SetUevent(ueventName);
            PublishCustomEvent(info, ueventAct);
        } else if (ueventAct == SEND_POPUP) {
            info.SetUevent(ueventName);
            PublishChangedEvent(info);
            HandleNotification(ueventName);
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

    if (!g_commonEventInitSuccess) {
        BATTERY_HILOGI(COMP_SVC, "common event service ability init success");
        g_commonEventInitSuccess = true;
    }

    return true;
}

bool BatteryNotify::PublishChangedEvent(const BatteryInfo& info)
{
    Want want;
    int32_t capacity = info.GetCapacity();
    int32_t pluggedType = static_cast<int32_t>(info.GetPluggedType());
    int32_t temperature = info.GetTemperature();
    int32_t healthState = static_cast<int32_t>(info.GetHealthState());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_CAPACITY, capacity);
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_VOLTAGE, info.GetVoltage());
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_TEMPERATURE, temperature);
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_HEALTH_STATE, healthState);
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_PLUGGED_TYPE, pluggedType);
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
    if (capacity != lastCapacity_ || pluggedType != lastPluggedType_ ||
        temperature != lastTemperature_ || healthState != lastHealthState_) {
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
        HiSysEventWrite(HiSysEvent::Domain::BATTERY, "CHANGED", HiSysEvent::EventType::STATISTIC,
            "LEVEL", capacity, "CHARGER", pluggedType, "VOLTAGE", info.GetVoltage(),
            "TEMPERATURE", temperature, "HEALTH", healthState, "CURRENT", info.GetNowCurrent(),
            "CHARGE_MODE", static_cast<int32_t>(info.GetChargeType()));
#endif
        lastCapacity_ = capacity;
        lastPluggedType_ = pluggedType;
        lastTemperature_ = temperature;
        lastHealthState_ = healthState;
    }
    bool isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
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
    const std::vector<std::string> permissionVec { "ohos.permission.POWER_OPTIMIZATION" };
    publishInfo.SetSubscriberPermissions(permissionVec);

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

#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
namespace {
std::atomic<bool> g_stopping = false;
std::atomic<bool> g_released = true;
constexpr int MAX_PLAY_TIME_MS = 2000;
constexpr int TICK_INTERVAL_MS = 100;
constexpr int US_PER_MS = 1000;

// be careful: most of the shared libraries in ohos do not support dynamic (un-)loading.
// for now, if the current process does not hav certain libs as dependency (thus they won't be unloaded),
// re-dlopen after dlclosing libmedia_client.z.so causes crashes (use of released symbols somehow).
// known libraries that should not be unloaded:  configpolicy_util, image_framework.
void AntiMemLeak()
{
    // this indirectly opened library causes mem leak, do not reopen it.
    // check global variables in third_party/libphonenumber/cpp/src/phonenumbers/ohos/update_metadata.cc
    // if there are any further libraries causing mem-leaks or crashes, add them here.
    void* tmpHandle = dlopen("libphonenumber_standard.z.so", RTLD_LAZY | RTLD_NODELETE);
    if (!tmpHandle) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "dlopen libphonenumber_standard.z.so failed");
    } else {
        dlclose(tmpHandle);
    }
}

void StartChargingSoundFunc()
{
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
    OHOS::PowerMgr::MemoryGuard guard;
#endif
    if (!g_service || !g_service->IsBootCompleted()) {
        return;
    }
    bool expected = true;
    bool ret = g_released.compare_exchange_strong(expected, false);
    if (!ret) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "charging sound not released");
        return;
    }
    AntiMemLeak();
    void* handle = dlopen("libcharging_sound.z.so", RTLD_LAZY);
    if (!handle) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "dlopen libcharging_sound failed: ret=%{public}s", dlerror());
        g_released.store(true);
        return;
    }
    auto ChargingSoundStart = reinterpret_cast<bool(*)(void)>(dlsym(handle, "ChargingSoundStart"));
    auto IsPlaying = reinterpret_cast<bool (*)(void)>(dlsym(handle, "IsPlaying"));
    if (!IsPlaying || !ChargingSoundStart) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "dlsym failed");
        dlclose(handle);
        g_released.store(true);
        return;
    }
    g_stopping.store(false);
    ffrt::submit([ChargingSoundStart, IsPlaying, handle]() {
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
        OHOS::PowerMgr::MemoryGuard guard;
#endif
        ChargingSoundStart();
        for (int timePassed = 0; timePassed < MAX_PLAY_TIME_MS; timePassed += TICK_INTERVAL_MS) {
            if (g_stopping.load()) {
                break;
            }
            if (!IsPlaying()) {
                break;
            }
            usleep(TICK_INTERVAL_MS * US_PER_MS);
        }
        dlclose(handle);
        g_released.store(true);
    });
}

}
#endif

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
#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
    StartChargingSoundFunc();
#endif
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
#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
    g_stopping.store(true);
#endif
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

bool BatteryNotify::PublishCustomEvent(const BatteryInfo& info, const std::string& commonEventName) const
{
    UEVENT_CHECK_INFO ueventCheckInfo = {
        .UeventName = info.GetUevent(),
        .checkResult = true
    };
    int ret = HookMgrExecute(GetBatteryHookMgr(), static_cast<int32_t>(BatteryHookStage::BATTERY_UEVENT_CHECK),
        (void*)&ueventCheckInfo, nullptr);
    if (ret == 0 && !ueventCheckInfo.checkResult) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "PublishCustomEvent fail, uevent=%{public}s, checkResult=%{public}d",
            ueventCheckInfo.UeventName.c_str(), ueventCheckInfo.checkResult);
        return false;
    }
    Want want;
    want.SetParam(BatteryInfo::COMMON_EVENT_KEY_UEVENT, info.GetUevent());
    want.SetAction(commonEventName);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(false);
    const std::vector<std::string> permissionVec { "ohos.permission.POWER_OPTIMIZATION" };
    publishInfo.SetSubscriberPermissions(permissionVec);

    bool isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "failed to publish battery custom event");
    }
    return isSuccess;
}

bool BatteryNotify::HandleNotification(const std::string& ueventName) const
{
#ifdef BATTERY_SUPPORT_NOTIFICATION
    std::unordered_map<std::string, std::vector<BatteryConfig::PopupConf>> popupCfg =
        BatteryConfig::GetInstance().GetPopupConf();
    auto iter = popupCfg.find(ueventName);
    if (iter == popupCfg.end()) {
        BATTERY_HILOGW(COMP_SVC, "HandleNotification not found conf: %{public}s", ueventName.c_str());
        return false;
    }
    typedef void(*HandleNotificationFunc)(const std::string&, int32_t,
        const std::unordered_map<std::string, BatteryConfig::NotificationConf>&);
    void* handler = dlopen("libbattery_notification.z.so", RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "dlopen HandleNotificationFunc failed, reason : %{public}s", dlerror());
        return false;
    }
    HandleNotificationFunc HandleNotification =
        reinterpret_cast<HandleNotificationFunc>(dlsym(handler, "HandleNotification"));
    if (HandleNotification == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "HandleNotificationFunc is null, reason : %{public}s", dlerror());
        dlclose(handler);
        handler = nullptr;
        return false;
    }
    auto nConfMap = BatteryConfig::GetInstance().GetNotificationConf();
    for (auto& item : iter->second) {
        HandleNotification(item.name, item.action, nConfMap);
        BATTERY_HILOGI(COMP_SVC, "popupName=%{public}s, popupAction=%{public}d", item.name.c_str(), item.action);
    }
    dlclose(handler);
    handler = nullptr;
#endif
    return true;
}

} // namespace PowerMgr
} // namespace OHOS

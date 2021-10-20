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

#include "battery_service_subscriber.h"

#include <typeinfo>
#include "ohos/aafwk/content/want.h"
#include "power_common.h"
#include "string_ex.h"
#include "batteryd_api.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace PowerMgr {
bool g_firstPublish = true;
std::string g_technology;
BatterydInfo g_batteryInfo;

BatteryServiceSubscriber::BatteryServiceSubscriber() {}

int32_t BatteryServiceSubscriber::Update(const BatteryInfo &info)
{
    bool isAllSuccess = true;
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryInfo: capacity=%{public}d, voltage=%{public}d, " \
                                      "temperature=%{public}d, healthState=%{public}d, pluggedType=%{public}d, " \
                                      "pluggedMaxCurrent=%{public}d, pluggedMaxVoltage=%{public}d, " \
                                      "chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
                                      "technology=%{public}s",
        info.GetCapacity(), info.GetVoltage(), info.GetTemperature(), info.GetHealthState(),
        info.GetPluggedType(), info.GetPluggedMaxCurrent(), info.GetPluggedMaxVoltage(), info.GetChargeState(),
        info.GetChargeCounter(), info.IsPresent(), info.GetTechnology().c_str());
    bool ret = HandleTechnologyChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleCapacityChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleVoltageChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleTemperatureChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleHealthStateChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandlePluggedTypeChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleMaxCurrentChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleMaxVoltageChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleChargeStateChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandleChargeCounterChangedEvent(info);
    isAllSuccess &= ret;
    ret = HandlePresentChangedEvent(info);
    g_firstPublish = false;

    return isAllSuccess ? ERR_OK : ERR_NO_INIT;
}

bool BatteryServiceSubscriber::HandleCapacityChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher before tostring, capacity=%{public}d", info.GetCapacity());
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CAPACITY);
    data.SetData(ToString(info.GetCapacity()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher capacity=%{public}d", info.GetCapacity());
    if ((g_firstPublish == true) || (g_batteryInfo.capacity_ != info.GetCapacity())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.capacity_ = info.GetCapacity();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish CAPACITY_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleVoltageChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher before tostring, voltage=%{public}d", info.GetVoltage());
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_VOLTAGE);
    data.SetData(ToString(info.GetVoltage()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher voltage=%{public}d", info.GetVoltage());
    if ((g_firstPublish == true) || (g_batteryInfo.voltage_ != info.GetVoltage())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.voltage_ = info.GetVoltage();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish VOLTAGE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleTemperatureChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_TEMPERATURE);
    data.SetData(ToString(info.GetTemperature()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher temperature=%{public}d", info.GetTemperature());
    if ((g_firstPublish == true) || (g_batteryInfo.temperature_ != info.GetTemperature())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.temperature_ = info.GetTemperature();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish TEMPERATURE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleHealthStateChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_HEALTH_STATE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetHealthState())));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher healthstate=%{public}d",
        static_cast<uint32_t>(info.GetHealthState()));
    if ((g_firstPublish == true) || (g_batteryInfo.healthState_ != static_cast<int32_t>(info.GetHealthState()))) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.healthState_ = static_cast<uint32_t>(info.GetHealthState());
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish HEALTH_STATE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandlePluggedTypeChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_TYPE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetPluggedType())));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher pluggedtype=%{public}d",
        static_cast<uint32_t>(info.GetPluggedType()));
    if ((g_firstPublish == true) || (g_batteryInfo.pluggedType_ != static_cast<int32_t>(info.GetPluggedType()))) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.pluggedType_ = static_cast<uint32_t>(info.GetPluggedType());
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish PLUGGED_TYPE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleMaxCurrentChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_CURRENT);
    data.SetData(ToString(info.GetPluggedMaxCurrent()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher maxcurrent=%{public}d", info.GetPluggedMaxCurrent());
    if ((g_firstPublish == true) || (g_batteryInfo.pluggedMaxCurrent_ != info.GetPluggedMaxCurrent())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.pluggedMaxCurrent_ = info.GetPluggedMaxCurrent();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish MAX_CURRENT_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleMaxVoltageChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_VOLTAGE);
    data.SetData(ToString(info.GetPluggedMaxVoltage()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher maxvoltage=%{public}d", info.GetPluggedMaxVoltage());
    if ((g_firstPublish == true) || (g_batteryInfo.pluggedMaxVoltage_ != info.GetPluggedMaxVoltage())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.pluggedMaxVoltage_ = info.GetPluggedMaxVoltage();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish MAX_VOLTAGE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleChargeStateChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CHARGE_STATE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetChargeState())));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher chargestate=%{public}d",
        static_cast<uint32_t>(info.GetChargeState()));
    if ((g_firstPublish == true) || (g_batteryInfo.chargeState_ != static_cast<int32_t>(info.GetChargeState()))) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.chargeState_ = static_cast<int32_t>(info.GetChargeState());
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish CHARGE_STATE_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleChargeCounterChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CHARGE_COUNTER);
    data.SetData(ToString(info.GetChargeCounter()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher chargecounter=%{public}d", info.GetChargeCounter());
    if ((g_firstPublish == true) || (g_batteryInfo.chargeCounter_ != info.GetChargeCounter())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.chargeCounter_ = info.GetChargeCounter();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish CHARGE_COUNTER_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandlePresentChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PRESENT);
    data.SetData(ToString(info.IsPresent()));
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher ispresent=%{public}d", info.IsPresent());
    if ((g_firstPublish == true) || (g_batteryInfo.present_ != info.IsPresent())) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
        g_batteryInfo.present_ = info.IsPresent();
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish PRESENT_CHANGED event");
    }
    return isSuccess;
}

bool BatteryServiceSubscriber::HandleTechnologyChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = true;

    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_TECHNOLOGY);
    data.SetData(info.GetTechnology());
    POWER_HILOGD(MODULE_BATT_SERVICE, "publisher technology=%{public}s", info.GetTechnology().c_str());
    if (g_firstPublish == true) {
        isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    }

    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish TECHNOLOGY_CHANGED event");
    }
    return isSuccess;
}
} // namespace PowerMgr
} // namespace OHOS

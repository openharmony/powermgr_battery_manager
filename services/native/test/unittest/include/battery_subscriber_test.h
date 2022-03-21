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

#ifndef BATTERY_SUBSCRIBER_TEST_H
#define BATTERY_SUBSCRIBER_TEST_H

#include <gtest/gtest.h>
#include <ipc_skeleton.h>

#include "battery_service.h"
#include "battery_log.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "string_ex.h"
#include "sys_param.h"

namespace OHOS {
namespace PowerMgr {
class BatterySubscriberTest : public testing::Test {
};

const std::string KEY_CAPACITY = ToString(BatteryInfo::COMMON_EVENT_CODE_CAPACITY);
const std::string KEY_VOLTAGE = ToString(BatteryInfo::COMMON_EVENT_CODE_VOLTAGE);
const std::string KEY_TEMPERATURE = ToString(BatteryInfo::COMMON_EVENT_CODE_TEMPERATURE);
const std::string KEY_HEALTH_STATE = ToString(BatteryInfo::COMMON_EVENT_CODE_HEALTH_STATE);
const std::string KEY_PLUGGED_TYPE = ToString(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_TYPE);
const std::string KEY_PLUGGED_MAX_CURRENT = ToString(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_CURRENT);
const std::string KEY_PLUGGED_MAX_VOLTAGE = ToString(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_VOLTAGE);
const std::string KEY_CHARGE_STATE = ToString(BatteryInfo::COMMON_EVENT_CODE_CHARGE_STATE);
const std::string KEY_CHARGE_COUNTER = ToString(BatteryInfo::COMMON_EVENT_CODE_CHARGE_COUNTER);
const std::string KEY_PRESENT = ToString(BatteryInfo::COMMON_EVENT_CODE_PRESENT);
const std::string KEY_TECHNOLOGY = ToString(BatteryInfo::COMMON_EVENT_CODE_TECHNOLOGY);

class SubscriberTest : public EventFwk::CommonEventSubscriber {
public:
    explicit SubscriberTest(const EventFwk::CommonEventSubscribeInfo& sp) : EventFwk::CommonEventSubscriber(sp)
    {}

    void OnReceiveEvent(const EventFwk::CommonEventData& data)
    {
        using namespace OHOS::HDI::Battery;
        BATTERY_HILOGD(LABEL_TEST, "OnReceiveEvent enter.");
        std::string action = data.GetWant().GetAction();
        BATTERY_HILOGD(LABEL_TEST, "BatteryService=== start. action=%{public}s", action.c_str());
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED) {
            int defaultCapacity = -1;
            int capacity = data.GetWant().GetIntParam(KEY_CAPACITY, defaultCapacity);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest capacity = %{public}d", capacity);

            int defaultVoltage = -1;
            int voltage = data.GetWant().GetIntParam(KEY_VOLTAGE, defaultVoltage);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest voltage = %{public}d", voltage);

            int defaultTemperature = -1;
            int temperature = data.GetWant().GetIntParam(KEY_TEMPERATURE, defaultTemperature);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest temperature = %{public}d", temperature);

            int defaultHealthState = -1;
            int healthState = data.GetWant().GetIntParam(KEY_HEALTH_STATE, defaultHealthState);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest healthState = %{public}d", healthState);

            int defaultPluggedType = -1;
            int pluggedType = data.GetWant().GetIntParam(KEY_PLUGGED_TYPE, defaultPluggedType);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest pluggedType = %{public}d", pluggedType);

            int defaultMaxCurrent = -1;
            int maxCurrent = data.GetWant().GetIntParam(KEY_PLUGGED_MAX_CURRENT, defaultMaxCurrent);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest maxCurrent = %{public}d", maxCurrent);

            int defaultMaxVoltage = -1;
            int maxVoltage = data.GetWant().GetIntParam(KEY_PLUGGED_MAX_VOLTAGE, defaultMaxVoltage);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest maxVoltage = %{public}d", maxVoltage);

            int defaultChargeState = -1;
            int chargeState = data.GetWant().GetIntParam(KEY_CHARGE_STATE, defaultChargeState);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest chargeState = %{public}d", chargeState);

            int defaultChargeCounter = -1;
            int chargeCounter = data.GetWant().GetIntParam(KEY_CHARGE_COUNTER, defaultChargeCounter);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest chargeCounter = %{public}d", chargeCounter);

            bool defaultPresent = false;
            bool isPresent = data.GetWant().GetBoolParam(KEY_PRESENT, defaultPresent);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest isPresent = %{public}d", isPresent);

            std::string defaultTechnology = "";
            std::string technology = data.GetWant().GetStringParam(KEY_TECHNOLOGY);
            BATTERY_HILOGD(LABEL_TEST, "SubscriberTest technology = %{public}s", technology.c_str());
        }
    }
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SUBSCRIBER_TEST_H

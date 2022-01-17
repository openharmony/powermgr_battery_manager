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
#include "power_common.h"
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

class SubscriberTest : public EventFwk::CommonEventSubscriber {
public:
    explicit SubscriberTest(const EventFwk::CommonEventSubscribeInfo& sp) : EventFwk::CommonEventSubscriber(sp)
    {}
    void OnReceiveEventOther(const EventFwk::CommonEventData& data)
    {
        POWER_HILOGD(MODULE_BATT_SERVICE, "OnReceiveEventOther enter.");
        std::string action = data.GetWant().GetAction();
        POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService=== start. action=%{public}s", action.c_str());
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED) {
            switch (data.GetCode()) {
                case BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_CURRENT: {
                    std::string maxcurrent = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "maxcurrent=%{public}s", maxcurrent.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_VOLTAGE: {
                    std::string maxvoltage = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "maxvoltage=%{public}s", maxvoltage.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_CHARGE_STATE: {
                    std::string chargestate = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "chargestate=%{public}s", chargestate.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_CHARGE_COUNTER: {
                    std::string chargecounter = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "chargecounter=%{public}s", chargecounter.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_PRESENT: {
                    std::string present = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "present=%{public}s", present.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_TECHNOLOGY: {
                    std::string technology = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "technology=%{public}s", technology.c_str());
                    break;
                }
                default: {
                    POWER_HILOGD(MODULE_BATT_SERVICE, "default case enter");
                    break;
                }
            }
        }
    }
    void OnReceiveEvent(const EventFwk::CommonEventData& data)
    {
        POWER_HILOGD(MODULE_BATT_SERVICE, "OnReceiveEvent enter.");
        std::string action = data.GetWant().GetAction();
        POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService=== start. action=%{public}s", action.c_str());
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED) {
            switch (data.GetCode()) {
                case BatteryInfo::COMMON_EVENT_CODE_CAPACITY: {
                    std::string capacity = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "capacity=%{public}s", capacity.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_VOLTAGE: {
                    std::string voltage = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "voltage=%{public}s", voltage.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_TEMPERATURE: {
                    std::string temperature = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "temperature=%{public}s", temperature.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_HEALTH_STATE: {
                    std::string healthstate = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "healthstate=%{public}s", healthstate.c_str());
                    break;
                }
                case BatteryInfo::COMMON_EVENT_CODE_PLUGGED_TYPE: {
                    std::string pluggedtype = data.GetData();
                    POWER_HILOGD(MODULE_BATT_SERVICE, "pluggedtype=%{public}s", pluggedtype.c_str());
                    break;
                }
                default: {
                    POWER_HILOGD(MODULE_BATT_SERVICE, "default case enter");
                    break;
                }
            }
        }
    }
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SUBSCRIBER_TEST_H

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

#ifndef BATTERY_SERVICE_SUBSCRIBER_H
#define BATTERY_SERVICE_SUBSCRIBER_H

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "ipc_object_stub.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryServiceSubscriber : public IPCObjectStub {
public:
    static int32_t Update(const BatteryInfo& info);

private:
    static bool HandleBatteryChangedEvent(const BatteryInfo& info);
    static bool CmpBatteryInfo(const BatteryInfo& info);
    static void SwaptBatteryInfo(const BatteryInfo& info);
    static bool HandleBatteryLowEvent(const BatteryInfo& info);
    static bool HandleBatteryOkayEvent(const BatteryInfo& info);
    static bool HandleBatteryPowerConnectedEvent(const BatteryInfo& info);
    static bool HandleBatteryPowerDisconnectedEvent(const BatteryInfo& info);
    static bool HandleBatteryChargingEvent(const BatteryInfo& info);
    static bool HandleBatteryDischargingEvent(const BatteryInfo& info);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SERVICE_SUBSCRIBER_H

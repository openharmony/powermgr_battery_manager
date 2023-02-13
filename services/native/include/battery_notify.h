/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <cstdint>
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "ipc_object_stub.h"
#include "battery_config.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryNotify {
public:
    BatteryNotify();
    ~BatteryNotify() = default;
    int32_t PublishEvents(const BatteryInfo& info);

private:
    void ChangedEventDeprecated(OHOS::AAFwk::Want& want, const BatteryInfo& info) const;
    bool PublishChangedEvent(const BatteryInfo& info) const;
    bool PublishLowEvent(const BatteryInfo& info) const;
    bool PublishOkayEvent(const BatteryInfo& info) const;
    bool PublishPowerConnectedEvent(const BatteryInfo& info) const;
    bool PublishPowerDisconnectedEvent(const BatteryInfo& info) const;
    bool PublishChargingEvent(const BatteryInfo& info) const;
    bool PublishDischargingEvent(const BatteryInfo& info) const;
    bool IsCommonEventServiceAbilityExist() const;

    int32_t lowCapacity_ = -1;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SERVICE_SUBSCRIBER_H

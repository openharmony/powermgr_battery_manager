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

#ifndef BATTERY_SERVICE_SUBSCRIBER_H
#define BATTERY_SERVICE_SUBSCRIBER_H

#include <cstdint>
#include <mutex>
#include "want.h"

#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryNotify {
public:
    BatteryNotify();
    ~BatteryNotify() = default;
    int32_t PublishEvents(BatteryInfo& info);

private:
    void HandleUevent(BatteryInfo& info);
    bool PublishChangedEvent(const BatteryInfo& info);
    bool PublishChangedEventInner(const BatteryInfo& info) const;
    bool PublishLowEvent(const BatteryInfo& info) const;
    bool PublishOkayEvent(const BatteryInfo& info) const;
    void StartVibrator() const;
    bool PublishPowerConnectedEvent(const BatteryInfo& info) const;
    bool PublishPowerDisconnectedEvent(const BatteryInfo& info) const;
    bool PublishChargingEvent(const BatteryInfo& info) const;
    bool PublishDischargingEvent(const BatteryInfo& info) const;
    bool PublishChargeTypeChangedEvent(const BatteryInfo& info);
    bool IsCommonEventServiceAbilityExist() const;
    bool PublishCustomEvent(const BatteryInfo& info, const std::string& commonEventName) const;
    void WirelessPluggedConnected(const BatteryInfo& info) const;
    void WirelessPluggedDisconnected(const BatteryInfo& info) const;
    void RotationMotionSubscriber() const;
    void RotationMotionUnsubscriber() const;

    int32_t lowCapacity_ = -1;
    ChargeType batteryInfoChargeType_ = ChargeType::NONE;
    int32_t lastCapacity_ = -1;
    int32_t lastPluggedType_ = -1;
    int32_t lastTemperature_ = -1;
    int32_t lastHealthState_ = -1;
    BatteryPluggedType lastPowerPluggedType_ = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    std::mutex mutex_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SERVICE_SUBSCRIBER_H

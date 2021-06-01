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

#include "batteryd_subscriber.h"

namespace OHOS {
namespace PowerMgr {
class BatteryServiceSubscriber : public BatterydSubscriber {
public:
    BatteryServiceSubscriber();
    ~BatteryServiceSubscriber() = default;
    int32_t Update(const BatteryInfo &info) override;

private:
    static bool HandleBatteryChangedEvent(const BatteryInfo &info);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SERVICE_SUBSCRIBER_H
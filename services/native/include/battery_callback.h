/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_BATTERY_MANAGER_BATTERY_CALLBACK_H
#define POWERMGR_BATTERY_MANAGER_BATTERY_CALLBACK_H

#include <functional>
#include <cstdint>
#include "v1_1/types.h"
#include "v1_1/ibattery_callback.h"

namespace OHOS {
namespace PowerMgr {
class BatteryCallback : public HDI::Battery::V1_1::IBatteryCallback {
public:
    BatteryCallback() {}
    virtual ~BatteryCallback() {}
    using BatteryEventCallback = std::function<int32_t(const HDI::Battery::V1_1::BatteryInfo& event)>;
    static int32_t RegisterBatteryEvent(const BatteryEventCallback& eventCb);
    int32_t Update(const HDI::Battery::V1_1::BatteryInfo& event) override;
private:
    static BatteryEventCallback eventCb_;
};
} // OHOS
} // PowerMgr
#endif // POWERMGR_BATTERY_MANAGER_BATTERY_CALLBACK_H

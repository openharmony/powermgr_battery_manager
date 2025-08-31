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

#ifndef POWERMGR_BATTERY_MANAGER_BATTERY_LIGHT_H
#define POWERMGR_BATTERY_MANAGER_BATTERY_LIGHT_H

#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryLight {
public:
    BatteryLight() = default;
    ~BatteryLight() = default;

    void InitLight();
    void TurnOff();
    void TurnOn(uint32_t color = 0);
    bool UpdateColor(BatteryChargeState chargeState, int32_t capacity);
    bool isAvailable() const;
    uint32_t GetLightColor() const;

private:
    bool available_ {false};
    int32_t lightId_ {-1};
    uint32_t lightColor_ {0};
};
} // namespace PowerMgr
} // namespace OHOS
#endif // POWERMGR_BATTERY_MANAGER_BATTERY_LIGHT_H

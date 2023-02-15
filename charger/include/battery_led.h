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

#ifndef BATTERY_LED_H
#define BATTERY_LED_H

#include "battery_config.h"
#include "power_supply_provider.h"
#include "v1_0/ilight_interface.h"
#include "v1_0/light_types.h"

namespace OHOS {
namespace PowerMgr {
class BatteryLed {
public:
    BatteryLed() = default;
    ~BatteryLed() = default;

    void InitLight();
    void TurnOff();
    void TurnOn(uint32_t color = 0);
    bool UpdateColor(int32_t chargeState, int32_t capacity);
    bool IsAvailable() const;
    uint32_t GetLightColor() const;

private:
    sptr<OHOS::HDI::Light::V1_0::ILightInterface> batteryLight_ {nullptr};
    bool available_ {false};
    uint32_t lightColor_ {0};
};
} // namespace PowerMgr
} // namespace OHOS
#endif

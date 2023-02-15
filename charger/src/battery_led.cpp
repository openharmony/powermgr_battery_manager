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

#include "battery_led.h"
#include "battery_log.h"

using namespace OHOS::HDI::Light::V1_0;
using namespace std;

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_1 {
namespace {
constexpr uint32_t MOVE_RIGHT_16 = 16;
constexpr uint32_t MOVE_RIGHT_8 = 8;
} // namespace
void BatteryLed::InitLight()
{
    batteryLight_ = ILightInterface::Get();
    if (batteryLight_ == nullptr) {
        BATTERY_HILOGW(COMP_HDI, "Light interface is null");
        return;
    }

    vector<HdfLightInfo> lightInfo;
    if (batteryLight_->GetLightInfo(lightInfo) < HDF_SUCCESS) {
        BATTERY_HILOGW(COMP_HDI, "Get battert light failed");
        return;
    }

    available_ = std::any_of(lightInfo.begin(), lightInfo.end(), [](const auto &item) {
        return item.lightId == HdfLightId::HDF_LIGHT_ID_BATTERY;
    });
    BATTERY_HILOGI(COMP_HDI, "Battery light is available: %{public}d", available_);
}

void BatteryLed::TurnOff()
{
    if (!available_) {
        return;
    }
    int32_t ret = batteryLight_->TurnOffLight(HdfLightId::HDF_LIGHT_ID_BATTERY);
    if (ret < HDF_SUCCESS) {
        BATTERY_HILOGW(COMP_HDI, "Failed to turn off the battery light");
    }
    lightColor_ = (ret < HDF_SUCCESS) ? lightColor_ : 0;
}

void BatteryLed::TurnOn(uint32_t color)
{
    if (!available_) {
        return;
    }
    struct HdfLightEffect effect = {
        .lightColor.colorValue.rgbColor.r = (color >> MOVE_RIGHT_16) & 0xFF,
        .lightColor.colorValue.rgbColor.g = (color >> MOVE_RIGHT_8) & 0xFF,
        .lightColor.colorValue.rgbColor.b = color & 0xFF,
    };
    BATTERY_HILOGD(COMP_HDI, "battery light color is %{public}d", color);
    int32_t ret = batteryLight_->TurnOnLight(HdfLightId::HDF_LIGHT_ID_BATTERY, effect);
    if (ret < HDF_SUCCESS) {
        BATTERY_HILOGW(COMP_HDI, "Failed to turn on the battery light");
    }
    lightColor_ = (ret < HDF_SUCCESS) ? lightColor_ : color;
}

bool BatteryLed::UpdateColor(int32_t chargeState, int32_t capacity)
{
    if ((chargeState == static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_NONE)) ||
        (chargeState == static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_RESERVED)) || !available_) {
        BATTERY_HILOGD(COMP_HDI, "not in charging state, turn off battery light");
        TurnOff();
        return false;
    }

    const auto& lightConf = BatteryConfig::GetInstance().GetLightConf();
    for (const auto& it : lightConf) {
        if ((capacity >= it.beginSoc) && (capacity <= it.endSoc)) {
            if (lightColor_ == it.rgb) {
                return true;
            }
            TurnOff();
            TurnOn(it.rgb);
            return true;
        }
    }
    return false;
}

bool BatteryLed::IsAvailable() const
{
    return available_;
}

uint32_t BatteryLed::GetLightColor() const
{
    return lightColor_;
}
} // namespace V1_1
} // namespace Battery
} // namespace HDI
} // namespace OHOS

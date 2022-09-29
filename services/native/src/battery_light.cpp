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

#include <vector>

#include <errors.h>

#include "battery_config.h"
#include "battery_light.h"
#include "battery_log.h"
#include "power_common.h"

using namespace OHOS::HDI::Light::V1_0;
using namespace std;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr uint32_t MOVE_RIGHT_16 = 16;
constexpr uint32_t MOVE_RIGHT_8 = 8;
}
void BatteryLight::InitLight()
{
    batteryLight_ = ILightInterface::Get();
    if (batteryLight_ == nullptr) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Light interface is null");
        return;
    }

    vector<HdfLightInfo> lightInfo;
    if (batteryLight_->GetLightInfo(lightInfo) < ERR_OK) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Get battert light failed");
        return;
    }

    for (const auto& item : lightInfo) {
        if (item.lightId == HdfLightId::HDF_LIGHT_ID_BATTERY) {
            available_ = true;
            BATTERY_HILOGI(FEATURE_BATT_LIGHT, "Battery light is available");
            break;
        }
    }
}

void BatteryLight::TurnOff()
{
    RETURN_IF(!available_);
    int32_t ret = batteryLight_->TurnOffLight(HdfLightId::HDF_LIGHT_ID_BATTERY);
    if (ret < ERR_OK) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Failed to turn off the battery light");
    }
    lightColor_ = (ret < ERR_OK) ? lightColor_ : 0;
}

void BatteryLight::TurnOn(uint32_t color, int32_t brightness)
{
    RETURN_IF(!available_);
    struct HdfLightEffect effect = {
        .lightColor.colorValue.rgbColor.brightness = brightness,
        .lightColor.colorValue.rgbColor.r = (color >> MOVE_RIGHT_16) & 0xFF,
        .lightColor.colorValue.rgbColor.g = (color >> MOVE_RIGHT_8) & 0xFF,
        .lightColor.colorValue.rgbColor.b = color & 0xFF,
    };
    BATTERY_HILOGD(FEATURE_BATT_LIGHT, "battery light color is %{public}d", color);
    int32_t ret = batteryLight_->TurnOnLight(HdfLightId::HDF_LIGHT_ID_BATTERY, effect);
    if (ret < ERR_OK) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Failed to turn on the battery light");
    }
    lightColor_ = (ret < ERR_OK) ? lightColor_ : color;
}

bool BatteryLight::UpdateColor(BatteryChargeState chargeState, int32_t capacity)
{
    if ((chargeState == BatteryChargeState::CHARGE_STATE_NONE) ||
        (chargeState == BatteryChargeState::CHARGE_STATE_BUTT)) {
        BATTERY_HILOGD(FEATURE_BATT_LIGHT, "not in charging state, turn off battery light");
        TurnOff();
        return false;
    }

    RETURN_IF_WITH_RET(!available_, false);
    const auto& lightConf = BatteryConfig::GetInstance().GetLightConf();
    for (const auto& it : lightConf) {
        if ((capacity >= it.beginSoc) && (capacity <= it.endSoc)) {
            RETURN_IF_WITH_RET(lightColor_ == it.rgb, true);
            TurnOff();
            TurnOn(it.rgb);
            return true;
        }
    }
    return false;
}

bool BatteryLight::isAvailable() const
{
    return available_;
}

uint32_t BatteryLight::GetLightColor() const
{
    return lightColor_;
}
} // namespace PowerMgr
} // namespace OHOS

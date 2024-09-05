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
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
#include "light_agent.h"
#include "light_agent_type.h"
#endif
using namespace std;

namespace OHOS {
namespace PowerMgr {
#ifdef HAS_SENSORS_MISCDEVICE_PART
namespace {
constexpr uint32_t MOVE_RIGHT_16 = 16;
constexpr uint32_t MOVE_RIGHT_8 = 8;
}
#endif
void BatteryLight::InitLight()
{
#ifdef HAS_SENSORS_MISCDEVICE_PART
    LightInfo* lightInfo = nullptr;
    int32_t count = 0;
    int32_t ret = OHOS::Sensors::GetLightList(&lightInfo, count);
    if (ret < ERR_OK || lightInfo == nullptr || count <= 0) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Light info is null");
        return;
    }

    for (int32_t i = 0; i < count; ++i) {
        BATTERY_HILOGD(FEATURE_BATT_LIGHT,
            "LightInfo name: %{public}s, id: %{public}d, number: %{public}d, type: %{public}d", lightInfo[i].lightName,
            lightInfo[i].lightId, lightInfo[i].lightNumber, lightInfo[i].lightType);
        // lightName is associated by the light hdi driver
        if (std::string(lightInfo[i].lightName) == "battery") {
            available_ = true;
            lightId_ = lightInfo[i].lightId;
            BATTERY_HILOGI(FEATURE_BATT_LIGHT, "Battery light is available");
            break;
        }
    }
#endif
}

void BatteryLight::TurnOff()
{
#ifdef HAS_SENSORS_MISCDEVICE_PART
    RETURN_IF(!available_);
    int32_t ret = OHOS::Sensors::TurnOff(lightId_);
    if (ret < ERR_OK) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Failed to turn off the battery light");
    }
    lightColor_ = (ret < ERR_OK) ? lightColor_ : 0;
#endif
}

void BatteryLight::TurnOn(uint32_t color)
{
#ifdef HAS_SENSORS_MISCDEVICE_PART
    RETURN_IF(!available_);
    union LightColor lightColor;
    lightColor.rgbColor.r = (color >> MOVE_RIGHT_16) & 0xFF;
    lightColor.rgbColor.g = (color >> MOVE_RIGHT_8) & 0xFF;
    lightColor.rgbColor.b = color & 0xFF;
    LightAnimation animation = {
        .mode = FlashMode::LIGHT_MODE_DEFAULT
    };
    BATTERY_HILOGD(FEATURE_BATT_LIGHT, "battery light color is %{public}u", color);
    int32_t ret = OHOS::Sensors::TurnOn(lightId_, lightColor, animation);
    if (ret < ERR_OK) {
        BATTERY_HILOGW(FEATURE_BATT_LIGHT, "Failed to turn on the battery light");
    }
    lightColor_ = (ret < ERR_OK) ? lightColor_ : color;
#endif
}

bool BatteryLight::UpdateColor(BatteryChargeState chargeState, int32_t capacity)
{
    if ((chargeState == BatteryChargeState::CHARGE_STATE_NONE) ||
        (chargeState == BatteryChargeState::CHARGE_STATE_BUTT)) {
        BATTERY_HILOGD(FEATURE_BATT_LIGHT, "not in charging state, turn off battery light");
#ifdef HAS_SENSORS_MISCDEVICE_PART
        TurnOff();
#endif
        return false;
    }

    RETURN_IF_WITH_RET(!available_, false);
    const auto& lightConf = BatteryConfig::GetInstance().GetLightConf();
    for (const auto& it : lightConf) {
        if ((capacity >= it.beginSoc) && (capacity <= it.endSoc)) {
            RETURN_IF_WITH_RET(lightColor_ == it.rgb, true);
#ifdef HAS_SENSORS_MISCDEVICE_PART
            TurnOff();
            TurnOn(it.rgb);
#endif
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

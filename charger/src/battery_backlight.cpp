/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "battery_backlight.h"

#include "charger_log.h"
#include "hdf_base.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr uint32_t BACKLIGHT_ON = 128;
}

BatteryBacklight::BatteryBacklight()
{
    if (composer_ == nullptr) {
        composer_.reset(IDisplayComposerInterface::Get());
        if (composer_ == nullptr) {
            return;
        }
    }
}

BatteryBacklight::~BatteryBacklight()
{
}

void BatteryBacklight::TurnOnScreen()
{
    if (screenState_ != SCREEN_ON) {
        BATTERY_HILOGD(FEATURE_CHARGING, "turn on screen");
        uint32_t devId = 0;
        composer_->SetDisplayPowerStatus(devId, POWER_STATUS_ON);
        composer_->SetDisplayBacklight(devId, BACKLIGHT_ON);
        screenState_ = SCREEN_ON;
    }
}

void BatteryBacklight::TurnOffScreen()
{
    if (screenState_ != SCREEN_OFF) {
        BATTERY_HILOGD(FEATURE_CHARGING, "turn off screen");
        uint32_t devId = 0;
        composer_->SetDisplayPowerStatus(devId, POWER_STATUS_OFF);
        screenState_ = SCREEN_OFF;
    }
}

int32_t BatteryBacklight::GetScreenState()
{
    return screenState_;
}
} // namespace PowerMgr
} // namespace OHOS

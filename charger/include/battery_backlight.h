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

#ifndef BATTERY_BACKLIGHT_H
#define BATTERY_BACKLIGHT_H

#include "v1_0/include/idisplay_composer_interface.h"

namespace OHOS {
namespace PowerMgr {
using namespace OHOS::HDI::Display::Composer::V1_0;
class BatteryBacklight {
public:
    enum ScreenState {
        SCREEN_OFF = 0,
        SCREEN_ON,
    };
    BatteryBacklight();
    ~BatteryBacklight();
    void TurnOnScreen();
    void TurnOffScreen();
    int32_t GetScreenState();

private:
    std::shared_ptr<IDisplayComposerInterface> composer_ = nullptr;
    enum ScreenState screenState_ = SCREEN_OFF;
};
} // namespace PowerMgr
} // namespace OHOS
#endif

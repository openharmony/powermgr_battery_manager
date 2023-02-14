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
#ifndef CHARGER_ANIMATION_H
#define CHARGER_ANIMATION_H

#include "battery_log.h"
#include "page/page_manager.h"
#include <string>
#include <thread>
#include <vector>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_1 {
class ChargerAnimation {
    enum AnimationState {
        ANIMATION_STOP = 0,
        ANIMATION_START,
        LACKPOWER_CHARGING_PROMPT_STOP,
        LACKPOWER_CHARGING_PROMPT_START,
        LACKPOWER_NOTCHARGING_PROMPT_STOP,
        LACKPOWER_NOTCHARGING_PROMPT_START,
    };

public:
    explicit ChargerAnimation() : pgMgr_ {Updater::PageManager::GetInstance()} {}
    ~ChargerAnimation() = default;
    bool InitConfig();
    void AnimationStart(const int32_t& capacity);
    void AnimationStop();
    void CapacityDisplay(const int32_t& capacity);
    void CapacityDestroy();
    void LackPowerChargingPromptStart();
    void LackPowerChargingPromptStop();
    void LackPowerNotChargingPromptStart();
    void LackPowerNotChargingPromptStop();

private:
    void InitRootView();
    Updater::PageManager& pgMgr_;
    enum AnimationState animationState_ = ANIMATION_STOP;
    enum AnimationState chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_STOP;
    enum AnimationState notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_STOP;
};
} // namespace V1_1
} // namespace Battery
} // namespace HDI
} // namespace OHOS
#endif

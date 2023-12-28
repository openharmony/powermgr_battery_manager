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

#include "animation_config.h"
#include "charger_log.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"

#include "animator/animator_manager.h"
#include "animator/easing_equation.h"
#include "charger_graphic_engine.h"
#include "components/root_view.h"

#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace OHOS {
namespace PowerMgr {
constexpr int CHAR_WIDTH = 5;
constexpr int MICROSECONDS_PER_MILLISECOND = 1000;
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
    ChargerAnimation() = default;
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

    class ChargerAnimatorCallback : public OHOS::AnimatorCallback {
    public:
        ChargerAnimatorCallback(std::shared_ptr<UIImageView> imageView, ImageComponentInfo& info)
            : view_ {imageView}, info_ {info}
        {
        }
        void Init()
        {
            if (view_ == nullptr) {
                BATTERY_HILOGE(FEATURE_CHARGING, "view_ is nullptr");
                return;
            }
            animator_ = std::make_unique<OHOS::Animator>(this, view_.get(), 0, true);
        }

        virtual void Callback(OHOS::UIView* view)
        {
            if (stop_) {
                return;
            }

            ShowNextImage();
        }

        void Start()
        {
            view_->SetVisible(true);
            GetAnimator()->Start();
            stop_ = false;
        }

        void Stop()
        {
            view_->SetVisible(false);
            stop_ = true;
        }

        OHOS::Animator* GetAnimator() const
        {
            if (animator_ == nullptr) {
                BATTERY_HILOGE(FEATURE_CHARGING, "animator_ is nullptr");
                return nullptr;
            }
            return animator_.get();
        }

        void ShowNextImage()
        {
            std::stringstream ss;
            ss << info_.resPath << info_.filePrefix << std::setw(CHAR_WIDTH) << std::setfill('0') << currId_ << ".png";
            currPath_ = ss.str();
            if (access(currPath_.c_str(), F_OK) == -1) {
                BATTERY_HILOGE(FEATURE_CHARGING, "path not exist");
            }

            view_->SetSrc(currPath_.c_str());
            view_->SetResizeMode(OHOS::UIImageView::ImageResizeMode::FILL);
            currId_ = (currId_ + 1) % info_.imgCnt;
            ChargerGraphicEngine::UsSleep(info_.updInterval * MICROSECONDS_PER_MILLISECOND);
        }

    private:
        std::string currPath_ {};
        uint32_t currId_ {0};
        std::shared_ptr<UIImageView> view_;
        std::unique_ptr<OHOS::Animator> animator_;
        bool stop_;
        ImageComponentInfo info_;
    };

private:
    void InitRootView();
    bool InitAllComponents();
    std::unique_ptr<UILabel> lackPower_;
    std::unique_ptr<UILabel> lackPowerNotCharge_;
    std::unique_ptr<UILabel> percentLabel_;
    std::shared_ptr<UIImageView> animatorImage_;
    std::shared_ptr<ChargerAnimatorCallback> animatorCallback_ = nullptr;
    enum AnimationState animationState_ = ANIMATION_STOP;
    enum AnimationState chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_STOP;
    enum AnimationState notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_STOP;
};
} // namespace PowerMgr
} // namespace OHOS
#endif

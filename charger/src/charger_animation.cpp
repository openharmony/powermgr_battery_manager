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

#include "charger_animation.h"
#include <cstdint>
#include <sys/types.h>
#include <unistd.h>

#include "common/graphic_startup.h"
#include "common/screen.h"
#include "config_policy_utils.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* FONT_PATH = "/system/etc/charger/resources/";
constexpr const char* TEXT_FONT = "ChargerAnimation_Sans_SC_Regular_Small.ttf";
constexpr uint32_t WHITE_BGCOLOR = 0x000000ff;
constexpr int N_HEX = 16;
}; // namespace

bool ChargerAnimation::InitConfig()
{
    OHOS::GraphicStartUp::Init();
    ChargerGraphicEngine::GetInstance().Init(WHITE_BGCOLOR, OHOS::ColorMode::ARGB8888, FONT_PATH, TEXT_FONT);
    InitRootView();
    InitAllComponents();
    return true;
}

template <class T>
T String2Int(const std::string& str, int base = N_HEX)
{
    char* end = nullptr;
    if (str.empty()) {
        errno = EINVAL;
        return 0;
    }
    if (((str[0] == '0') && (str[1] == 'x')) || (str[1] == 'X')) {
        base = N_HEX;
    }
    T result = strtoull(str.c_str(), &end, base);
    return result;
}

static OHOS::ColorType StrToColor(const std::string& hexColor)
{
    std::size_t startPos = 1ul;
    auto getNextField = [&startPos, &hexColor]() {
        constexpr std::size_t width = 2ul;
        uint8_t ret = (startPos > hexColor.size()) ? 0 : String2Int<uint8_t>(hexColor.substr(startPos, width));
        startPos += width;
        return ret;
    };
    auto r = getNextField();
    auto g = getNextField();
    auto b = getNextField();
    auto a = getNextField();
    return OHOS::Color::GetColorFromRGBA(r, g, b, a);
}

static std::unique_ptr<UILabel> InitLabel(LabelComponentInfo& info)
{
    auto label = std::make_unique<UILabel>();
    label->SetViewId(info.common.id.c_str());
    label->SetPosition(info.common.x, info.common.y, info.common.w, info.common.h);
    label->SetText(info.text.c_str());
    label->SetFont(TEXT_FONT, info.fontSize);
    label->SetStyle(OHOS::STYLE_TEXT_COLOR, StrToColor(info.fontColor.c_str()).full);
    label->SetStyle(OHOS::STYLE_BACKGROUND_COLOR, StrToColor(info.bgColor.c_str()).full);
    label->SetVisible(info.common.visible);
    return label;
}

static std::shared_ptr<UIImageView> InitImageView(ImageComponentInfo& info)
{
    auto animatorImage = std::make_shared<UIImageView>();
    animatorImage->SetViewId(info.common.id.c_str());
    animatorImage->SetPosition(info.common.x, info.common.y, info.common.w, info.common.h);
    animatorImage->SetVisible(info.common.visible);
    return animatorImage;
}

bool ChargerAnimation::InitAllComponents()
{
    auto animationConfig = std::make_shared<AnimationConfig>();
    auto ret = animationConfig->ParseConfig();
    if (!ret) {
        return false;
    }
    auto [animationImageInfo, animationLabelInfo] = animationConfig->GetCharingAnimationInfo();
    auto chargingLabelInfo = animationConfig->GetCharingPromptInfo();
    auto notChargingLabelInfo = animationConfig->GetNotCharingPromptInfo();

    lackPower_ = InitLabel(chargingLabelInfo);
    lackPowerNotCharge_ = InitLabel(notChargingLabelInfo);
    percentLabel_ = InitLabel(animationLabelInfo);
    animatorImage_ = InitImageView(animationImageInfo);
    auto container = std::make_unique<UIViewGroup>();
    auto width = ChargerGraphicEngine::GetInstance().GetScreenWidth();
    auto height = ChargerGraphicEngine::GetInstance().GetScreenHeight();
    container->SetPosition(0, 0, width, height);
    container->SetStyle(OHOS::STYLE_BACKGROUND_COLOR, OHOS::Color::Black().full);
    container->Add(lackPower_.get());
    container->Add(lackPowerNotCharge_.get());
    container->Add(animatorImage_.get());
    container->Add(percentLabel_.get());
    container->SetVisible(true);
    OHOS::RootView::GetInstance()->Add(container.release());
    animatorCallback_ = std::make_shared<ChargerAnimatorCallback>(animatorImage_, animationImageInfo);
    animatorCallback_->Init();
    return true;
}

void ChargerAnimation::InitRootView()
{
    OHOS::RootView::GetInstance()->SetPosition(0, 0);
    OHOS::RootView::GetInstance()->SetStyle(OHOS::STYLE_BACKGROUND_COLOR, OHOS::Color::Black().full);
    OHOS::RootView::GetInstance()->Resize(
        OHOS::Screen::GetInstance().GetWidth(), OHOS::Screen::GetInstance().GetHeight());
    OHOS::RootView::GetInstance()->Invalidate();
}

void ChargerAnimation::AnimationStart(const int32_t& capacity)
{
    if (animationState_ != ANIMATION_START) {
        animatorCallback_->Start();
        animationState_ = ANIMATION_START;
    }
    CapacityDisplay(capacity);
    BATTERY_HILOGD(FEATURE_CHARGING, "Animation has been started");
}

void ChargerAnimation::AnimationStop()
{
    if (animationState_ != ANIMATION_STOP) {
        animatorCallback_->Stop();
        CapacityDestroy();
        animationState_ = ANIMATION_STOP;
    }
    BATTERY_HILOGD(FEATURE_CHARGING, "Animation has been stopped");
}

void ChargerAnimation::CapacityDisplay(const int32_t& capacity)
{
    percentLabel_->SetText((std::to_string(capacity) + '%').c_str());
    percentLabel_->SetVisible(true);
}

void ChargerAnimation::CapacityDestroy()
{
    percentLabel_->SetVisible(false);
}

void ChargerAnimation::LackPowerChargingPromptStart()
{
    if (chargingPromptState_ != LACKPOWER_CHARGING_PROMPT_START) {
        lackPower_->SetVisible(true);
        chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_START;
    }
}

void ChargerAnimation::LackPowerChargingPromptStop()
{
    if (chargingPromptState_ != LACKPOWER_CHARGING_PROMPT_STOP) {
        lackPower_->SetVisible(false);
        chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_STOP;
    }
}

void ChargerAnimation::LackPowerNotChargingPromptStart()
{
    if (notChargingPromptState_ != LACKPOWER_NOTCHARGING_PROMPT_START) {
        lackPowerNotCharge_->SetVisible(true);
        notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_START;
    }
}

void ChargerAnimation::LackPowerNotChargingPromptStop()
{
    if (notChargingPromptState_ != LACKPOWER_NOTCHARGING_PROMPT_STOP) {
        lackPowerNotCharge_->SetVisible(false);
        notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_STOP;
    }
}
} // namespace PowerMgr
} // namespace OHOS

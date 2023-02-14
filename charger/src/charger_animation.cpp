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

#include "../include/charger_animation.h"
#include <sys/types.h>

#include "common/graphic_startup.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "graphic_engine.h"

#include "component/img_view_adapter.h"
#include "component/text_label_adapter.h"
#include "layout/layout_parser.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_1 {
namespace {
constexpr const char* UI_CFG_FILE = "/vendor/etc/charger/resources/animation.json";
constexpr const char* FONT_PATH = "/vendor/etc/charger/resources/";
constexpr const char* CHARGER_ANIMATION_PAGE_ID = "Charger:Animation";
constexpr const char* CHARGER_ANIMATION_COM_ID = "Charging_Animation_Image";
constexpr const char* CHARGER_PERCENT_COM_ID = "Charging_Percent_Label";
constexpr const char* LACKPOWER_CHARGING_STATE_PAGE_ID = "Charger:Lackpower_Charging_Prompt";
constexpr const char* LACKPOWER_CHARGING_STATE_COM_ID = "LackPower_Charging_Label";
constexpr const char* LACKPOWER_NOT_CHARGING_STATE_PAGE_ID = "Charger:Lackpower_Not_Charging_Prompt";
constexpr const char* LACKPOWER_NOT_CHARGING_STATE_COM_ID = "LackPower_Not_Charging_Label";
constexpr uint32_t WHITE_BGCOLOR = 0x000000ff;
}; // namespace

bool ChargerAnimation::InitConfig()
{
    // graphic engine init
    OHOS::GraphicStartUp::Init();
    Updater::GraphicEngine::GetInstance().Init(WHITE_BGCOLOR, OHOS::ColorMode::ARGB8888, FONT_PATH);
    InitRootView();

    std::vector<std::string> layoutFiles {UI_CFG_FILE};
    std::vector<Updater::UxPageInfo> pageInfos {};
    if (!Updater::LayoutParser::GetInstance().LoadLayout(layoutFiles, pageInfos)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "load layout error: %{private}s", UI_CFG_FILE);
        return false;
    }

    if (!pgMgr_.Init(pageInfos, CHARGER_ANIMATION_PAGE_ID)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "page manager init error");
        return false;
    }
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
        pgMgr_.ShowPage(CHARGER_ANIMATION_PAGE_ID);
        pgMgr_[CHARGER_ANIMATION_PAGE_ID][CHARGER_ANIMATION_COM_ID].As<Updater::ImgViewAdapter>()->Start();
        animationState_ = ANIMATION_START;
    }
    CapacityDisplay(capacity);
    BATTERY_HILOGD(FEATURE_CHARGING, "Animation has been started");
}

void ChargerAnimation::AnimationStop()
{
    if (animationState_ != ANIMATION_STOP) {
        pgMgr_[CHARGER_ANIMATION_PAGE_ID][CHARGER_ANIMATION_COM_ID].As<Updater::ImgViewAdapter>()->Stop();
        pgMgr_[CHARGER_ANIMATION_PAGE_ID].SetVisible(false);
        CapacityDestroy();
        animationState_ = ANIMATION_STOP;
    }
    BATTERY_HILOGD(FEATURE_CHARGING, "Animation has been stopped");
}

void ChargerAnimation::CapacityDisplay(const int32_t& capacity)
{
    pgMgr_[CHARGER_ANIMATION_PAGE_ID][CHARGER_PERCENT_COM_ID].As<Updater::TextLabelAdapter>()->SetText(
        std::to_string(capacity) + '%');
}

void ChargerAnimation::CapacityDestroy()
{
    pgMgr_[CHARGER_ANIMATION_PAGE_ID][CHARGER_PERCENT_COM_ID]->SetVisible(false);
}

void ChargerAnimation::LackPowerChargingPromptStart()
{
    if (chargingPromptState_ != LACKPOWER_CHARGING_PROMPT_START) {
        pgMgr_.ShowPage(LACKPOWER_CHARGING_STATE_PAGE_ID);
        auto lackPowerChargingTag = pgMgr_[LACKPOWER_CHARGING_STATE_PAGE_ID][LACKPOWER_CHARGING_STATE_COM_ID]
            .As<Updater::TextLabelAdapter>()
            ->GetText();
        pgMgr_[LACKPOWER_CHARGING_STATE_PAGE_ID][LACKPOWER_CHARGING_STATE_COM_ID]
            .As<Updater::TextLabelAdapter>()
            ->SetText(lackPowerChargingTag);
        pgMgr_[LACKPOWER_CHARGING_STATE_PAGE_ID][LACKPOWER_CHARGING_STATE_COM_ID]->SetVisible(true);
        chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_START;
    }
}

void ChargerAnimation::LackPowerChargingPromptStop()
{
    if (chargingPromptState_ != LACKPOWER_CHARGING_PROMPT_STOP) {
        pgMgr_[LACKPOWER_CHARGING_STATE_PAGE_ID][LACKPOWER_CHARGING_STATE_COM_ID]->SetVisible(false);
        chargingPromptState_ = LACKPOWER_CHARGING_PROMPT_STOP;
    }
}

void ChargerAnimation::LackPowerNotChargingPromptStart()
{
    if (notChargingPromptState_ != LACKPOWER_NOTCHARGING_PROMPT_START) {
        pgMgr_.ShowPage(LACKPOWER_NOT_CHARGING_STATE_PAGE_ID);
        auto lackPowerNotChargingTag = pgMgr_[LACKPOWER_NOT_CHARGING_STATE_PAGE_ID][LACKPOWER_NOT_CHARGING_STATE_COM_ID]
            .As<Updater::TextLabelAdapter>()
            ->GetText();
        pgMgr_[LACKPOWER_NOT_CHARGING_STATE_PAGE_ID][LACKPOWER_NOT_CHARGING_STATE_COM_ID]
            .As<Updater::TextLabelAdapter>()
            ->SetText(lackPowerNotChargingTag);
        pgMgr_[LACKPOWER_NOT_CHARGING_STATE_PAGE_ID][LACKPOWER_NOT_CHARGING_STATE_COM_ID]->SetVisible(true);
        notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_START;
    }
}

void ChargerAnimation::LackPowerNotChargingPromptStop()
{
    if (notChargingPromptState_ != LACKPOWER_NOTCHARGING_PROMPT_STOP) {
        pgMgr_[LACKPOWER_NOT_CHARGING_STATE_PAGE_ID][LACKPOWER_NOT_CHARGING_STATE_COM_ID]->SetVisible(false);
        notChargingPromptState_ = LACKPOWER_NOTCHARGING_PROMPT_STOP;
    }
}
} // namespace V1_1
} // namespace Battery
} // namespace HDI
} // namespace OHOS

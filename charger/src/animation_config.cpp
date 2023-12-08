/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation_config.h"
#include "charger_log.h"
#include "config_policy_utils.h"
#include "string_ex.h"

#include <fstream>
#include <sstream>
#include <unistd.h>

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* ANIMATION_CONFIG_PATH = "/system/etc/charger/resources/animation.json";
constexpr const char* CHARGER_ANIMATION_NAME = "animation";
constexpr const char* LACKPOWER_CHARGING_NAME = "lackpowerChargingPrompt";
constexpr const char* LACKPOWER_NOT_CHARGING_NAME = "lackpowerNotChargingPrompt";
constexpr const char* ANIMATION_COM = "components";
constexpr const char* ANIMATION_COM_LABEL = "UILabel";
constexpr const char* ANIMATION_COM_IMAGEVIEW = "UIImageView";
} // namespace

bool AnimationConfig::ParseConfig()
{
    std::ifstream ifs(ANIMATION_CONFIG_PATH);
    if (!ifs.is_open()) {
        BATTERY_HILOGE(FEATURE_CHARGING, "open json file failed");
        return false;
    }
    configObj_.clear();
    configObj_ = nlohmann::json::parse(ifs, nullptr, false);
    ifs.close();
    if (configObj_.is_discarded()) {
        BATTERY_HILOGE(FEATURE_CHARGING, "parse config file error");
        return false;
    }

    auto animation = configObj_[CHARGER_ANIMATION_NAME];
    ParseAnimationConfig(animation);
    auto chargingPrompt = configObj_[LACKPOWER_CHARGING_NAME];
    ParseLackPowerChargingConfig(chargingPrompt);
    auto notChargingPrompt = configObj_[LACKPOWER_NOT_CHARGING_NAME];
    ParseLackPowerNotChargingConfig(notChargingPrompt);

    return true;
}

void AnimationConfig::ParseAnimationLabel(nlohmann::json& component, LabelComponentInfo& info)
{
    info.common.type = ANIMATION_COM_LABEL;
    if (component.find("id") != component.end()) {
        info.common.id = component.at("id").get<std::string>();
    }
    if (component.find("text") != component.end()) {
        info.text = component.at("text").get<std::string>();
    }
    if (component.find("x") != component.end()) {
        info.common.x = component.at("x").get<int>();
    }
    if (component.find("y") != component.end()) {
        info.common.y = component.at("y").get<int>();
    }
    if (component.find("w") != component.end()) {
        info.common.w = component.at("w").get<int>();
    }
    if (component.find("h") != component.end()) {
        info.common.h = component.at("h").get<int>();
    }
    if (component.find("fontSize") != component.end()) {
        info.fontSize = component.at("fontSize").get<int8_t>();
    }
    if (component.find("fontColor") != component.end()) {
        info.fontColor = component.at("fontColor").get<std::string>();
    }
    if (component.find("bgColor") != component.end()) {
        info.bgColor = component.at("bgColor").get<std::string>();
    }
    if (component.find("align") != component.end()) {
        info.align = component.at("align").get<std::string>();
    }
}

void AnimationConfig::ParseAnimationImage(nlohmann::json& component, ImageComponentInfo& info)
{
    info.common.type = ANIMATION_COM_IMAGEVIEW;
    if (component.find("id") != component.end()) {
        info.common.id = component.at("id").get<std::string>();
    }
    if (component.find("x") != component.end()) {
        info.common.x = component.at("x").get<int>();
    }
    if (component.find("y") != component.end()) {
        info.common.y = component.at("y").get<int>();
    }
    if (component.find("w") != component.end()) {
        info.common.w = component.at("w").get<int>();
    }
    if (component.find("h") != component.end()) {
        info.common.h = component.at("h").get<int>();
    }
    if (component.find("resPath") != component.end()) {
        info.resPath = component.at("resPath").get<std::string>();
    }
    if (component.find("imgCnt") != component.end()) {
        info.imgCnt = component.at("imgCnt").get<int>();
    }
    if (component.find("updInterval") != component.end()) {
        info.updInterval = component.at("updInterval").get<int>();
    }
    if (component.find("filePrefix") != component.end()) {
        info.filePrefix = component.at("filePrefix").get<std::string>();
    }
}

void AnimationConfig::ParseAnimationConfig(nlohmann::json& jsonObj)
{
    auto components = jsonObj[ANIMATION_COM];
    for (auto& component : components) {
        if (component.find("type") != component.end()) {
            auto type = component.at("type").get<std::string>();
            if (type == ANIMATION_COM_IMAGEVIEW) {
                ParseAnimationImage(component, animationInfo_.first);
            } else if (type == ANIMATION_COM_LABEL) {
                ParseAnimationLabel(component, animationInfo_.second);
            }
        }
    }
}

void AnimationConfig::ParseLackPowerChargingConfig(nlohmann::json& jsonObj)
{
    auto components = jsonObj[ANIMATION_COM];
    for (auto& component : components) {
        if (component.find("type") != component.end()) {
            auto type = component.at("type").get<std::string>();
            if (type == ANIMATION_COM_LABEL) {
                ParseAnimationLabel(component, chargingInfo_);
            }
        }
    }
}

void AnimationConfig::ParseLackPowerNotChargingConfig(nlohmann::json& jsonObj)
{
    auto components = jsonObj[ANIMATION_COM];
    for (auto& component : components) {
        if (component.find("type") != component.end()) {
            auto type = component.at("type").get<std::string>();
            if (type == ANIMATION_COM_LABEL) {
                ParseAnimationLabel(component, notChargingInfo_);
            }
        }
    }
}

std::pair<ImageComponentInfo, LabelComponentInfo> AnimationConfig::GetCharingAnimationInfo()
{
    return animationInfo_;
}

LabelComponentInfo AnimationConfig::GetCharingPromptInfo()
{
    return chargingInfo_;
}

LabelComponentInfo AnimationConfig::GetNotCharingPromptInfo()
{
    return notChargingInfo_;
}
} // namespace PowerMgr
} // namespace OHOS

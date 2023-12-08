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

#ifndef ANIMATION_CONFIG_H
#define ANIMATION_CONFIG_H

#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include "nocopyable.h"
#include <nlohmann/json.hpp>

namespace OHOS {
namespace PowerMgr {
struct CommonInfo {
    int x;
    int y;
    int w;
    int h;
    std::string id;
    std::string type;
    bool visible;
};

struct LabelComponentInfo {
    uint8_t fontSize;
    std::string text;
    std::string align;
    std::string fontColor;
    std::string bgColor;
    struct CommonInfo common;
};

struct ImageComponentInfo {
    std::string resPath;
    std::string filePrefix;
    uint32_t imgCnt;
    uint32_t updInterval;
    struct CommonInfo common;
};

class AnimationConfig : public NoCopyable {
public:
    bool ParseConfig();
    std::pair<ImageComponentInfo, LabelComponentInfo> GetCharingAnimationInfo();
    LabelComponentInfo GetCharingPromptInfo();
    LabelComponentInfo GetNotCharingPromptInfo();

private:
    void ParseAnimationConfig(nlohmann::json& jsonObj);
    void ParseLackPowerChargingConfig(nlohmann::json& jsonObj);
    void ParseLackPowerNotChargingConfig(nlohmann::json& jsonObj);
    void ParseAnimationImage(nlohmann::json& component, ImageComponentInfo& info);
    void ParseAnimationLabel(nlohmann::json& component, LabelComponentInfo& info);
    std::pair<ImageComponentInfo, LabelComponentInfo> animationInfo_;
    LabelComponentInfo chargingInfo_;
    LabelComponentInfo notChargingInfo_;
    nlohmann::json configObj_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif

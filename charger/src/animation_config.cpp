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
#include "battery_mgr_cjson_utils.h"
#include "charger_log.h"
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

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    configObj_ = cJSON_Parse(content.c_str());
    ifs.close();
    if (!configObj_) {
        BATTERY_HILOGE(FEATURE_CHARGING, "parse config file error");
        return false;
    }
    if (BatteryMgrJsonUtils::IsEmptyJsonParse(configObj_)) {
        cJSON_Delete(configObj_);
        configObj_ = nullptr;
        BATTERY_HILOGW(FEATURE_CHARGING, "cJSON parse result is empty, animation config is %{public}s",
            content.c_str());
        return false;
    }
    auto animation = cJSON_GetObjectItemCaseSensitive(configObj_, CHARGER_ANIMATION_NAME);
    ParseAnimationConfig(animation);
    auto chargingPrompt = cJSON_GetObjectItemCaseSensitive(configObj_, LACKPOWER_CHARGING_NAME);
    ParseLackPowerChargingConfig(chargingPrompt);
    auto notChargingPrompt = cJSON_GetObjectItemCaseSensitive(configObj_, LACKPOWER_NOT_CHARGING_NAME);
    ParseLackPowerNotChargingConfig(notChargingPrompt);
    cJSON_Delete(configObj_);
    configObj_ = nullptr;
    return true;
}

void AnimationConfig::ParseAnimationLabel(cJSON* component, LabelComponentInfo& info)
{
    info.common.type = ANIMATION_COM_LABEL;
    cJSON* idItem = cJSON_GetObjectItemCaseSensitive(component, "id");
    if (BatteryMgrJsonUtils::IsValidJsonString(idItem)) {
        info.common.id = idItem->valuestring;
    }
    cJSON* textItem = cJSON_GetObjectItemCaseSensitive(component, "text");
    if (BatteryMgrJsonUtils::IsValidJsonString(textItem)) {
        info.text = textItem->valuestring;
    }
    cJSON* xitem = cJSON_GetObjectItemCaseSensitive(component, "x");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(xitem)) {
        info.common.x = xitem->valueint;
    }
    cJSON* yItem = cJSON_GetObjectItemCaseSensitive(component, "y");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(yItem)) {
        info.common.y = yItem->valueint;
    }
    cJSON* wItem = cJSON_GetObjectItemCaseSensitive(component, "w");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(wItem)) {
        info.common.w = wItem->valueint;
    }
    cJSON* hItem = cJSON_GetObjectItemCaseSensitive(component, "h");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(hItem)) {
        info.common.h = hItem->valueint;
    }
    cJSON* fontSizeItem = cJSON_GetObjectItemCaseSensitive(component, "fontSize");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(fontSizeItem)) {
        info.fontSize = static_cast<int8_t>(fontSizeItem->valueint);
    }
    cJSON* fontColorItem = cJSON_GetObjectItemCaseSensitive(component, "fontColor");
    if (BatteryMgrJsonUtils::IsValidJsonString(fontColorItem)) {
        info.fontColor = fontColorItem->valuestring;
    }
    cJSON* bgColorItem = cJSON_GetObjectItemCaseSensitive(component, "bgColor");
    if (BatteryMgrJsonUtils::IsValidJsonString(bgColorItem)) {
        info.bgColor = bgColorItem->valuestring;
    }
    cJSON* alignItem = cJSON_GetObjectItemCaseSensitive(component, "align");
    if (BatteryMgrJsonUtils::IsValidJsonString(alignItem)) {
        info.align = alignItem->valuestring;
    }
}

void AnimationConfig::ParseAnimationImage(cJSON* component, ImageComponentInfo& info)
{
    info.common.type = ANIMATION_COM_IMAGEVIEW;
    cJSON* idItem = cJSON_GetObjectItemCaseSensitive(component, "id");
    if (BatteryMgrJsonUtils::IsValidJsonString(idItem)) {
        info.common.id = idItem->valuestring;
    }
    cJSON* xitem = cJSON_GetObjectItemCaseSensitive(component, "x");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(xitem)) {
        info.common.x = xitem->valueint;
    }
    cJSON* yItem = cJSON_GetObjectItemCaseSensitive(component, "y");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(yItem)) {
        info.common.y = yItem->valueint;
    }
    cJSON* wItem = cJSON_GetObjectItemCaseSensitive(component, "w");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(wItem)) {
        info.common.w = wItem->valueint;
    }
    cJSON* hItem = cJSON_GetObjectItemCaseSensitive(component, "h");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(hItem)) {
        info.common.h = hItem->valueint;
    }
    cJSON* resPathItem = cJSON_GetObjectItemCaseSensitive(component, "resPath");
    if (BatteryMgrJsonUtils::IsValidJsonString(resPathItem)) {
        info.resPath = resPathItem->valuestring;
    }
    cJSON* imgCntItem = cJSON_GetObjectItemCaseSensitive(component, "imgCnt");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(imgCntItem)) {
        info.imgCnt = imgCntItem->valueint;
    }
    cJSON* updIntervalItem = cJSON_GetObjectItemCaseSensitive(component, "updInterval");
    if (BatteryMgrJsonUtils::IsValidJsonNumber(updIntervalItem)) {
        info.updInterval = updIntervalItem->valueint;
    }
    cJSON* filePrefixItem = cJSON_GetObjectItemCaseSensitive(component, "filePrefix");
    if (BatteryMgrJsonUtils::IsValidJsonString(filePrefixItem)) {
        info.filePrefix = filePrefixItem->valuestring;
    }
}

void AnimationConfig::ParseAnimationConfig(cJSON* jsonObj)
{
    if (!BatteryMgrJsonUtils::IsValidJsonObject(jsonObj)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "AnimationConfig is invalid");
        return;
    }
    auto components = cJSON_GetObjectItemCaseSensitive(jsonObj, ANIMATION_COM);
    if (!BatteryMgrJsonUtils::IsJsonArrayOrJsonObject(components)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The animationConfig component is not an array or object");
        return;
    }
    cJSON* component = nullptr;
    cJSON_ArrayForEach(component, components) {
        auto typeItem = cJSON_GetObjectItemCaseSensitive(component, "type");
        if (!BatteryMgrJsonUtils::IsValidJsonString(typeItem)) {
            continue;
        }
        if (strcmp(typeItem->valuestring, ANIMATION_COM_IMAGEVIEW) == 0) {
            ParseAnimationImage(component, animationInfo_.first);
        } else if (strcmp(typeItem->valuestring, ANIMATION_COM_LABEL) == 0) {
            ParseAnimationLabel(component, animationInfo_.second);
        }
    }
}

void AnimationConfig::ParseLackPowerChargingConfig(cJSON* jsonObj)
{
    if (!BatteryMgrJsonUtils::IsValidJsonObject(jsonObj)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "LackPowerChargingConfig is invalid");
        return;
    }
    auto components = cJSON_GetObjectItemCaseSensitive(jsonObj, ANIMATION_COM);
    if (!BatteryMgrJsonUtils::IsJsonArrayOrJsonObject(components)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The lackPowerChargingConfig component is not an array or object");
        return;
    }
    cJSON* component = nullptr;
    cJSON_ArrayForEach(component, components) {
        auto typeItem = cJSON_GetObjectItemCaseSensitive(component, "type");
        if (!BatteryMgrJsonUtils::IsValidJsonString(typeItem)) {
            continue;
        }
        if (strcmp(typeItem->valuestring, ANIMATION_COM_LABEL) == 0) {
            ParseAnimationLabel(component, chargingInfo_);
        }
    }
}

void AnimationConfig::ParseLackPowerNotChargingConfig(cJSON* jsonObj)
{
    if (!BatteryMgrJsonUtils::IsValidJsonObject(jsonObj)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "LackPowerNotChargingConfig is invalid");
        return;
    }
    auto components = cJSON_GetObjectItemCaseSensitive(jsonObj, ANIMATION_COM);
    if (!BatteryMgrJsonUtils::IsJsonArrayOrJsonObject(components)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The lackPowerNotChargingConfig component is not an array or object");
        return;
    }
    cJSON* component = nullptr;
    cJSON_ArrayForEach(component, components) {
        auto typeItem = cJSON_GetObjectItemCaseSensitive(component, "type");
        if (!BatteryMgrJsonUtils::IsValidJsonString(typeItem)) {
            continue;
        }
        if (strcmp(typeItem->valuestring, ANIMATION_COM_LABEL) == 0) {
            ParseAnimationLabel(component, notChargingInfo_);
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

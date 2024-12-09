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

#include "battery_config.h"

#include "string_ex.h"

#ifdef HAS_BATTERY_CONFIG_POLICY_PART
#include "config_policy_utils.h"
#endif

#include "battery_log.h"
#include "power_common.h"

namespace {
#ifdef HAS_BATTERY_CONFIG_POLICY_PART
constexpr const char* BATTERY_CONFIG_EXCEPTION_PATH = "";
constexpr const char* BATTERY_CONFIG_PATH = "etc/battery/battery_config.json";
#endif
constexpr const char* SYSTEM_BATTERY_CONFIG_PATH = "/system/etc/battery/battery_config.json";
constexpr const char* VENDOR_BATTERY_CONFIG_PATH = "/vendor/etc/battery/battery_config.json";
constexpr int32_t MAP_KEY_INDEX = 0;
constexpr int32_t BEGIN_SOC_INDEX = 0;
constexpr int32_t END_SOC_INDEX = 1;
constexpr int32_t MAX_SOC_RANGE = 2;
constexpr int32_t RED_INDEX = 0;
constexpr int32_t GREEN_INDEX = 1;
constexpr int32_t BLUE_INDEX = 2;
constexpr int32_t MAX_RGB_RANGE = 3;
constexpr int32_t MAX_DEPTH = 5;
constexpr int32_t MIN_DEPTH = 1;
constexpr uint32_t MOVE_LEFT_16 = 16;
constexpr uint32_t MOVE_LEFT_8 = 8;
constexpr int32_t FIRST_BUTTON_INDEX = 0;
constexpr int32_t SECOND_BUTTON_INDEX = 1;
constexpr int32_t MAX_BUTTON_RANGE = 2;
}
namespace OHOS {
namespace PowerMgr {
std::shared_ptr<BatteryConfig> BatteryConfig::instance_ = nullptr;
std::mutex BatteryConfig::mutex_;

BatteryConfig& BatteryConfig::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = std::make_shared<BatteryConfig>();
    }
    return *(instance_.get());
}

#ifdef HAS_BATTERY_CONFIG_POLICY_PART
bool BatteryConfig::ParseConfig()
{
    char buf[MAX_PATH_LEN];
    char* path = GetOneCfgFile(BATTERY_CONFIG_PATH, buf, MAX_PATH_LEN);
    if (path == nullptr || *path == '\0') {
        BATTERY_HILOGW(COMP_SVC, "GetOneCfgFile battery_config.json is NULL");
        path = const_cast<char*>(BATTERY_CONFIG_EXCEPTION_PATH);
    }
    BATTERY_HILOGD(COMP_SVC, "GetOneCfgFile battery_config.json");

    Json::CharReaderBuilder readerBuilder;
    std::ifstream ifsConf;

    RETURN_IF_WITH_RET(!OpenFile(ifsConf, path), false);

    config_.clear();
    readerBuilder["collectComments"] = false;
    JSONCPP_STRING errs;

    if (parseFromStream(readerBuilder, ifsConf, &config_, &errs) && !config_.empty()) {
        ParseConfInner();
    }
    ifsConf.close();
    return true;
}
#endif

bool BatteryConfig::IsExist(std::string key) const
{
    return !GetValue(key).isNull();
}

int32_t BatteryConfig::GetInt(std::string key, int32_t defVal) const
{
    Json::Value value = GetValue(key);
    return (value.isNull() || !value.isInt()) ? defVal : value.asInt();
}

const std::vector<BatteryConfig::LightConf>& BatteryConfig::GetLightConf() const
{
    return lightConf_;
}

bool BatteryConfig::GetWirelessChargerConf() const
{
    return wirelessChargerEnable_;
}

const std::vector<BatteryConfig::CommonEventConf>& BatteryConfig::GetCommonEventConf() const
{
    return commonEventConf_;
}

bool BatteryConfig::OpenFile(std::ifstream& ifsConf, const std::string& configPath)
{
    bool isOpen = false;
    if (!configPath.empty()) {
        ifsConf.open(configPath);
        isOpen = ifsConf.is_open();
        BATTERY_HILOGD(COMP_SVC, "open configPath file is %{public}d", isOpen);
    }
    RETURN_IF_WITH_RET(isOpen, true);

    ifsConf.open(VENDOR_BATTERY_CONFIG_PATH);
    isOpen = ifsConf.is_open();
    BATTERY_HILOGI(COMP_SVC, "open then vendor battery_config.json is %{public}d", isOpen);
    RETURN_IF_WITH_RET(isOpen, true);

    ifsConf.open(SYSTEM_BATTERY_CONFIG_PATH);
    isOpen = ifsConf.is_open();
    BATTERY_HILOGI(COMP_SVC, "open then system battery_config.json is %{public}d", isOpen);
    return isOpen;
}

void BatteryConfig::ParseConfInner()
{
    lightConf_.clear();
    ParseLightConf("low");
    ParseLightConf("normal");
    ParseLightConf("high");
    BATTERY_HILOGD(COMP_SVC, "The battery light configuration size %{public}d",
        static_cast<int32_t>(lightConf_.size()));
    ParseWirelessChargerConf();
    ParseBootActionsConf();
    ParsePopupConf();
    ParseNotificationConf();
}

void BatteryConfig::ParseLightConf(std::string level)
{
    Json::Value soc = GetValue("light." + level + ".soc");
    Json::Value rgb = GetValue("light." + level + ".rgb");
    if (!soc.isArray() || !rgb.isArray()) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s configuration is invalid.", level.c_str());
        return;
    }

    if (soc.size() != MAX_SOC_RANGE || !soc[BEGIN_SOC_INDEX].isInt() || !soc[END_SOC_INDEX].isInt()) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s soc data type error.", level.c_str());
        return;
    }
    if (rgb.size() != MAX_RGB_RANGE || !rgb[RED_INDEX].isUInt() || !rgb[GREEN_INDEX].isUInt() ||
        !rgb[BLUE_INDEX].isUInt()) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s rgb data type error.", level.c_str());
        return;
    }
    BatteryConfig::LightConf lightConf = {
        .beginSoc = soc[BEGIN_SOC_INDEX].asInt(),
        .endSoc = soc[END_SOC_INDEX].asInt(),
        .rgb = (rgb[RED_INDEX].asUInt() << MOVE_LEFT_16) |
               (rgb[GREEN_INDEX].asUInt() << MOVE_LEFT_8) |
               rgb[BLUE_INDEX].asUInt()
    };
    lightConf_.push_back(lightConf);
}

void BatteryConfig::ParseWirelessChargerConf()
{
    Json::Value wirelessCharger = GetValue("wirelesscharger");
    if (wirelessCharger.isNull() || !wirelessCharger.isInt()) {
        BATTERY_HILOGW(COMP_SVC, "wirelesscharger is invalid");
        return;
    }
    wirelessChargerEnable_ = static_cast<bool>(wirelessCharger.asInt());
}


void BatteryConfig::ParseBootActionsConf()
{
    Json::Value bootActionsConfig = GetValue("boot_actions");
    if (bootActionsConfig.isNull() || !bootActionsConfig.isObject()) {
        BATTERY_HILOGW(COMP_SVC, "boot_actions is invalid");
        return;
    }
    ParseCommonEventConf(bootActionsConfig);
}

void BatteryConfig::ParseCommonEventConf(const Json::Value& bootActionsConfig)
{
    Json::Value commonEventConfs = bootActionsConfig["sendcommonevent"];
    if (commonEventConfs.isNull() || !commonEventConfs.isArray()) {
        BATTERY_HILOGW(COMP_SVC, "The common event config is invalid");
        return;
    }
    commonEventConf_.clear();
    for (const auto& commonEventConf : commonEventConfs) {
        BatteryConfig::CommonEventConf tempCommonEventConf;
        Json::Value eventName = commonEventConf["event_name"];
        Json::Value sceneConfigName = commonEventConf["scene_config"]["name"];
        Json::Value sceneConfigEqual = commonEventConf["scene_config"]["equal"];
        Json::Value sceneConfigNotEqual = commonEventConf["scene_config"]["not_equal"];
        Json::Value uevent = commonEventConf["uevent"];
        if (!eventName.isString() || !sceneConfigName.isString() || !uevent.isString()) {
            BATTERY_HILOGW(COMP_SVC, "parse common event config failed");
            continue;
        }
        if (!sceneConfigEqual.isNull() && sceneConfigEqual.isString()) {
            tempCommonEventConf.sceneConfigEqual = true;
            tempCommonEventConf.sceneConfigValue = sceneConfigEqual.asString();
        } else if (!sceneConfigNotEqual.isNull() && sceneConfigNotEqual.isString()) {
            tempCommonEventConf.sceneConfigEqual = false;
            tempCommonEventConf.sceneConfigValue = sceneConfigNotEqual.asString();
        } else {
            BATTERY_HILOGW(COMP_SVC, "parse expect value failed");
            continue;
        }
        tempCommonEventConf.eventName = eventName.asString();
        tempCommonEventConf.sceneConfigName = sceneConfigName.asString();
        tempCommonEventConf.uevent = uevent.asString();
        commonEventConf_.push_back(tempCommonEventConf);
    }
    BATTERY_HILOGI(COMP_SVC, "The battery commonevent configuration size %{public}d",
        static_cast<int32_t>(commonEventConf_.size()));
}

const std::unordered_map<std::string, std::vector<BatteryConfig::PopupConf>>& BatteryConfig::GetPopupConf() const
{
    BATTERY_HILOGI(COMP_SVC, "GetPopupConf");
    return popupConfig_;
}

void BatteryConfig::ParsePopupConf()
{
    Json::Value popupConfig = GetValue("popup");
    if (popupConfig.isNull() || !popupConfig.isObject()) {
        BATTERY_HILOGW(COMP_SVC, "popupConfig invalid");
        return;
    }
    popupConfig_.clear();
    Json::Value::Members members = popupConfig.getMemberNames();
    for (auto iter = members.begin(); iter != members.end(); iter++) {
        std::string uevent = *iter;
        Json::Value valueObj = popupConfig[uevent];
        if (valueObj.isNull() || !valueObj.isArray()) {
            BATTERY_HILOGW(COMP_SVC, "ueventConf invalid, key=%{public}s", uevent.c_str());
            continue;
        }
        std::vector<BatteryConfig::PopupConf> popupConfVec;
        for (const auto& popupObj : valueObj) {
            Json::Value popupName = popupObj["name"];
            Json::Value popupAction = popupObj["action"];
            if (!popupName.isString() || !popupAction.isUInt()) {
                BATTERY_HILOGW(COMP_SVC, "popupObj invalid, key=%{public}s", uevent.c_str());
                continue;
            }
            BatteryConfig::PopupConf popupCfg = {
                .name = popupName.asString(),
                .action = popupAction.asUInt()
            };
            BATTERY_HILOGI(COMP_SVC, "add popupConf %{public}s, %{public}d", popupCfg.name.c_str(), popupCfg.action);
            popupConfVec.emplace_back(popupCfg);
        }
        BATTERY_HILOGI(COMP_SVC, "popupConfVec size: %{public}d", static_cast<int32_t>(popupConfVec.size()));
        popupConfig_.emplace(uevent, popupConfVec);
    }
    BATTERY_HILOGI(COMP_SVC, "popupConfVec size: %{public}d", static_cast<int32_t>(popupConfig_.size()));
}

const std::unordered_map<std::string, BatteryConfig::NotificationConf>& BatteryConfig::GetNotificationConf() const
{
    return notificationConfMap_;
}

void BatteryConfig::ParseNotificationConf()
{
    Json::Value nConf = GetValue("notification");
    if (nConf.isNull() || !nConf.isArray()) {
        BATTERY_HILOGW(COMP_SVC, "nConf is invalid");
        return;
    }
    for (const auto& conf : nConf) {
        Json::Value nameObj = conf["name"];
        Json::Value iconObj = conf["icon"];
        Json::Value titleObj = conf["title"];
        Json::Value textObj = conf["text"];
        Json::Value buttonObj = conf["button"];
        if (!nameObj.isString() || !iconObj.isString() || !titleObj.isString()
            || !textObj.isString() || !buttonObj.isArray()) {
            BATTERY_HILOGW(COMP_SVC, "stringConf Parse failed");
            continue;
        }
        if (buttonObj.size() != MAX_BUTTON_RANGE || !buttonObj[FIRST_BUTTON_INDEX].isObject() ||
            !buttonObj[SECOND_BUTTON_INDEX].isObject()) {
            BATTERY_HILOGW(COMP_SVC, "buttonConf is invalid");
            continue;
        }
        Json::Value firstButtonNameObj = buttonObj[FIRST_BUTTON_INDEX]["name"];
        Json::Value firstButtonActionObj = buttonObj[FIRST_BUTTON_INDEX]["action"];
        Json::Value secondButtonNameObj = buttonObj[SECOND_BUTTON_INDEX]["name"];
        Json::Value secondButtonActionObj = buttonObj[SECOND_BUTTON_INDEX]["action"];
        if (!firstButtonNameObj.isString() || !firstButtonActionObj.isString()
            || !secondButtonNameObj.isString() || !secondButtonActionObj.isString()) {
            BATTERY_HILOGW(COMP_SVC, "buttonConf Parse failed");
            return;
        }
        std::string name = nameObj.asString();
        BatteryConfig::NotificationConf notificationConf = {
            .name = name,
            .icon = iconObj.asString(),
            .title = titleObj.asString(),
            .firstButton = std::make_pair(firstButtonNameObj.asString(), firstButtonActionObj.asString()),
            .secondButton = std::make_pair(secondButtonNameObj.asString(), secondButtonActionObj.asString())
        };
        BATTERY_HILOGI(COMP_SVC, "notificationConf name: %{public}s", name.c_str());
        notificationConfMap_.emplace(name, notificationConf);
    }
    BATTERY_HILOGI(COMP_SVC, "notificationConf size: %{public}d", static_cast<int32_t>(notificationConfMap_.size()));
}

Json::Value BatteryConfig::FindConf(const std::string& key) const
{
    return (config_.isObject() && config_.isMember(key)) ? config_[key] : Json::Value();
}

bool BatteryConfig::SplitKey(const std::string& key, std::vector<std::string>& keys) const
{
    SplitStr(TrimStr(key), ".", keys);
    return (keys.size() < MIN_DEPTH || keys.size() > MAX_DEPTH) ? false : true;
}

Json::Value BatteryConfig::GetValue(std::string key) const
{
    std::vector<std::string> keys;
    if (!SplitKey(key, keys)) {
        BATTERY_HILOGW(COMP_SVC, "The key does not meet the. key=%{public}s", key.c_str());
        return Json::Value();
    }

    Json::Value value = FindConf(keys[MAP_KEY_INDEX]);
    if (value.isNull()) {
        BATTERY_HILOGD(COMP_SVC, "Value is empty. key=%{public}s", key.c_str());
        return value;
    }

    for (size_t i = 1; i < keys.size(); ++i) {
        if (!value.isObject() || !value.isMember(keys[i])) {
            BATTERY_HILOGW(COMP_SVC, "The key is not configured. key=%{public}s", keys[i].c_str());
            break;
        }
        value = value[keys[i]];
    }
    return value;
}
} // namespace PowerMgr
} // namespace OHOS

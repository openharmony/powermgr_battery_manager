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
#include "battery_mgr_cjson_utils.h"
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

    std::ifstream ifsConf;

    RETURN_IF_WITH_RET(!OpenFile(ifsConf, path), false);

    if (config_) {
        cJSON_Delete(config_);
        config_ = nullptr;
    }

    std::string content((std::istreambuf_iterator<char>(ifsConf)), std::istreambuf_iterator<char>());
    config_ = cJSON_Parse(content.c_str());
    ifsConf.close();
    if (config_ == nullptr) {
        const char* errorPtr = cJSON_GetErrorPtr();
        BATTERY_HILOGW(FEATURE_CHARGING, "cJSON parse error: in %{public}s",
            (errorPtr != nullptr) ? errorPtr : "unknown error");
        return false;
    }

    if (BatteryMgrJsonUtils::IsEmptyJsonParse(config_)) {
        cJSON_Delete(config_);
        config_ = nullptr;
        BATTERY_HILOGW(FEATURE_CHARGING, "cJSON parse result is empty, battery config is %{public}s", content.c_str());
        return false;
    } else {
        ParseConfInner();
    }
    return true;
}
#endif

bool BatteryConfig::IsExist(std::string key) const
{
    cJSON* value = GetValue(key);
    return (value && !cJSON_IsNull(value));
}

int32_t BatteryConfig::GetInt(std::string key, int32_t defVal) const
{
    cJSON* value = GetValue(key);
    return (!BatteryMgrJsonUtils::IsValidJsonNumber(value)) ? defVal : static_cast<int32_t>(value->valueint);
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
    cJSON* soc = GetValue("light." + level + ".soc");
    cJSON* rgb = GetValue("light." + level + ".rgb");
    if (!BatteryMgrJsonUtils::IsValidJsonArray(soc) || !BatteryMgrJsonUtils::IsValidJsonArray(rgb)) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s configuration is invalid.", level.c_str());
        return;
    }
    if (cJSON_GetArraySize(soc) != MAX_SOC_RANGE) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s soc data length error.", level.c_str());
        return;
    }
    cJSON* beginSocItem = cJSON_GetArrayItem(soc, BEGIN_SOC_INDEX);
    cJSON* endSocItem = cJSON_GetArrayItem(soc, END_SOC_INDEX);
    if (!BatteryMgrJsonUtils::IsValidJsonNumber(beginSocItem) || !BatteryMgrJsonUtils::IsValidJsonNumber(endSocItem)) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s soc data type error.", level.c_str());
        return;
    }
    if (cJSON_GetArraySize(rgb) != MAX_RGB_RANGE) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s rgb data length error.", level.c_str());
        return;
    }
    cJSON* redItem = cJSON_GetArrayItem(rgb, RED_INDEX);
    cJSON* greenItem = cJSON_GetArrayItem(rgb, GREEN_INDEX);
    cJSON* blueItem = cJSON_GetArrayItem(rgb, BLUE_INDEX);
    if (!BatteryMgrJsonUtils::IsValidJsonNumber(redItem) || !BatteryMgrJsonUtils::IsValidJsonNumber(greenItem) ||
        !BatteryMgrJsonUtils::IsValidJsonNumber(blueItem)) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s rgb data type error.", level.c_str());
        return;
    }
    BatteryConfig::LightConf lightConf = {
        .beginSoc = static_cast<int32_t>(beginSocItem->valueint),
        .endSoc = static_cast<int32_t>(endSocItem->valueint),
        .rgb = (static_cast<uint32_t>(redItem->valueint) << MOVE_LEFT_16) |
                (static_cast<uint32_t>(greenItem->valueint) << MOVE_LEFT_8) |
                static_cast<uint32_t>(blueItem->valueint)
    };
    lightConf_.push_back(lightConf);
}

void BatteryConfig::ParseWirelessChargerConf()
{
    cJSON* wirelessCharger = GetValue("wirelesscharger");
    if (!BatteryMgrJsonUtils::IsValidJsonNumber(wirelessCharger)) {
        BATTERY_HILOGW(COMP_SVC, "wirelesscharger is invalid");
        return;
    }
    wirelessChargerEnable_ = static_cast<bool>(wirelessCharger->valueint);
}

void BatteryConfig::ParseBootActionsConf()
{
    cJSON* bootActionsConfig = GetValue("boot_actions");
    if (!BatteryMgrJsonUtils::IsValidJsonObject(bootActionsConfig)) {
        BATTERY_HILOGW(COMP_SVC, "boot_actions is invalid");
        return;
    }
    ParseCommonEventConf(bootActionsConfig);
}

void BatteryConfig::ParseCommonEventConf(const cJSON* bootActionsConfig)
{
    cJSON* commonEventConfs = cJSON_GetObjectItemCaseSensitive(bootActionsConfig, "sendcommonevent");
    if (!BatteryMgrJsonUtils::IsValidJsonArray(commonEventConfs)) {
        BATTERY_HILOGW(COMP_SVC, "The common event config is invalid");
        return;
    }
    commonEventConf_.clear();
    cJSON* commonEventConf = nullptr;
    cJSON_ArrayForEach(commonEventConf, commonEventConfs) {
        BatteryConfig::CommonEventConf tempCommonEventConf;
        cJSON* eventName = cJSON_GetObjectItemCaseSensitive(commonEventConf, "event_name");
        cJSON* sceneConfig = cJSON_GetObjectItemCaseSensitive(commonEventConf, "scene_config");
        cJSON* sceneConfigName = sceneConfig ? cJSON_GetObjectItemCaseSensitive(sceneConfig, "name") : nullptr;
        cJSON* sceneConfigEqual = sceneConfig ? cJSON_GetObjectItemCaseSensitive(sceneConfig, "equal") : nullptr;
        cJSON* sceneConfigNotEqual = sceneConfig ?
            cJSON_GetObjectItemCaseSensitive(sceneConfig, "not_equal") : nullptr;
        cJSON* uevent = cJSON_GetObjectItemCaseSensitive(commonEventConf, "uevent");
        if (!BatteryMgrJsonUtils::IsValidJsonString(eventName) ||
            !BatteryMgrJsonUtils::IsValidJsonString(sceneConfigName) ||
            !BatteryMgrJsonUtils::IsValidJsonString(uevent)) {
            BATTERY_HILOGW(COMP_SVC, "parse common event config failed");
            continue;
        }
        if (BatteryMgrJsonUtils::IsValidJsonString(sceneConfigEqual)) {
            tempCommonEventConf.sceneConfigEqual = true;
            tempCommonEventConf.sceneConfigValue = sceneConfigEqual->valuestring;
        } else if (BatteryMgrJsonUtils::IsValidJsonString(sceneConfigNotEqual)) {
            tempCommonEventConf.sceneConfigEqual = false;
            tempCommonEventConf.sceneConfigValue = sceneConfigNotEqual->valuestring;
        } else {
            BATTERY_HILOGW(COMP_SVC, "parse expect value failed");
            continue;
        }
        tempCommonEventConf.eventName = eventName->valuestring;
        tempCommonEventConf.sceneConfigName = sceneConfigName->valuestring;
        tempCommonEventConf.uevent = uevent->valuestring;
        commonEventConf_.emplace_back(tempCommonEventConf);
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
    cJSON* popupConfig = GetValue("popup");
    if (!BatteryMgrJsonUtils::IsValidJsonObject(popupConfig)) {
        BATTERY_HILOGW(COMP_SVC, "popupConfig invalid");
        return;
    }
    popupConfig_.clear();
    cJSON* valueObj = nullptr;
    cJSON_ArrayForEach(valueObj, popupConfig) {
        if (valueObj->string == nullptr) {
            BATTERY_HILOGW(COMP_HDI, "Found null key in popup config");
            continue;
        }
        std::string uevent = valueObj->string;
        if (!BatteryMgrJsonUtils::IsValidJsonArray(valueObj)) {
            BATTERY_HILOGW(COMP_SVC, "ueventConf invalid, key=%{public}s", uevent.c_str());
            continue;
        }
        std::vector<BatteryConfig::PopupConf> popupConfVec;
        cJSON* popupObj = nullptr;
        cJSON_ArrayForEach(popupObj, valueObj) {
            cJSON* popupName = cJSON_GetObjectItemCaseSensitive(popupObj, "name");
            cJSON* popupAction = cJSON_GetObjectItemCaseSensitive(popupObj, "action");
            if (!BatteryMgrJsonUtils::IsValidJsonString(popupName) ||
                !BatteryMgrJsonUtils::IsValidJsonNumber(popupAction)) {
                BATTERY_HILOGW(COMP_SVC, "popupObj invalid, key=%{public}s", uevent.c_str());
                continue;
            }
            BatteryConfig::PopupConf popupCfg = {
                .name = popupName->valuestring,
                .action = static_cast<uint32_t>(popupAction->valueint)
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
    cJSON* nConf = GetValue("notification");
    if (!BatteryMgrJsonUtils::IsValidJsonArray(nConf)) {
        BATTERY_HILOGW(COMP_SVC, "nConf is invalid");
        return;
    }
    SaveNotificationConfToMap(nConf);
}

void BatteryConfig::SaveNotificationConfToMap(cJSON* nConf)
{
    cJSON* conf = nullptr;
    cJSON_ArrayForEach(conf, nConf) {
        cJSON* nameObj = cJSON_GetObjectItemCaseSensitive(conf, "name");
        cJSON* iconObj = cJSON_GetObjectItemCaseSensitive(conf, "icon");
        cJSON* titleObj = cJSON_GetObjectItemCaseSensitive(conf, "title");
        cJSON* textObj = cJSON_GetObjectItemCaseSensitive(conf, "text");
        cJSON* flagsObj = cJSON_GetObjectItemCaseSensitive(conf, "flags");
        cJSON* buttonObj = cJSON_GetObjectItemCaseSensitive(conf, "button");
        if (!BatteryMgrJsonUtils::IsValidJsonString(nameObj) || !BatteryMgrJsonUtils::IsValidJsonString(iconObj) ||
            !BatteryMgrJsonUtils::IsValidJsonString(titleObj) || !BatteryMgrJsonUtils::IsValidJsonString(textObj) ||
            !BatteryMgrJsonUtils::IsValidJsonArray(buttonObj)) {
            BATTERY_HILOGW(COMP_SVC, "stringConf Parse failed");
            continue;
        }
        if (cJSON_GetArraySize(buttonObj) != MAX_BUTTON_RANGE) {
            BATTERY_HILOGW(COMP_SVC, "notificationConf button data length error");
            continue;
        }
        cJSON* firstButton = cJSON_GetArrayItem(buttonObj, FIRST_BUTTON_INDEX);
        cJSON* secondButton = cJSON_GetArrayItem(buttonObj, SECOND_BUTTON_INDEX);
        if (!BatteryMgrJsonUtils::IsValidJsonObject(firstButton) ||
            !BatteryMgrJsonUtils::IsValidJsonObject(secondButton)) {
            BATTERY_HILOGW(COMP_SVC, "buttonConf is invalid");
            continue;
        }
        cJSON* firstButtonNameObj = cJSON_GetObjectItemCaseSensitive(firstButton, "name");
        cJSON* firstButtonActionObj = cJSON_GetObjectItemCaseSensitive(firstButton, "action");
        cJSON* secondButtonNameObj = cJSON_GetObjectItemCaseSensitive(secondButton, "name");
        cJSON* secondButtonActionObj = cJSON_GetObjectItemCaseSensitive(secondButton, "action");
        if (!BatteryMgrJsonUtils::IsValidJsonString(firstButtonNameObj) ||
            !BatteryMgrJsonUtils::IsValidJsonString(firstButtonActionObj) ||
            !BatteryMgrJsonUtils::IsValidJsonString(secondButtonNameObj) ||
            !BatteryMgrJsonUtils::IsValidJsonString(secondButtonActionObj)) {
            BATTERY_HILOGW(COMP_SVC, "buttonConf Parse failed");
            return;
        }
        std::string name = nameObj->valuestring;
        uint32_t controlFlags = 0;
        if (BatteryMgrJsonUtils::IsValidJsonNumber(flagsObj)) {
            controlFlags = static_cast<uint32_t>(flagsObj->valueint);
        }
        BatteryConfig::NotificationConf notificationConf = {
            .name = name,
            .icon = iconObj->valuestring,
            .title = titleObj->valuestring,
            .text = textObj->valuestring,
            .flags = controlFlags,
            .firstButton = std::make_pair(firstButtonNameObj->valuestring, firstButtonActionObj->valuestring),
            .secondButton = std::make_pair(secondButtonNameObj->valuestring, secondButtonActionObj->valuestring)
        };
        BATTERY_HILOGI(COMP_SVC, "notificationConf name: %{public}s, flags: %{public}u", name.c_str(), controlFlags);
        notificationConfMap_.emplace(name, notificationConf);
    }
    BATTERY_HILOGI(COMP_SVC, "notificationConf size: %{public}d", static_cast<int32_t>(notificationConfMap_.size()));
}

cJSON* BatteryConfig::FindConf(const std::string& key) const
{
    return (config_ && cJSON_IsObject(config_) && cJSON_HasObjectItem(config_, key.c_str())) ?
        cJSON_GetObjectItemCaseSensitive(config_, key.c_str()) : nullptr;
}

bool BatteryConfig::SplitKey(const std::string& key, std::vector<std::string>& keys) const
{
    SplitStr(TrimStr(key), ".", keys);
    return (keys.size() < MIN_DEPTH || keys.size() > MAX_DEPTH) ? false : true;
}

cJSON* BatteryConfig::GetValue(std::string key) const
{
    std::vector<std::string> keys;
    if (!SplitKey(key, keys)) {
        BATTERY_HILOGW(COMP_SVC, "The key does not meet the. key=%{public}s", key.c_str());
        return nullptr;
    }

    cJSON* value = FindConf(keys[MAP_KEY_INDEX]);
    if (!value || cJSON_IsNull(value)) {
        BATTERY_HILOGD(COMP_SVC, "Value is empty. key=%{public}s", key.c_str());
        return value;
    }

    for (size_t i = 1; i < keys.size(); ++i) {
        if (!cJSON_IsObject(value) || !cJSON_HasObjectItem(value, keys[i].c_str())) {
            BATTERY_HILOGW(COMP_SVC, "The key is not configured. key=%{public}s", keys[i].c_str());
            break;
        }
        value = cJSON_GetObjectItemCaseSensitive(value, keys[i].c_str());
    }
    return value;
}
} // namespace PowerMgr
} // namespace OHOS

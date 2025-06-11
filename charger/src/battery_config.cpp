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

#include "battery_mgr_cjson_utils.h"
#include "string_ex.h"
#ifdef HAS_BATTERY_CONFIG_POLICY_PART
#include "config_policy_utils.h"
#endif
#include "charger_log.h"

namespace OHOS {
namespace PowerMgr {
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
} // namespace
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
        BATTERY_HILOGW(FEATURE_CHARGING, "GetOneCfgFile battery_config.json is NULL");
        path = const_cast<char*>(BATTERY_CONFIG_EXCEPTION_PATH);
    }
    BATTERY_HILOGD(FEATURE_CHARGING, "GetOneCfgFile battery_config.json");

    std::ifstream ifsConf;
    if (!OpenFile(ifsConf, path)) {
        return false;
    }

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

std::string BatteryConfig::GetString(std::string key, std::string defVal) const
{
    cJSON* value = GetValue(key);
    return (!BatteryMgrJsonUtils::IsValidJsonString(value)) ? defVal : value->valuestring;
}

const std::vector<BatteryConfig::LightConf>& BatteryConfig::GetLightConf() const
{
    return lightConf_;
}

void BatteryConfig::DestroyInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    instance_ = nullptr;
}

bool BatteryConfig::OpenFile(std::ifstream& ifsConf, const std::string& configPath)
{
    bool isOpen = false;
    if (!configPath.empty()) {
        ifsConf.open(configPath);
        isOpen = ifsConf.is_open();
        BATTERY_HILOGD(FEATURE_CHARGING, "open configPath file is %{public}d", isOpen);
    }
    if (isOpen) {
        return true;
    }

    ifsConf.open(VENDOR_BATTERY_CONFIG_PATH);
    isOpen = ifsConf.is_open();
    BATTERY_HILOGI(FEATURE_CHARGING, "open then vendor battery_config.json is %{public}d", isOpen);

    if (isOpen) {
        return true;
    }

    ifsConf.open(SYSTEM_BATTERY_CONFIG_PATH);
    isOpen = ifsConf.is_open();
    BATTERY_HILOGI(FEATURE_CHARGING, "open then system battery_config.json is %{public}d", isOpen);
    return isOpen;
}

void BatteryConfig::ParseConfInner()
{
    lightConf_.clear();
    ParseLightConf("low");
    ParseLightConf("normal");
    ParseLightConf("high");
    BATTERY_HILOGD(FEATURE_CHARGING, "The battery light configuration size %{public}d",
        static_cast<int32_t>(lightConf_.size()));
}

void BatteryConfig::ParseLightConf(std::string level)
{
    cJSON* soc = GetValue("light." + level + ".soc");
    cJSON* rgb = GetValue("light." + level + ".rgb");
    if (!BatteryMgrJsonUtils::IsValidJsonArray(soc) || !BatteryMgrJsonUtils::IsValidJsonArray(rgb)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The battery light %{public}s configuration is invalid.", level.c_str());
        return;
    }
    if (cJSON_GetArraySize(soc) != MAX_SOC_RANGE) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The battery light %{public}s soc data length error.", level.c_str());
        return;
    }
    cJSON* beginSocItem = cJSON_GetArrayItem(soc, BEGIN_SOC_INDEX);
    cJSON* endSocItem = cJSON_GetArrayItem(soc, END_SOC_INDEX);
    if (!BatteryMgrJsonUtils::IsValidJsonNumber(beginSocItem) || !BatteryMgrJsonUtils::IsValidJsonNumber(endSocItem)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The battery light %{public}s soc data type error.", level.c_str());
        return;
    }
    if (cJSON_GetArraySize(rgb) != MAX_RGB_RANGE) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The battery light %{public}s rgb data length error.", level.c_str());
        return;
    }
    cJSON* redItem = cJSON_GetArrayItem(rgb, RED_INDEX);
    cJSON* greenItem = cJSON_GetArrayItem(rgb, GREEN_INDEX);
    cJSON* blueItem = cJSON_GetArrayItem(rgb, BLUE_INDEX);
    if (!BatteryMgrJsonUtils::IsValidJsonNumber(redItem) || !BatteryMgrJsonUtils::IsValidJsonNumber(greenItem) ||
        !BatteryMgrJsonUtils::IsValidJsonNumber(blueItem)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "The battery light %{public}s rgb data type error.", level.c_str());
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
        BATTERY_HILOGW(FEATURE_CHARGING, "The key does not meet the. key=%{public}s", key.c_str());
        return nullptr;
    }

    cJSON* value = FindConf(keys[MAP_KEY_INDEX]);
    if (!value || cJSON_IsNull(value)) {
        BATTERY_HILOGD(FEATURE_CHARGING, "Value is empty. key=%{public}s", key.c_str());
        return value;
    }

    for (size_t i = 1; i < keys.size(); ++i) {
        if (!cJSON_IsObject(value) || !cJSON_HasObjectItem(value, keys[i].c_str())) {
            BATTERY_HILOGW(FEATURE_CHARGING, "The key is not configured. key=%{public}s", keys[i].c_str());
            break;
        }
        value = cJSON_GetObjectItemCaseSensitive(value, keys[i].c_str());
    }
    return value;
}
} // namespace PowerMgr
} // namespace OHOS

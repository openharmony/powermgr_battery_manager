/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "battery_log.h"
#include "power_common.h"

namespace {
constexpr const char* SYSTEM_BATTERY_CONFIG_PATH = "/system/etc/battery/battery_config.json";
constexpr const char* VENDOR_BATTERY_CONFIG_PATH = "/vendor/etc/battery/battery_config.json";
constexpr int32_t MAP_KEY_INDEX = 0;
constexpr int32_t BEGIN_SOC_INDEX = 0;
constexpr int32_t END_SOC_INDEX = 1;
constexpr int32_t RED_INDEX = 0;
constexpr int32_t GREEN_INDEX = 1;
constexpr int32_t BLUE_INDEX = 2;
constexpr int32_t MAX_DEPTH = 5;
constexpr int32_t MIN_DEPTH = 1;
constexpr uint32_t MOVE_LEFT_16 = 16;
constexpr uint32_t MOVE_LEFT_8 = 8;
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

bool BatteryConfig::ParseConfig(std::string configPath)
{
    Json::CharReaderBuilder readerBuilder;
    std::ifstream ifsConf;

    RETURN_IF_WITH_RET (!OpenFile(ifsConf, configPath), false);

    config_.clear();
    readerBuilder["collectComments"] = false;
    JSONCPP_STRING errs;

    if (parseFromStream(readerBuilder, ifsConf, &config_, &errs) && !config_.empty()) {
        ParseConfInner();
    }
    ifsConf.close();
    return true;
}

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

bool BatteryConfig::OpenFile(std::ifstream& ifsConf, const std::string& configPath)
{
    bool isOpen = false;
    if (!configPath.empty()) {
        ifsConf.open(configPath);
        isOpen = ifsConf.is_open();
        BATTERY_HILOGD(COMP_SVC, "open file is %{public}d", isOpen);
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
}

void BatteryConfig::ParseLightConf(std::string level)
{
    Json::Value soc = GetValue("light." + level + ".soc");
    Json::Value rgb = GetValue("light." + level + ".rgb");
    if (!soc.isArray() || !rgb.isArray()) {
        BATTERY_HILOGW(COMP_SVC, "The battery light %{public}s configuration is invalid.", level.c_str());
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
        BATTERY_HILOGW(COMP_SVC, "Value is empty. key=%{public}s", keys[MAP_KEY_INDEX].c_str());
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

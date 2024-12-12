/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "notification_locale.h"
#include "config_policy_utils.h"
#include "locale_config.h"
#include "locale_matcher.h"
#include "battery_info.h"
#include "battery_log.h"
#include "power_common.h"

namespace {
constexpr const char* LOCALE_CONFIG_PATH = "/system/etc/battery/resources/locale_path.json";
constexpr const char* SYSTEM_BATTERY_RESOURCE_PATH = "/system/etc/battery/resources/";
constexpr const char* SYSTEM_BATTERY_RESOURCEEXT_PATH = "/system/etc/battery/resourcesExt/";
constexpr const char* ELEMENT_STRING_FILE = "/element/string.json";
constexpr const char* DEFAULT_LANGUAGE_EN = "base";
}

namespace OHOS {
namespace PowerMgr {
std::shared_ptr<NotificationLocale> NotificationLocale::instance_ = nullptr;
std::mutex NotificationLocale::mutex_;
NotificationLocale& NotificationLocale::GetInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
        instance_ = std::make_shared<NotificationLocale>();
    }
    return *(instance_.get());
}

bool NotificationLocale::ParseJsonfile(const std::string& targetPath,
    std::unordered_map<std::string, std::string>& container)
{
    if (access(targetPath.c_str(), F_OK) != 0) {
        BATTERY_HILOGE(COMP_SVC, "targetPath %{public}s invalid", targetPath.c_str());
        return false;
    }
    std::ifstream inputStream(targetPath.c_str(), std::ios::in | std::ios::binary);
    std::string fileStr(std::istreambuf_iterator<char> {inputStream}, std::istreambuf_iterator<char> {});
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(fileStr.data(), fileStr.data() + fileStr.size(), root)) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s json parse error", targetPath.c_str());
        return false;
    }
    if (root.isNull() || !root.isObject()) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s json root error", targetPath.c_str());
        return false;
    }
    Json::Value stringConf = root["String"];
    if (stringConf.isNull() || !stringConf.isArray()) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s stringConf invalid", targetPath.c_str());
    }
    for (const auto& conf : stringConf) {
        Json::Value nameObj = conf["name"];
        Json::Value valueObj = conf["value"];
        if (nameObj.isString() && valueObj.isString() &&
            !nameObj.asString().empty() && !valueObj.asString().empty()) {
            container.insert(std::make_pair(nameObj.asString(), valueObj.asString()));
        }
    }
    BATTERY_HILOGE(COMP_SVC, "%{public}s stringConf end", targetPath.c_str());
    return true;
}

void NotificationLocale::ParseLocaleCfg()
{
    languageMap_.clear();
    ParseJsonfile(LOCALE_CONFIG_PATH, languageMap_);
}

void NotificationLocale::UpdateStringMap()
{
    OHOS::Global::I18n::LocaleInfo locale(Global::I18n::LocaleConfig::GetSystemLocale());
    std::string curBaseName = locale.GetBaseName();
    if (localeBaseName_ == curBaseName) {
        return;
    }
    BATTERY_HILOGI(COMP_SVC, "UpdateResourceMap: change from [%{public}s] to [%{public}s]",
        localeBaseName_.c_str(), curBaseName.c_str());
    localeBaseName_ = curBaseName;
    std::string language = DEFAULT_LANGUAGE_EN;
    if (languageMap_.find(localeBaseName_) != languageMap_.end()) {
        language = languageMap_[localeBaseName_];
    } else {
        for (auto& iter : languageMap_) {
            OHOS::Global::I18n::LocaleInfo eachLocale(iter.first);
            if (OHOS::Global::I18n::LocaleMatcher::Match(&locale, &eachLocale)) {
                language = iter.second;
                break;
            }
        }
    }
    stringMap_.clear();
    std::string resourcePath = SYSTEM_BATTERY_RESOURCE_PATH + language + ELEMENT_STRING_FILE;
    ParseJsonfile(resourcePath, stringMap_);
    resourcePath = SYSTEM_BATTERY_RESOURCEEXT_PATH + language + ELEMENT_STRING_FILE;
    ParseJsonfile(resourcePath, stringMap_);
}

std::string NotificationLocale::GetStringByKey(const std::string& key)
{
    auto iter = stringMap_.find(key);
    if (iter != stringMap_.end()) {
        return iter->second;
    }
    return "";
}
}
}
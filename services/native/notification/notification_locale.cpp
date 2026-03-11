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

#include <unistd.h>
#include <cJSON.h>

#include "notification_locale.h"
#include "config_policy_utils.h"
#include "locale_config.h"
#include "locale_matcher.h"
#include "battery_info.h"
#include "battery_log.h"
#include "battery_mgr_cjson_utils.h"
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

    return SaveJsonToMap(fileStr, targetPath, container);
}

bool NotificationLocale::SaveJsonToMap(const std::string& fileStr, const std::string& targetPath,
    std::unordered_map<std::string, std::string>& container)
{
    cJSON* root = cJSON_Parse(fileStr.c_str());
    if (!root) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s json parse error", targetPath.c_str());
        return false;
    }
    if (cJSON_IsNull(root) || !cJSON_IsObject(root)) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s json root error", targetPath.c_str());
        cJSON_Delete(root);
        return false;
    }
    cJSON* stringConf = cJSON_GetObjectItemCaseSensitive(root, "string");
    if (!BatteryMgrJsonUtils::IsValidJsonArray(stringConf)) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s stringConf invalid", targetPath.c_str());
        cJSON_Delete(root);
        return false;
    }
    cJSON* conf = nullptr;
    cJSON_ArrayForEach(conf, stringConf) {
        cJSON* nameObj = cJSON_GetObjectItemCaseSensitive(conf, "name");
        cJSON* valueObj = cJSON_GetObjectItemCaseSensitive(conf, "value");
        if (BatteryMgrJsonUtils::IsValidJsonStringAndNoEmpty(nameObj) &&
            BatteryMgrJsonUtils::IsValidJsonStringAndNoEmpty(valueObj)) {
            container.insert(std::make_pair(nameObj->valuestring, valueObj->valuestring));
        }
    }
    cJSON_Delete(root);
    BATTERY_HILOGI(COMP_SVC, "%{public}s stringConf end", targetPath.c_str());
    return true;
}

void NotificationLocale::ParseLocaleCfg()
{
    if (islanguageMapInit_) {
        return;
    }
    languageMap_.clear();
    if (ParseJsonfile(LOCALE_CONFIG_PATH, languageMap_)) {
        islanguageMapInit_ = true;
    }
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
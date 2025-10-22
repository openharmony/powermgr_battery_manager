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

#ifndef POWERMGR_BATTERY_MANAGER_BATTERY_CONFIG_H
#define POWERMGR_BATTERY_MANAGER_BATTERY_CONFIG_H

#include <fstream>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <unordered_map>

#include <cJSON.h>
#include "nocopyable.h"

namespace OHOS {
namespace PowerMgr {
class BatteryConfig : public NoCopyable {
public:
    struct LightConf {
        int32_t beginSoc;
        int32_t endSoc;
        uint32_t rgb;
    };
    struct CommonEventConf {
        std::string eventName;
        std::string uevent;
        std::string sceneConfigName;
        bool sceneConfigEqual;
        std::string sceneConfigValue;
    };
    struct PopupConf {
        std::string name;
        int32_t action;
    };
    struct NotificationConf {
        std::string name;
        std::string icon;
        std::string title;
        std::string text;
        uint32_t bannerFlags = 0;
        std::pair<std::string, std::string> firstButton;
        std::pair<std::string, std::string> secondButton;
        const std::string GetInfo() const
        {
            return "name: " + name + ", icon: " + icon + ", title: " + title +
                ", text: " + text + ", bannerFlags: " + std::to_string(bannerFlags) +
                ", firstButton: " + firstButton.first + ", " + firstButton.second +
                ", secondButton: " + secondButton.first + ", " + secondButton.second;
        }
    };
    static BatteryConfig& GetInstance();
    bool ParseConfig();
    bool IsExist(std::string key) const;
    int32_t GetInt(std::string key, int32_t defVal = 0) const;
    const std::vector<LightConf>& GetLightConf() const;
    bool GetWirelessChargerConf() const;
    const std::vector<BatteryConfig::CommonEventConf>& GetCommonEventConf() const;
    const std::unordered_map<std::string, std::vector<BatteryConfig::PopupConf>>& GetPopupConf() const;
    const std::unordered_map<std::string, BatteryConfig::NotificationConf>& GetNotificationConf() const;

private:
    bool OpenFile(std::ifstream& ifsConf, const std::string& configPath);
    void ParseConfInner();
    void ParseLightConf(std::string level);
    void ParseWirelessChargerConf();
    void ParseBootActionsConf();
    void ParsePopupConf();
    void ParseNotificationConf();
    void SaveNotificationConfToMap(cJSON* nConf);
    void ParseCommonEventConf(const cJSON* bootActionsConfig);
    cJSON* FindConf(const std::string& key) const;
    bool SplitKey(const std::string& key, std::vector<std::string>& keys) const;
    cJSON* GetValue(std::string key) const;
    cJSON* config_;
    std::vector<BatteryConfig::LightConf> lightConf_;
    std::vector<BatteryConfig::CommonEventConf> commonEventConf_;
    bool wirelessChargerEnable_ { false };
    std::unordered_map<std::string, std::vector<BatteryConfig::PopupConf>> popupConfig_;
    std::unordered_map<std::string, BatteryConfig::NotificationConf> notificationConfMap_;
    static std::mutex mutex_;
    static std::shared_ptr<BatteryConfig> instance_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // POWERMGR_BATTERY_MANAGER_BATTERY_CONFIG_H

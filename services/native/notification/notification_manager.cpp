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

#include "notification_manager.h"
#include "notification_locale.h"
#include "battery_log.h"

namespace OHOS {
namespace PowerMgr {
static const uint32_t PUBLISH_POPUP_ACTION = 0;
static const uint32_t CANCLE_POPUP_ACTION = 1;

void NotificationManager::HandleNotification(const std::string& popupName, uint32_t popupAction,
    const std::unordered_map<std::string, BatteryConfig::NotificationConf>& nConfMap)
{
    NotificationLocale::GetInstance().ParseLocaleCfg();
    NotificationLocale::GetInstance().UpdateStringMap();
    if (popupAction == PUBLISH_POPUP_ACTION) {
        auto iter = nConfMap.find(popupName);
        if (iter != nConfMap.end()) {
            BatteryConfig::NotificationConf nCfg = FillNotificationCfg(iter->second);
            PublishNotification(nCfg);
        }
    } else if (popupAction == CANCLE_POPUP_ACTION) {
        CancleNotification(popupName);
    }
}

void NotificationManager::PublishNotification(BatteryConfig::NotificationConf& nCfg)
{
    BATTERY_HILOGI(COMP_SVC, "Satrt PublishNotification %{public}s", nCfg.GetInfo().c_str());
    std::shared_ptr<IBatteryNotification> batteryNotification = std::make_shared<NotificationCenter>();
    if (batteryNotification == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "PublishNotification batteryNotification is null");
        return;
    }
    batteryNotification->CreateBaseStyle(nCfg);

    if (!nCfg.firstButton.first.empty()) {
        batteryNotification = CreateButtonStyle(batteryNotification, nCfg.firstButton);
    }
    if (!nCfg.secondButton.first.empty()) {
        batteryNotification = CreateButtonStyle(batteryNotification, nCfg.secondButton);
    }

    if (batteryNotification == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "CreateButtonStyle failed");
        return;
    }
    batteryNotification->PublishNotification();
    std::lock_guard<std::mutex> lock(mapMutex_);
    notificationMap_.emplace(nCfg.name, batteryNotification);
}

std::shared_ptr<IBatteryNotification> NotificationManager::CreateButtonStyle(
    const std::shared_ptr<IBatteryNotification>& batteryNotification,
    const std::pair<std::string, std::string>& nButton)
{
    std::shared_ptr<IBatteryNotification> buttonWarp = nullptr;
    if (batteryNotification == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "CreateButtonStyle:%{public}s failed", nButton.first.c_str());
        return buttonWarp;
    }
    buttonWarp = std::make_shared<ButtonDecorator>(batteryNotification);
    if (buttonWarp == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "buttonWarp is null");
        return buttonWarp;
    }
    buttonWarp->SetActionButton(nButton.first, nButton.second);
    return buttonWarp;
}


void NotificationManager::CancleNotification(const std::string& popupName)
{
    std::lock_guard<std::mutex> lock(mapMutex_);
    auto iter = notificationMap_.find(popupName);
    if (iter != notificationMap_.end()) {
        std::shared_ptr<IBatteryNotification> batteryNotification = iter->second;
        if (batteryNotification != nullptr) {
            batteryNotification->CancelNotification();
        }
        notificationMap_.erase(popupName);
    }
}

BatteryConfig::NotificationConf NotificationManager::FillNotificationCfg(const BatteryConfig::NotificationConf& cfg)
{
    auto& localeConfig = NotificationLocale::GetInstance();
    BatteryConfig::NotificationConf temp = cfg;
    temp.title = localeConfig.GetStringByKey(cfg.title);
    temp.text = localeConfig.GetStringByKey(cfg.text);
    temp.firstButton.first = localeConfig.GetStringByKey(cfg.firstButton.first);
    temp.secondButton.first = localeConfig.GetStringByKey(cfg.secondButton.first);
    return temp;
}

extern "C" API void HandleNotification(const std::string& name, int32_t action,
    const std::unordered_map<std::string, BatteryConfig::NotificationConf>& nConfMap)
{
    NotificationManager::GetInstance().HandleNotification(name, action, nConfMap);
}

}
}
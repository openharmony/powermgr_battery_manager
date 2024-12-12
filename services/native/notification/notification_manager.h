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

#ifndef BATTERY_NOTIFICATION_MANAGER_H
#define BATTERY_NOTIFICATION_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include "battery_config.h"
#include "notification_center.h"
#include "notification_decorator.h"

namespace OHOS {
namespace PowerMgr {
class NotificationManager {
public:
    static NotificationManager& GetInstance()
    {
        static NotificationManager notificationMgr;
        return notificationMgr;
    }
    void HandleNotification(const std::string& popupName, uint32_t popupAction);
    void CancleNotification(const std::string& popupName);
private:
    NotificationManager() = default;
    virtual ~NotificationManager() = default;
    void PublishNotification(BatteryConfig::NotificationConf& nCfg);
    std::shared_ptr<IBatteryNotification> CreateButtonStyle(
        const std::shared_ptr<IBatteryNotification>& batteryNotification,
        const std::pair<std::string, std::string>& nButton);
    BatteryConfig::NotificationConf FillNotificationCfg(const BatteryConfig::NotificationConf& cfg);
    std::mutex mapMutex_;
    std::unordered_map<std::string, std::shared_ptr<IBatteryNotification>> notificationMap_;
};
}   // namespace PowerMgr
}   // namespace OHOS

#endif // BATTERY_NOTIFICATION_MANAGER_H
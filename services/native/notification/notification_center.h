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

#ifndef BATTERY_NOTIFICATION_CENTER_H
#define BATTERY_NOTIFICATION_CENTER_H

#include "ibattery_notification.h"
#include "notification_helper.h"

namespace OHOS {
namespace PowerMgr {
class NotificationCenter : public IBatteryNotification {
public:
    NotificationCenter() = default;
    ~NotificationCenter() override = default;

    void CreateBaseStyle(const BatteryConfig::NotificationConf& nCfg) override;

    bool PublishNotification() override;

    bool CancelNotification() override;

    void SetActionButton(const std::string& buttonName, const std::string& buttonAction) override;
protected:
    void SetNotificationId(const std::string& popupName);

    void SetContent(const std::string& title, const std::string& text);

    void SetCreatorUid();

    void SetCreatorBundleName();

    void SetSlotType();

    void SetInProgress();

    void SetUnremovable();

    void SetBadgeIconStyle();

    void SetNotificationControlFlags(uint32_t flags);

    void SetLittleIcon(const std::string& iconPath);

    Notification::NotificationRequest request_;
};
}   // namespace PowerMgr
}   // namespace OHOS

#endif // BATTERY_NOTIFICATION_CENTER_H
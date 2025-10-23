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

#include <vector>
#include <unistd.h>
#include "notification_helper.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"
#include "battery_log.h"
#include "image_source.h"
#include "pixel_map.h"
#include "notification_center.h"

namespace OHOS {
namespace PowerMgr {
static const int BATTERY_NOTIFICATION_SYS_ABILITY_ID = 5528;
static const std::string BATTERY_NOTIFICATION_SYS_ABILITY_NAME = "";

void NotificationCenter::CreateBaseStyle(const BatteryConfig::NotificationConf& nCfg)
{
    SetNotificationId(nCfg.name);
    SetContent(nCfg.title, nCfg.text);
    SetCreatorUid();
    SetCreatorBundleName();
    SetSlotType();
    SetInProgress();
    SetUnremovable();
    SetBadgeIconStyle();
    SetLittleIcon(nCfg.icon);
    SetNotificationControlFlags(nCfg.bannerFlags);
}

void NotificationCenter::SetNotificationId(const std::string& popupName)
{
    int32_t notificationId = static_cast<int32_t>(std::hash<std::string>()(popupName));
    BATTERY_HILOGI(COMP_SVC, "SetNotificationId notifationId[%{public}d]", notificationId);
    request_.SetNotificationId(notificationId);
}

void NotificationCenter::SetContent(const std::string& title, const std::string& text)
{
    std::shared_ptr<Notification::NotificationNormalContent> content
        = std::make_shared<Notification::NotificationNormalContent>();
    if (content == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "Failed to create NotificationNormalContent");
        return;
    }
    content->SetTitle(title);
    content->SetText(text);
    std::shared_ptr<Notification::NotificationContent> notificationContent
        = std::make_shared<Notification::NotificationContent>(content);
    if (notificationContent == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "Failed to create NotificationContent");
        return;
    }
    request_.SetContent(notificationContent);
}

void NotificationCenter::SetCreatorUid()
{
    request_.SetCreatorUid(BATTERY_NOTIFICATION_SYS_ABILITY_ID);
}

void NotificationCenter::SetCreatorBundleName()
{
    request_.SetCreatorBundleName(BATTERY_NOTIFICATION_SYS_ABILITY_NAME);
}

void NotificationCenter::SetSlotType()
{
    request_.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
}

void NotificationCenter::SetInProgress()
{
    request_.SetInProgress(true);
}

void NotificationCenter::SetUnremovable()
{
    request_.SetUnremovable(true);
}

void NotificationCenter::SetBadgeIconStyle()
{
    request_.SetBadgeIconStyle(Notification::NotificationRequest::BadgeStyle::LITTLE);
}

void NotificationCenter::SetNotificationControlFlags(uint32_t bannerFlags)
{
    request_.SetNotificationControlFlags(request_.GetNotificationControlFlags() | bannerFlags);
}

void NotificationCenter::SetLittleIcon(const std::string& iconPath)
{
    if (access(iconPath.c_str(), F_OK) != 0) {
        BATTERY_HILOGE(COMP_SVC, "iconPath[%{public}s] is invalid", iconPath.c_str());
        return;
    }
    uint32_t errorCode = 0;
    Media::SourceOptions opts;
    opts.formatHint = "image/png";
    auto imageSource = Media::ImageSource::CreateImageSource(iconPath, opts, errorCode);
    if (imageSource == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "Failed to create ImageSource");
        return;
    }
    Media::DecodeOptions decodeOpts;
    std::unique_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    request_.SetLittleIcon(std::move(pixelMap));
}

void NotificationCenter::SetActionButton(const std::string& buttonName, const std::string& buttonAction)
{
    if (buttonName.empty()) {
        BATTERY_HILOGE(COMP_SVC, "SetActionButton buttonCfg is NULL");
        return;
    }
    auto want = std::make_shared<AAFwk::Want>();
    want->SetAction(buttonAction);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::CONSTANT_FLAG);
    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        0, AbilityRuntime::WantAgent::WantAgentConstant::OperationType::SEND_COMMON_EVENT,
        flags, wants, nullptr
    );
    auto wantAgentDeal = AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    std::shared_ptr<Notification::NotificationActionButton> actionButtonDeal =
        Notification::NotificationActionButton::Create(nullptr, buttonName, wantAgentDeal);
    request_.AddActionButton(actionButtonDeal);
}

bool NotificationCenter::PublishNotification()
{
    ErrCode code = Notification::NotificationHelper::PublishNotification(request_);
    BATTERY_HILOGI(COMP_SVC, "NotificationCenter::PublishNotification: %{public}d", static_cast<int32_t>(code));
    return true;
}

bool NotificationCenter::CancelNotification()
{
    int32_t notificationId = request_.GetNotificationId();
    ErrCode code = Notification::NotificationHelper::CancelNotification(notificationId);
    BATTERY_HILOGI(COMP_SVC, "NotificationCenter::CancelNotification: %{public}d", static_cast<int32_t>(code));
    return true;
}
}
}
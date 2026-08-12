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

#include "notification_decorator.h"
#include "battery_log.h"
#include "battery_srv_client.h"

namespace {
static const std::string REVERSE_SUPER_CHARGE_OPEN = "notification.battery.reverse_super_charge_open";
static const std::string REVERSE_SUPER_CHARGE_CLOSE = "notification.battery.reverse_super_charge_close";
static const std::string REVERSE_SUPER_CHARGE_CHANGE_DIR = "notification.battery.reverse_super_charge_change_direction";
}
namespace OHOS {
namespace PowerMgr {
bool NotificationDecorator::PublishNotification()
{
    if (batteryNotification_ == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "batteryNotification_ is nullptr");
        return false;
    }
    batteryNotification_->PublishNotification();
    return true;
}

bool NotificationDecorator::CancelNotification()
{
    if (batteryNotification_ == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "batteryNotification_ is nullptr");
        return false;
    }
    batteryNotification_->CancelNotification();
    return true;
}

void ButtonDecorator::SetActionButton(const std::string& buttonName, const std::string& buttonAction)
{
    if (batteryNotification_ == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "batteryNotification_ is nullptr");
        return;
    }
    batteryNotification_->SetActionButton(buttonName, buttonAction);
    std::shared_ptr<ButtonFactory> button = nullptr;
    if (buttonAction == REVERSE_SUPER_CHARGE_OPEN) {
        button = std::make_shared<ReverseSuperChargeOpenButton>();
    } else if (buttonAction == REVERSE_SUPER_CHARGE_CLOSE) {
        button = std::make_shared<ReverseSuperChargeCloseButton>();
    } else if (buttonAction == REVERSE_SUPER_CHARGE_CHANGE_DIR) {
        button = std::make_shared<ReverseSuperChargeChangeDirButton>();
    }
    if (button == nullptr) {
        BATTERY_HILOGW(COMP_SVC, "button_ is nullptr, buttonName[%{public}s] buttonAction[%{public}s]",
            buttonName.c_str(), buttonAction.c_str());
        return;
    }
    button->RegisterButtonEvent(buttonAction);
    buttonVec_.emplace_back(button);
}

void ReverseSuperChargeOpenButton::RegisterButtonEvent(const std::string& buttonAction)
{
    EventHandle onReceiveOpenModeEvent = [this](const OHOS::EventFwk::CommonEventData& data) {
        this->OpenMode();
    };
    buttonCes_.AddEventHandle(buttonAction, onReceiveOpenModeEvent);
    buttonCes_.RegisterCesEvent();
}

void ReverseSuperChargeOpenButton::OpenMode()
{
    BatterySrvClient::GetInstance().SetBatteryConfig("reverse_super_charge", "2");
    BATTERY_HILOGI(COMP_SVC, "onReceiveOpenModeEvent end");
}

void ReverseSuperChargeCloseButton::RegisterButtonEvent(const std::string& buttonAction)
{
    EventHandle onReceiveCloseModeEvent = [this](const OHOS::EventFwk::CommonEventData& data) {
        this->CloseMode();
    };
    buttonCes_.AddEventHandle(buttonAction, onReceiveCloseModeEvent);
    buttonCes_.RegisterCesEvent();
}

void ReverseSuperChargeCloseButton::CloseMode()
{
    BatterySrvClient::GetInstance().SetBatteryConfig("reverse_super_charge", "1");
    BATTERY_HILOGI(COMP_SVC, "onReceiveCloseModeEvent end");
}

void ReverseSuperChargeChangeDirButton::RegisterButtonEvent(const std::string& buttonAction)
{
    EventHandle onReceiveChangeDirEvent = [this](const OHOS::EventFwk::CommonEventData& data) {
        this->ChangeDir();
    };
    buttonCes_.AddEventHandle(buttonAction, onReceiveChangeDirEvent);
    buttonCes_.RegisterCesEvent();
}

void ReverseSuperChargeChangeDirButton::ChangeDir()
{
    BatterySrvClient::GetInstance().SetBatteryConfig("reverse_super_charge", "0");
    BATTERY_HILOGI(COMP_SVC, "onReceiveChangeDirEvent end");
}
}
}
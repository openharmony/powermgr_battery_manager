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

#ifndef BATTERY_NOTIFICATION_DECORATOR_H
#define BATTERY_NOTIFICATION_DECORATOR_H

#include <memory>
#include <string>
#include "button_event.h"
#include "notification_center.h"

namespace OHOS {
namespace PowerMgr {
class ButtonFactory {
public:
    ButtonFactory() = default;
    virtual ~ButtonFactory() = default;
    virtual void RegisterButtonEvent(const std::string& buttonAction);
};
class NotificationDecorator : public IBatteryNotification {
public:
    explicit NotificationDecorator(std::shared_ptr<IBatteryNotification> batteryNotification)
        : batteryNotification_(batteryNotification) {}
    ~NotificationDecorator() override = default;
    bool PublishNotification() override;
    bool CancelNotification() override;
protected:
    std::shared_ptr<IBatteryNotification> batteryNotification_;
};

class ButtonDecorator : public NotificationDecorator {
public:
    explicit ButtonDecorator(std::shared_ptr<IBatteryNotification> batteryNotification)
        : NotificationDecorator(batteryNotification) {}
    ~ButtonDecorator() override = default;
    void SetActionButton(const std::string& buttonName, const std::string& buttonAction) override;
private:
    std::shared_ptr<ButtonFactory> button_;
};

class ReverseSuperChargeOpenButton : public ButtonFactory {
public:
    ReverseSuperChargeOpenButton() {}
    ~ReverseSuperChargeOpenButton() override
    {
        buttonCes_.UnRegisterCesEvent();
    }
    void RegisterButtonEvent(const std::string& buttonAction) override;
private:
    void OpenMode();
    ButtonCes buttonCes_;
};

class ReverseSuperChargeCloseButton : public ButtonFactory {
public:
    ReverseSuperChargeCloseButton() {}
    ~ReverseSuperChargeCloseButton() override
    {
        buttonCes_.UnRegisterCesEvent();
    }
    void RegisterButtonEvent(const std::string& buttonAction) override;
private:
    void CloseMode();
    ButtonCes buttonCes_;
};
}   // namespace PowerMgr
}   // namespace OHOS

#endif // BATTERY_NOTIFICATION_DECORATOR_H
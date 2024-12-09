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

#include "button_event.h"
#include "battery_log.h"

namespace {
constexpr int32_t POWERUID = 5528;
}
namespace OHOS {
namespace PowerMgr {
void ButtonCes::AddEventHandle(const std::string& action, EventHandle func)
{
    eventHandles_[action] = func;
}

void ButtonCes::RegisterCesEvent()
{
    if (isCesEventSubscribered_) {
        return;
    }
    OHOS::EventFwk::MatchingSkills matchingSkills;
    for (auto& itFunc : eventHandles_) {
        matchingSkills.AddEvent(itFunc.first);
    }
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    subscriberInfo.SetPublisherUid(POWERUID);
    cesEventSubscriber_ = std::make_shared<Ces>(subscriberInfo, eventHandles_);
    if (cesEventSubscriber_ == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "cesEventSubscriber_ nullptr");
        return;
    }
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(cesEventSubscriber_)) {
        cesEventSubscriber_ = nullptr;
        BATTERY_HILOGE(COMP_SVC, "SubscriberCommonEvent fail");
    } else {
        isCesEventSubscribered_ = true;
    }
}

void ButtonCes::UnRegisterCesEvent()
{
    if (!isCesEventSubscribered_) {
        return;
    }
    EventFwk::CommonEventManager::UnSubscribeCommonEvent(cesEventSubscriber_);
    cesEventSubscriber_ = nullptr;
    isCesEventSubscribered_ = false;
}

ButtonCes::Ces::Ces(const CommonEventSubscribeInfo &subscriberInfo,
    const std::unordered_map<std::string, EventHandle>& handles)
    : CommonEventSubscriber(subscriberInfo), eventHandles_(handles)
{
    BATTERY_HILOGI(COMP_SVC, "Button CesEventSubscriber");
}

ButtonCes::Ces::~Ces()
{
    BATTERY_HILOGI(COMP_SVC, "Button ~CesEventSubscriber");
}

void ButtonCes::Ces::OnReceiveEvent(const CommonEventData &eventData)
{
    auto action = eventData.GetWant().GetAction();
    auto it = eventHandles_.find(action);
    if (it == eventHandles_.end()) {
        BATTERY_HILOGI(COMP_SVC, "Ignore Event: %{public}s", action.c_str());
        return;
    }
    BATTERY_HILOGD(COMP_SVC, "Handle Event: %{public}s", action.c_str());
    auto func = it->second;
    if (func) {
        it->second(eventData);
    }
}
}
}
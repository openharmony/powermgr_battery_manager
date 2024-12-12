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
#ifndef BATTERY_BUTTON_EVENT_H
#define BATTERY_BUTTON_EVENT_H

#include <string>
#include <unordered_map>
#include <memory>
#include "common_event_manager.h"
#include "common_event_support.h"

namespace OHOS {
namespace PowerMgr {
using EventHandle = std::function<void(const OHOS::EventFwk::CommonEventData &data)>;
using namespace OHOS::EventFwk;
class ButtonCes {
public:
    void AddEventHandle(const std::string& action, EventHandle func);
    
    void RegisterCesEvent();

    void UnRegisterCesEvent();

    class Ces : public CommonEventSubscriber {
    public:
        explicit Ces(const CommonEventSubscribeInfo &subscriberInfo,
            const std::unordered_map<std::string, EventHandle>& handles);
        virtual ~Ces();
        void OnReceiveEvent(const CommonEventData &eventData) override;
    private:
        const std::unordered_map<std::string, EventHandle>& eventHandles_;
    };
private:
    bool isCesEventSubscribered_ = false;
    std::shared_ptr<Ces> cesEventSubscriber_ = nullptr;
    std::unordered_map<std::string, EventHandle> eventHandles_;
};
}  // namespace PowerMgr
}  // namespace OHOS

#endif // BATTERY_BUTTON_EVENT_H
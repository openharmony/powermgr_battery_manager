/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_BATTERY_SERVICE_EVENT_HANDLER_H
#define POWERMGR_BATTERY_SERVICE_EVENT_HANDLER_H

#include <memory>
#include "refbase.h"
#include "inner_event.h"
#include "event_runner.h"
#include "event_handler.h"

namespace OHOS {
namespace PowerMgr {
class BatteryService;

class BatteryServiceEventHandler : public AppExecFwk::EventHandler {
public:
    enum {
        EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER,
        EVENT_REGISTER_BATTERY_HDI_CALLBACK
    };

    BatteryServiceEventHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
        const wptr<BatteryService>& service);
    ~BatteryServiceEventHandler() = default;
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event) override;

private:
    wptr<BatteryService> service_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // POWERMGR_BATTERY_SERVICE_EVENT_HANDLER_H

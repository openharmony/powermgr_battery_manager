/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_service_event_handler.h"
#include "battery_log.h"
#include "battery_service.h"

namespace OHOS {
namespace PowerMgr {
BatteryServiceEventHandler::BatteryServiceEventHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
    const wptr<BatteryService>& service)
    : AppExecFwk::EventHandler(runner), service_(service)
{
    BATTERY_HILOGD(COMP_SVC, "Instance created");
}

void BatteryServiceEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    auto bmsptr = service_.promote();
    if (bmsptr == nullptr) {
        BATTERY_HILOGE(COMP_SVC, "Battery service is nullptr");
        return;
    }
    BATTERY_HILOGI(COMP_SVC, "Start to process, eventId: %{public}d", event->GetInnerEventId());
    switch (event->GetInnerEventId()) {
        case EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER: {
            bmsptr->RegisterHdiStatusListener();
            break;
        }
        case EVENT_REGISTER_BATTERY_HDI_CALLBACK: {
            bmsptr->RegisterBatteryHdiCallback();
            break;
        }
        default:
            BATTERY_HILOGW(COMP_SVC, "No matched event id");
    }
}
} // namespace PowerMgr
} // namespace OHOS

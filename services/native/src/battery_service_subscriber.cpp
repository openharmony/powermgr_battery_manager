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

#include "battery_service_subscriber.h"

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "ohos/aafwk/content/want.h"
#include "power_common.h"
#include "string_ex.h"

using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace PowerMgr {
BatteryServiceSubscriber::BatteryServiceSubscriber() {}

int32_t BatteryServiceSubscriber::Update(const BatteryInfo &info)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryInfo: capacity=%{public}d, voltage=%{public}d, " \
                                      "temperature=%{public}d, healthState=%{public}d, pluggedType=%{public}d, " \
                                      "pluggedMaxCurrent=%{public}d, pluggedMaxVoltage=%{public}d, " \
                                      "chargeState=%{public}d, chargeCounter=%{public}d, present=%{public}d, " \
                                      "technology=%{public}s",
        info.GetCapacity(), info.GetVoltage(), info.GetTemperature(), info.GetHealthState(),
        info.GetPluggedType(), info.GetPluggedMaxCurrent(), info.GetPluggedMaxVoltage(), info.GetChargeState(),
        info.GetChargeCounter(), info.IsPresent(), info.GetTechnology().c_str());
    bool ret = HandleBatteryChangedEvent(info);
    return ret ? ERR_OK : ERR_NO_INIT;
}

bool BatteryServiceSubscriber::HandleBatteryChangedEvent(const BatteryInfo &info)
{
    Want want;
    want.SetAction(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventData data;
    data.SetWant(want);
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CAPACITY);
    data.SetData(ToString(info.GetCapacity()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_VOLTAGE);
    data.SetData(ToString(info.GetVoltage()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_TEMPERATURE);
    data.SetData(ToString(info.GetTemperature()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_HEALTH_STATE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetHealthState())));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_TYPE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetPluggedType())));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_CURRENT);
    data.SetData(ToString(info.GetPluggedMaxCurrent()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PLUGGED_MAX_VOLTAGE);
    data.SetData(ToString(info.GetPluggedMaxVoltage()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CHARGE_STATE);
    data.SetData(ToString(static_cast<uint32_t>(info.GetChargeState())));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_CHARGE_COUNTER);
    data.SetData(ToString(info.GetChargeCounter()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_PRESENT);
    data.SetData(ToString(info.IsPresent()));
    data.SetCode(BatteryInfo::COMMON_EVENT_CODE_TECHNOLOGY);
    data.SetData(info.GetTechnology());
    CommonEventPublishInfo publishInfo;
    publishInfo.SetOrdered(true);
    bool isSuccess = CommonEventManager::PublishCommonEvent(data, publishInfo);
    if (!isSuccess) {
        POWER_HILOGD(MODULE_BATT_SERVICE, "failed to publish BATTERY_CHANGED event");
    }
    return isSuccess;
}
} // namespace PowerMgr
} // namespace OHOS

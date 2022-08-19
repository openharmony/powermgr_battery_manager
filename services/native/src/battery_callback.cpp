/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "battery_callback.h"

#include "hdf_base.h"
#include "battery_log.h"

namespace OHOS {
namespace PowerMgr {
BatteryCallback::BatteryEventCallback BatteryCallback::eventCb_ = nullptr;
int32_t BatteryCallback::Update(const HDI::Battery::V1_1::BatteryInfo& event)
{
    if (eventCb_ == nullptr) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "eventCb_ is nullptr, cannot update battery info");
        return HDF_FAILURE;
    }
    return eventCb_(event);
}

int32_t BatteryCallback::RegisterBatteryEvent(const BatteryEventCallback& eventCb)
{
    eventCb_ = eventCb;
    return HDF_SUCCESS;
}
} // OHOS
} // PowerMgr

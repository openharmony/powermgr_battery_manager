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

#include "ohbattery_info.h"

#include  <cinttypes>

#include "battery_log.h"
#include "battery_srv_client.h"

using namespace OHOS::PowerMgr;

int32_t OH_BatteryInfo_GetCapacity()
{
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t ret = batterySrvClient.GetCapacity();
    BATTERY_HILOGD(LABEL_TEST, "OH_BatteryInfo_GetCapacity called, value = %{public}d", ret);
    return ret;
}

BatteryInfo_BatteryPluggedType OH_BatteryInfo_GetPluggedType()
{
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    uint32_t result = static_cast<uint32_t>(batterySrvClient.GetPluggedType());
    BatteryInfo_BatteryPluggedType ret = PLUGGED_TYPE_NONE;
    switch (result) {
        case static_cast<uint32_t>(PLUGGED_TYPE_AC):
            ret = PLUGGED_TYPE_AC;
            break;
        case static_cast<uint32_t>(PLUGGED_TYPE_USB):
            ret = PLUGGED_TYPE_USB;
            break;
        case static_cast<uint32_t>(PLUGGED_TYPE_WIRELESS):
            ret = PLUGGED_TYPE_WIRELESS;
            break;
        case static_cast<uint32_t>(PLUGGED_TYPE_NONE):
            ret = PLUGGED_TYPE_NONE;
            break;
        default:
            ret = PLUGGED_TYPE_BUTT;
            break;
    }
    BATTERY_HILOGD(LABEL_TEST, "OH_BatteryInfo_GetPluggedType called, result = %{public}" PRIu32, result);
    return ret;
}

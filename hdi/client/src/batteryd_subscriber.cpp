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

#include "batteryd_subscriber.h"
#include "power_common.h"

namespace OHOS {
namespace PowerMgr {
int BatterydSubscriber::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    switch (code) {
        case CMD_NOTIFY_SUBSCRIBER: {
            const BatteryInfo info = ParseBatteryInfo(data, reply, option);
            return Update(info);
        }
        default: {
            POWER_HILOGW(MODULE_BATTERYD, "no specific cmd for code: %{public}d", code);
            return ERR_INVALID_OPERATION;
        }
    }
}

const BatteryInfo BatterydSubscriber::ParseBatteryInfo(MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    BatteryInfo info;
    info.SetCapacity(data.ReadInt32());
    info.SetVoltage(data.ReadInt32());
    info.SetTemperature(data.ReadInt32());
    info.SetHealthState((BatteryHealthState)data.ReadInt32());
    info.SetPluggedType((BatteryPluggedType)data.ReadInt32());
    info.SetPluggedMaxCurrent(data.ReadInt32());
    info.SetPluggedMaxVoltage(data.ReadInt32());
    info.SetChargeState((BatteryChargeState)data.ReadInt32());
    info.SetChargeCounter(data.ReadInt32());
    info.SetPresent((bool)data.ReadInt8());
    info.SetTechnology(data.ReadCString());
    return info;
}
} // namespace PowerMgr
} // namespace OHOS

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
int BatterydSubscriber::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    switch (code) {
        case CMD_NOTIFY_SUBSCRIBER: {
            const BatteryInfo &info = ParserBatteryInfo(data, reply, option);
            return Update(info);
        }
        default: {
            POWER_HILOGW(MODULE_BATTERYD, "no specific cmd for code: %{public}d", code);
            return ERR_INVALID_OPERATION;
        }
    }
}

const BatteryInfo &BatterydSubscriber::ParserBatteryInfo(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    auto builder = std::make_shared<BatteryInfo::Builder>();
    auto *info = builder->SetCapacity(data.ReadInt32())
        ->SetVoltage(data.ReadInt32())
        ->SetTemperature(data.ReadInt32())
        ->SetHealthState((BatteryHealthState)data.ReadInt32())
        ->SetPluggedType((BatteryPluggedType)data.ReadInt32())
        ->SetPluggedMaxCurrent(data.ReadInt32())
        ->SetPluggedMaxVoltage(data.ReadInt32())
        ->SetChargeState((BatteryChargeState)data.ReadInt32())
        ->SetChargeCounter(data.ReadInt32())
        ->SetPresent((bool)data.ReadInt8())
        ->SetTechnology(data.ReadCString())
        ->Build();
    return *info;
}
} // namespace PowerMgr
} // namespace OHOS
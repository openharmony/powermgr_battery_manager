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

#ifndef BATTERY_SRV_STUB_H
#define BATTERY_SRV_STUB_H

#include <cstdint>
#include "refbase.h"
#include "nocopyable.h"
#include "iremote_stub.h"
#include "ibattery_srv.h"
#include "message_option.h"

namespace OHOS {
namespace PowerMgr {
class BatterySrvStub : public IRemoteStub<IBatterySrv> {
public:
    DISALLOW_COPY_AND_MOVE(BatterySrvStub);

    BatterySrvStub() = default;

    virtual ~BatterySrvStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

private:
    int32_t GetCapacityStub(MessageParcel& reply);
    int32_t GetChargingStatusStub(MessageParcel& reply);
    int32_t GetHealthStatusStub(MessageParcel& reply);
    int32_t GetPluggedTypeStub(MessageParcel& reply);
    int32_t GetVoltageStub(MessageParcel& reply);
    int32_t GetPresentStub(MessageParcel& reply);
    int32_t GetTechnologyStub(MessageParcel& reply);
    int32_t GetBatteryTemperatureStub(MessageParcel& reply);
    int32_t GetCapacityLevelStub(MessageParcel& reply);
    int64_t GetRemainingChargeTimeStub(MessageParcel& reply);
    int32_t GetNowCurrentStub(MessageParcel& reply);
    int32_t GetRemainEnergyStub(MessageParcel& reply);
    int32_t GetTotalEnergyStub(MessageParcel& reply);
    int32_t CheckRequestCode(const uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_SRV_STUB_H

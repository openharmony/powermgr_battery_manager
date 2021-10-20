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

#ifndef BATTERY_HOST_SERVICE_STUB_H
#define BATTERY_HOST_SERVICE_STUB_H

#include "message_parcel.h"
#include "message_option.h"
#include "refbase.h"
#include "core/hdf_device_desc.h"
#include "batteryd_api.h"
#include "power_supply_provider.h"
#include "battery_thread.h"
#include "battery_config.h"
#include "battery_led.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryHostServiceStub {
public:
    BatteryHostServiceStub() {}
    virtual ~BatteryHostServiceStub() {}

    int32_t Init();
    int32_t DispatchBindBatterySubscriber(HdfSBuf& data);
    int32_t DispatchUnbindBatterySubscriber();
    int32_t DispatchGetCapacity(MessageParcel& reply) const;
    int32_t DispatchChangePath(MessageParcel& data) const;
    int32_t DispatchGetVoltage(MessageParcel& reply) const;
    int32_t DispatchGetTemperature(MessageParcel& reply) const;
    int32_t DispatchGetHealthState(MessageParcel& reply) const;
    int32_t DispatchGetPluggedType(MessageParcel& reply) const;
    int32_t DispatchGetChargeState(MessageParcel& reply) const;
    int32_t DispatchGetPresent(MessageParcel& reply) const;
    int32_t DispatchGetTechnology(MessageParcel& reply) const;
    void UpdateBatterydInfo(const char *msg) const;
    void UpdateBatterydInfo() const;
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
private:
    void HandleTemperature(const int32_t &temperature) const;
    void ShutDown(std::string &reason) const;
    void NotifySubscriber(const struct BatterydInfo *batteryInfo) const;
    struct HdfRemoteService *subscriber_ =  nullptr;
    std::unique_ptr<BatteryThread> loop_ = nullptr;
    std::unique_ptr<BatteryConfig> batteryConfig_ = nullptr;
    std::unique_ptr<BatteryLed> batteryLed_ = nullptr;
};

void *BatteryHostServiceStubInstance();

int32_t BatteryHostServiceOnRemoteRequest(char *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // BATTERY_HOST_SERVICE_STUB_H

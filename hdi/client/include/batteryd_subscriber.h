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

#ifndef BATTERYD_SUBSCRIBER_H
#define BATTERYD_SUBSCRIBER_H

#include "battery_info.h"
#include "batteryd_api.h"
#include "ipc_object_stub.h"

namespace OHOS {
namespace PowerMgr {
class BatterydSubscriber : public IPCObjectStub {
public:
    explicit BatterydSubscriber() : IPCObjectStub(u"ohos.powermgr.IBatterydSubscriber") {};
    virtual ~BatterydSubscriber() = default;
    virtual int32_t Update(const BatteryInfo &info) = 0;
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    static const BatteryInfo &ParserBatteryInfo(MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERYD_SUBSCRIBER_H

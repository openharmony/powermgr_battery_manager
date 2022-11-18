/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "mock_remote_object.h"

namespace OHOS {
namespace PowerMgr {
MockRemoteObject::MockRemoteObject(std::u16string descriptor) : IRemoteObject(descriptor) {}

MockRemoteObject::~MockRemoteObject() {}

int32_t MockRemoteObject::GetObjectRefCount()
{
    return 0;
}

int MockRemoteObject::Dump(int fd, const std::vector<std::u16string>& args)
{
    (void)args;
    return 0;
}

int MockRemoteObject::SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    (void)code;
    (void)data;
    (void)reply;
    (void)option;
    return -1;
}

bool MockRemoteObject::AddDeathRecipient(const sptr<DeathRecipient>& recipient)
{
    (void)recipient;
    return false;
}

bool MockRemoteObject::RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
{
    (void)recipient;
    return false;
}
} // namespace PowerMgr
} // namespace OHOS

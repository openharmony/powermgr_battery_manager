/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "mock_common_event_manager.h"

namespace OHOS {
namespace EventFwk {
bool CommonEventManager::PublishCommonEvent(const CommonEventData &data)
{
    return PowerMgr::MockBatteryCommonEventManager::PublishCommonEvent(data);
}

bool CommonEventManager::PublishCommonEventAsUser(const CommonEventData &data, const int32_t &userId)
{
    return PowerMgr::MockBatteryCommonEventManager::PublishCommonEventAsUser(data, userId);
}


bool CommonEventManager::PublishCommonEvent(
    const CommonEventData &data, const CommonEventPublishInfo &publishInfo)
{
    return PowerMgr::MockBatteryCommonEventManager::PublishCommonEvent(data, publishInfo);
}


int32_t CommonEventManager::NewPublishCommonEvent(
    const CommonEventData &data, const CommonEventPublishInfo &publishInfo)
{
    return PowerMgr::MockBatteryCommonEventManager::NewPublishCommonEvent(data, publishInfo);
}

bool CommonEventManager::PublishCommonEventAsUser(
    const CommonEventData &data,
    const CommonEventPublishInfo &publishInfo,
    const int32_t &userId)
{
    return PowerMgr::MockBatteryCommonEventManager::PublishCommonEventAsUser(data, publishInfo, userId);
}


int32_t CommonEventManager::NewPublishCommonEventAsUser(
    const CommonEventData &data,
    const CommonEventPublishInfo &publishInfo,
    const int32_t &userId)
{
    return PowerMgr::MockBatteryCommonEventManager::PublishCommonEventAsUser(data, publishInfo, userId);
}
}

namespace PowerMgr {
bool MockBatteryCommonEventManager::boolReturnValue_ = false;
int32_t MockBatteryCommonEventManager::int32ReturnValue_ = 0;

bool MockBatteryCommonEventManager::PublishCommonEvent(const EventFwk::CommonEventData &data)
{
    return boolReturnValue_;
}

bool MockBatteryCommonEventManager::PublishCommonEventAsUser(
    const EventFwk::CommonEventData &data, const int32_t &userId)
{
    return boolReturnValue_;
}

bool MockBatteryCommonEventManager::PublishCommonEvent(
    const EventFwk::CommonEventData &data, const EventFwk::CommonEventPublishInfo &publishInfo)
{
    return boolReturnValue_;
}

int32_t MockBatteryCommonEventManager::NewPublishCommonEvent(
    const EventFwk::CommonEventData &data, const EventFwk::CommonEventPublishInfo &publishInfo)
{
    return int32ReturnValue_;
}

bool MockBatteryCommonEventManager::PublishCommonEventAsUser(
    const EventFwk::CommonEventData &data,
    const EventFwk::CommonEventPublishInfo &publishInfo,
    const int32_t &userId)
{
    return boolReturnValue_;
}

int32_t MockBatteryCommonEventManager::NewPublishCommonEventAsUser(
    const EventFwk::CommonEventData &data,
    const EventFwk::CommonEventPublishInfo &publishInfo,
    const int32_t &userId)
{
    return int32ReturnValue_;
}
}
}
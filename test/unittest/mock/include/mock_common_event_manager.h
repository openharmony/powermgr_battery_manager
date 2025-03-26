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

#ifndef BASE_POWERMGR_BATTERY_MOCK_COMMON_EVENT_MANAGER
#define BASE_POWERMGR_BATTERY_MOCK_COMMON_EVENT_MANAGER
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_publish_info.h"
#include "common_event_support.h"

namespace OHOS {
namespace PowerMgr {
class MockBatteryCommonEventManager {
public:
    /**
     * Publishes a standard common event.
     *
     * @param data Indicates the common event data.
     * @return Returns true if success; false otherwise.
     */
    static bool PublishCommonEvent(const EventFwk::CommonEventData &data);

    /**
     * Publishes a standard common event.
     *
     * @param data Indicates the common event data.
     * @param userId Indicates the user ID.
     * @return Returns true if success; false otherwise.
     */
    static bool PublishCommonEventAsUser(const EventFwk::CommonEventData &data, const int32_t &userId);

    /**
     * Publishes a common event.
     *
     * @param data Indicates the common event data.
     * @param publishInfo Indicates the publish info.
     * @return Returns true if success; false otherwise.
     */
    static bool PublishCommonEvent(
        const EventFwk::CommonEventData &data, const EventFwk::CommonEventPublishInfo &publishInfo);

    /**
     * Publishes a common event.
     *
     * @param data Indicates the common event data.
     * @param publishInfo Indicates the publish info.
     * @return Returns ERR_OK if success; otherwise failed.
     */
    static int32_t NewPublishCommonEvent(
        const EventFwk::CommonEventData &data, const EventFwk::CommonEventPublishInfo &publishInfo);

    /**
     * Publishes a common event.
     *
     * @param data Indicates the common event data.
     * @param publishInfo Indicates the publish info.
     * @param userId Indicates the user ID.
     * @return Returns true if success; false otherwise.
     */
    static bool PublishCommonEventAsUser(
        const EventFwk::CommonEventData &data,
        const EventFwk::CommonEventPublishInfo &publishInfo,
        const int32_t &userId);

    /**
     * Publishes a common event.
     *
     * @param data Indicates the common event data.
     * @param publishInfo Indicates the publish info.
     * @param userId Indicates the user ID.
     * @return Returns ERR_OK if success; otherwise failed.
     */
    static int32_t NewPublishCommonEventAsUser(
        const EventFwk::CommonEventData &data,
        const EventFwk::CommonEventPublishInfo &publishInfo,
        const int32_t &userId);
    
    static void SetBoolReturnValue(const bool value)
    {
        boolReturnValue_ = value;
    }
    static void SetInt32ReturnValue(const int32_t value)
    {
        int32ReturnValue_ = value;
    }
private:
    static bool boolReturnValue_;
    static int32_t int32ReturnValue_;
};
}
}
#endif
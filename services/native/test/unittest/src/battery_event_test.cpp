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

#include "battery_event_test.h"
#include "battery_service.h"
#include "battery_service_event_handler.h"
#include "power_common.h"
#include <memory>

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* TEST_BATTERY_SERVICE_NAME = "TestBatteryService";
} // namespace

/**
 * @tc.name: BatteryEvent001
 * @tc.desc: eventId is EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER, ProcessEvent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryEventTest, BatteryEvent001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent001 start.");
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner = AppExecFwk::EventRunner::Create(TEST_BATTERY_SERVICE_NAME);
    std::shared_ptr<BatteryServiceEventHandler> eventHandler;
    eventHandler =
        std::make_shared<BatteryServiceEventHandler>(eventRunner, DelayedSpSingleton<BatteryService>::GetInstance());
    EXPECT_NE(nullptr, eventHandler);

    int32_t eventId = static_cast<int32_t>(BatteryServiceEventHandler::EVENT_RETRY_REGISTER_HDI_STATUS_LISTENER);
    auto event = AppExecFwk::InnerEvent::Get(eventId);
    if (eventHandler) {
        eventHandler->ProcessEvent(event);
    }
    EXPECT_TRUE(event->GetInnerEventId() == eventId);
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent001 end.");
}

/**
 * @tc.name: BatteryEvent002
 * @tc.desc: eventId is EVENT_REGISTER_BATTERY_HDI_CALLBACK, ProcessEvent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryEventTest, BatteryEvent002, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent002 start.");
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner = AppExecFwk::EventRunner::Create(TEST_BATTERY_SERVICE_NAME);
    std::shared_ptr<BatteryServiceEventHandler> eventHandler;
    eventHandler =
        std::make_shared<BatteryServiceEventHandler>(eventRunner, DelayedSpSingleton<BatteryService>::GetInstance());
    EXPECT_NE(nullptr, eventHandler);

    int32_t eventId = static_cast<int32_t>(BatteryServiceEventHandler::EVENT_REGISTER_BATTERY_HDI_CALLBACK);
    auto event = AppExecFwk::InnerEvent::Get(eventId);
    if (eventHandler) {
        eventHandler->ProcessEvent(event);
    }
    EXPECT_TRUE(event->GetInnerEventId() == eventId);
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent002 end.");
}

/**
 * @tc.name: BatteryEvent003
 * @tc.desc: eventId is default, ProcessEvent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryEventTest, BatteryEvent003, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent003 start.");
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner = AppExecFwk::EventRunner::Create(TEST_BATTERY_SERVICE_NAME);
    std::shared_ptr<BatteryServiceEventHandler> eventHandler;
    eventHandler =
        std::make_shared<BatteryServiceEventHandler>(eventRunner, DelayedSpSingleton<BatteryService>::GetInstance());
    EXPECT_NE(nullptr, eventHandler);

    int32_t eventId = 100;
    auto event = AppExecFwk::InnerEvent::Get(eventId);
    if (eventHandler) {
        eventHandler->ProcessEvent(event);
    }
    EXPECT_TRUE(event->GetInnerEventId() == eventId);
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent003 end.");
}

/**
 * @tc.name: BatteryEvent004
 * @tc.desc: The batterService in EventRunner is empty
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryEventTest, BatteryEvent004, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent004 start.");
    sptr<BatteryService> batterService = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> eventRunner = AppExecFwk::EventRunner::Create(TEST_BATTERY_SERVICE_NAME);
    std::shared_ptr<BatteryServiceEventHandler> eventHandler;
    eventHandler = std::make_shared<BatteryServiceEventHandler>(eventRunner, batterService);
    EXPECT_NE(nullptr, eventHandler);

    int32_t eventId = static_cast<int32_t>(BatteryServiceEventHandler::EVENT_REGISTER_BATTERY_HDI_CALLBACK);
    auto event = AppExecFwk::InnerEvent::Get(eventId);
    if (eventHandler) {
        eventHandler->ProcessEvent(event);
    }
    EXPECT_TRUE(event->GetInnerEventId() == eventId);
    BATTERY_HILOGD(LABEL_TEST, "BatteryEvent004 end.");
}
} // namespace PowerMgr
} // namespace OHOS

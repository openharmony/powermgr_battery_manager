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

#include "battery_subscriber_test.h"

#include <memory>
#include <string>

#include "common_event_manager.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS::EventFwk;
using namespace OHOS;
using namespace std;

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions subscriber
 * @tc.type: FUNC
 */
static HWTEST_F(BatterySubscriberTest, BatteryService001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService001 start.");
    const std::string EVENT = CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EVENT);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<SubscriberTest> subscriber = std::make_shared<SubscriberTest>(subscriberInfo);
    bool subscribeResult = CommonEventManager::SubscribeCommonEvent(subscriber);
    BATTERY_HILOGD(LABEL_TEST, "subscribeResult=%{public}d", subscribeResult);

    sleep(5);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService001 end.");
}

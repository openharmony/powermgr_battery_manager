/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#ifdef GTEST
#define private   public
#define protected public
#endif

#include <iostream>
#include <string>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_publish_info.h"
#include "common_event_support.h"
#include "battery_notify.h"
#include "battery_log.h"
#include "test_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
shared_ptr<BatteryNotify> g_batteryNotify = nullptr;
} // namespace

void BatteryEventTest::SetUpTestCase()
{
    if (g_batteryNotify == nullptr) {
        g_batteryNotify = make_shared<BatteryNotify>();
    }
}

void BatteryEventTest::TearDownTestCase()
{
    if (g_batteryNotify != nullptr) {
        g_batteryNotify.reset();
        g_batteryNotify = nullptr;
    }
}
/**
 * @tc.name: BatteryEventTest001
 * @tc.desc: test PublishLowEvent function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTest, BatteryEventTest001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest001 function start!");
    BatteryInfo info;
    bool ret = g_batteryNotify->PublishLowEvent(info);
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest001 function end!");
}

/**
 * @tc.name: BatteryEventTest002
 * @tc.desc: test PublishCustomEvent function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTest, BatteryEventTest002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest002 function start!");
    BatteryInfo info;
    bool ret = g_batteryNotify->PublishCustomEvent(info, "test.battery.custom.event");
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest002 function end!");
}

/**
 * @tc.name: BatteryEventTest003
 * @tc.desc: test HandleNotification function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTest, BatteryEventTest003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest003 function start!");
    std::string popupName = "popup_test003";
    bool ret = g_batteryNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest003 function end!");
}

/**
 * @tc.name: BatteryEventTest004
 * @tc.desc: test HandleNotification function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTest, BatteryEventTest004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest004 function start!");
    BatteryInfo info;
    std::string ueventName = "notification_test004";
    info.SetUevent(ueventName);
    bool ret =  g_batteryNotify->PublishChangedEvent(info);
    EXPECT_TRUE(ret);
    std::string popupName = "popup_test004";
    ret = g_batteryNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTest004 function end!");
}
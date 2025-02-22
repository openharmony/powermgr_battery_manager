/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "battery_notification_test.h"
#ifdef GTEST
#define private   public
#define protected public
#endif

#include "notification_manager.h"
#include "notification_locale.h"
#include "battery_notify.h"
#include <string>
#include <memory>
#include "battery_log.h"
using namespace testing::ext;

namespace {
std::shared_ptr<OHOS::PowerMgr::BatteryNotify> g_batteryServiceNotify = nullptr;
} // namespace

namespace OHOS {
namespace PowerMgr {

BatteryInfo* g_batteryServiceInfo;

void BatteryNotificationTest::SetUpTestCase()
{
    g_batteryServiceNotify = std::make_shared<BatteryNotify>();
}

void BatteryNotificationTest::SetUp()
{
    g_batteryServiceInfo = new BatteryInfo();
}

void BatteryNotificationTest::TearDown()
{
    if (g_batteryServiceInfo != nullptr) {
        delete g_batteryServiceInfo;
        g_batteryServiceInfo = nullptr;
    }
}

/**
 * @tc.name: BatteryNotification001
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification001 begin");
    g_batteryInfo->SetUevent("battery_notification_test001");
    EXPECT_EQ(g_batteryInfo->GetUevent(), "battery_notification_test001");
    std::string popupName = "BatteryNotification001_popup";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification001 end");
}

/**
 * @tc.name: BatteryNotification002
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification002 begin");
    std::string popupName = "BatteryNotification002_popup";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    std::shared_ptr<NotificationManager> notificationMgr = std::make_shared<NotificationManager>();
    EXPECT_TRUE(notificationMgr != nullptr);
    notificationMgr->HandleNotification(popupName, 0, {});
    notificationMgr->HandleNotification(popupName, 1, {});
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification002 end");
}

/**
 * @tc.name: BatteryNotification003
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification003 begin");
    std::string popupName = "battery_notification_test003";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    std::shared_ptr<NotificationManager> notificationMgr = std::make_shared<NotificationManager>();
    EXPECT_TRUE(notificationMgr != nullptr);
    BatteryConfig::NotificationConf nCfg;
    nCfg.name = "BatteryNotification003_popup";
    notificationMgr->PublishNotification(nCfg);
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 1);
    notificationMgr->CancleNotification("BatteryNotification003_popup");
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification003 end");
}

/**
 * @tc.name: BatteryNotification004
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification004 begin");
    std::string popupName = "battery_notification_test004";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    std::shared_ptr<NotificationManager> notificationMgr = std::make_shared<NotificationManager>();
    EXPECT_TRUE(notificationMgr != nullptr);
    BatteryConfig::NotificationConf nCfg {
        .name = "BatteryNotification004_popup",
        .icon = "/etc/battery/battery_icon.png",
        .text = "test",
        .title = "test",
        .firstButton = std::make_pair("", ""),
        .secondButton = std::make_pair("", ""),
    };
    notificationMgr->PublishNotification(nCfg);
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 1);
    notificationMgr->CancleNotification("BatteryNotification004_popup");
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification004 end");
}

/**
 * @tc.name: BatteryNotification005
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification005 begin");
    std::string popupName = "battery_notification_test005";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    std::shared_ptr<NotificationManager> notificationMgr = std::make_shared<NotificationManager>();
    EXPECT_TRUE(notificationMgr != nullptr);
    BatteryConfig::NotificationConf nCfg {
        .name = "BatteryNotification005_popup",
        .icon = "/etc/battery/battery_icon.png",
        .text = "test005",
        .title = "test",
        .firstButton = std::make_pair("Enable", "notification.battery.test_enable"),
        .secondButton = std::make_pair("", ""),
    };
    notificationMgr->PublishNotification(nCfg);
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 1);
    notificationMgr->CancleNotification("BatteryNotification005_popup");
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification005 end");
}

/**
 * @tc.name: BatteryNotification006
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotificationTest, BatteryNotification006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification006 begin");
    std::string popupName = "battery_notification_test006";
    bool ret = g_batteryServiceNotify->HandleNotification(popupName);
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    std::shared_ptr<NotificationManager> notificationMgr = std::make_shared<NotificationManager>();
    EXPECT_TRUE(notificationMgr != nullptr);
    BatteryConfig::NotificationConf nCfg {
        .name = "BatteryNotification006_popup",
        .icon = "/etc/battery/battery_icon.png",
        .text = "test006",
        .title = "test",
        .firstButton = std::make_pair("Enable", "notification.battery.test_enable"),
        .secondButton = std::make_pair("Disable", "notification.battery.test_disable"),
    };
    notificationMgr->PublishNotification(nCfg);
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 1);
    notificationMgr->CancleNotification("BatteryNotification006_popup");
    EXPECT_TRUE(notificationMgr->notificationMap_.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotification006 end");
}
}
}
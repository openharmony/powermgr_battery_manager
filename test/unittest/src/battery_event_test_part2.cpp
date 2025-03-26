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

#ifdef GTEST
#define private   public
#define protected public
#endif
#include <iostream>
#include <string>

#include "battery_notify.h"
#include "battery_log.h"
#include "test_utils.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mock_common_event_manager.h"
#include "mock_sa_manager.h"
#include "mock_remote_object.h"
#include "iremote_object.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
shared_ptr<BatteryNotify> g_batteryNotify = nullptr;
const int32_t COMMON_EVENT_SERVICE_ID = 3299;
bool g_commonEventInitSuccess = false;
} // namespace

namespace OHOS {
namespace PowerMgr {
bool BatteryNotify::IsCommonEventServiceAbilityExist() const
{
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sysMgr) {
        BATTERY_HILOGE(COMP_SVC,
            "IsCommonEventServiceAbilityExist Get ISystemAbilityManager failed, no SystemAbilityManager");
        return false;
    }
    sptr<IRemoteObject> remote = sysMgr->CheckSystemAbility(COMMON_EVENT_SERVICE_ID);
    if (!remote) {
        BATTERY_HILOGE(COMP_SVC, "No CesServiceAbility");
        return false;
    }

    if (!g_commonEventInitSuccess) {
        BATTERY_HILOGI(COMP_SVC, "common event service ability init success");
        g_commonEventInitSuccess = true;
    }

    return true;
}
class BatteryEventTestPart2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    MockSystemAbilityManager mockSam;
    SystemAbilityManagerClient& mockClient = SystemAbilityManagerClient::GetInstance();
};
} // namespace PowerMgr
} // namespace OHOS

void BatteryEventTestPart2::SetUpTestCase()
{
    if (g_batteryNotify == nullptr) {
        g_batteryNotify = make_shared<BatteryNotify>();
    }
}

void BatteryEventTestPart2::TearDownTestCase()
{
    if (g_batteryNotify != nullptr) {
        g_batteryNotify.reset();
        g_batteryNotify = nullptr;
    }
}

/**
 * @tc.name: BatteryEventTestPart2001
 * @tc.desc: test PublishLowEvent function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTestPart2, BatteryEventTestPart2001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2001 start.");
    BatteryInfo info;
    MockBatteryCommonEventManager::SetBoolReturnValue(true);
    bool ret = g_batteryNotify->PublishLowEvent(info);
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2001 end.");
}

/**
 * @tc.name: BatteryEventTestPart2002
 * @tc.desc: test PublishPowerConnectedEvent function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTestPart2, BatteryEventTestPart2002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2002 start.");
    BatteryInfo info;
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(true);
    bool ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_FALSE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_BUTT);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(true);
    ret = g_batteryNotify->PublishPowerConnectedEvent(info);
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2002 end.");
}

/**
 * @tc.name: BatteryEventTestPart2003
 * @tc.desc: test PublishPowerDisconnectedEvent function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTestPart2, BatteryEventTestPart2003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2003 start.");
    BatteryInfo info;
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    MockBatteryCommonEventManager::SetBoolReturnValue(true);
    bool ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_FALSE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_USB);
    MockBatteryCommonEventManager::SetBoolReturnValue(false);
    ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_TRUE(ret);
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_BUTT);
    MockBatteryCommonEventManager::SetBoolReturnValue(true);
    ret = g_batteryNotify->PublishPowerDisconnectedEvent(info);
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2003 end.");
}

/**
 * @tc.name: BatteryEventTestPart2004
 * @tc.desc: test PublishEvents function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTestPart2, BatteryEventTestPart2004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2004 start.");
    BatteryInfo info;
    info.SetUevent("InitBatteryInfo");
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_NONE);
    EXPECT_CALL(mockClient, GetSystemAbilityManager())
        .WillRepeatedly(testing::Return(nullptr));
    int32_t ret = g_batteryNotify->PublishEvents(info);
    EXPECT_TRUE(ret == ERR_NO_INIT);

    sptr<IRemoteObject> mockRemote = new MockRemoteObject(u"BatteryEventTestPart2004");
    EXPECT_CALL(mockClient, GetSystemAbilityManager())
        .WillRepeatedly(testing::Return(&mockSam));
    EXPECT_CALL(mockSam, CheckSystemAbility(COMMON_EVENT_SERVICE_ID))
        .WillOnce(testing::Return(mockRemote));
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_BUTT);
    ret = g_batteryNotify->PublishEvents(info);
    EXPECT_TRUE(ret == ERR_OK);

    EXPECT_CALL(mockSam, CheckSystemAbility(COMMON_EVENT_SERVICE_ID))
        .WillOnce(testing::Return(mockRemote));
    info.SetPluggedType(BatteryPluggedType::PLUGGED_TYPE_AC);
    ret = g_batteryNotify->PublishEvents(info);
    EXPECT_TRUE(ret == ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2004 end.");
}

/**
 * @tc.name: BatteryEventTestPart2005
 * @tc.desc: test IsCommonEventServiceAbilityExist function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryEventTestPart2, BatteryEventTestPart2005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2005 start.");
    EXPECT_CALL(mockClient, GetSystemAbilityManager())
        .WillRepeatedly(testing::Return(nullptr));
    bool result = g_batteryNotify->IsCommonEventServiceAbilityExist();
    EXPECT_FALSE(result);

    EXPECT_CALL(mockClient, GetSystemAbilityManager())
        .WillRepeatedly(testing::Return(&mockSam));
    EXPECT_CALL(mockSam, CheckSystemAbility(COMMON_EVENT_SERVICE_ID))
        .WillOnce(testing::Return(nullptr));
    result = g_batteryNotify->IsCommonEventServiceAbilityExist();
    EXPECT_FALSE(result);

    sptr<IRemoteObject> mockRemote = new MockRemoteObject(u"BatteryEventTestPart2005");
    EXPECT_CALL(mockClient, GetSystemAbilityManager())
        .WillOnce(testing::Return(&mockSam));
    EXPECT_CALL(mockSam, CheckSystemAbility(COMMON_EVENT_SERVICE_ID))
        .WillOnce(testing::Return(mockRemote));
    result = g_batteryNotify->IsCommonEventServiceAbilityExist();
    EXPECT_TRUE(result);
    testing::Mock::AllowLeak(&mockClient);
    BATTERY_HILOGI(LABEL_TEST, "BatteryEventTestPart2005 end.");
}

/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "battery_event_system_test.h"

#include "battery_info.h"
#include "battery_srv_client.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "power_common.h"
#include "securec.h"
#include "test_utils.h"
#include <condition_variable>
#include <datetime_ex.h>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <if_system_ability_manager.h>
#include <iostream>
#include <ipc_skeleton.h>
#include <mutex>
#include <string_ex.h>
#include <unistd.h>

using namespace testing::ext;
using namespace std;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::EventFwk;
using namespace OHOS::PowerMgr;

namespace {
std::condition_variable g_cv;
std::mutex g_mtx;
std::string g_action = "";
int32_t g_capacity = -1;
int32_t g_chargeState = -1;
int32_t g_capacityLevel = -1;
constexpr int64_t TIME_OUT = 1;
bool g_isMock = false;
const int32_t RETRY_TIMES = 2;
const std::string MOCK_BATTERY_PATH = "/data/service/el0/battery/";
const std::string KEY_CAPACITY = BatteryInfo::COMMON_EVENT_KEY_CAPACITY;
const std::string KEY_CAPACITY_LEVEL = BatteryInfo::COMMON_EVENT_KEY_CAPACITY_LEVEL;
const std::string KEY_CHARGE_STATE = BatteryInfo::COMMON_EVENT_KEY_CHARGE_STATE;
const std::string KEY_PLUGGED_MAX_VOLTAGE = BatteryInfo::COMMON_EVENT_KEY_PLUGGED_MAX_VOLTAGE;
} // namespace

class CommonEventBatteryChangedTest : public CommonEventSubscriber {
public:
    CommonEventBatteryChangedTest() = default;
    explicit CommonEventBatteryChangedTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryChangedTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryChangedTest> RegisterEvent();
};

CommonEventBatteryChangedTest::CommonEventBatteryChangedTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryLowTest : public CommonEventSubscriber {
public:
    CommonEventBatteryLowTest() = default;
    explicit CommonEventBatteryLowTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryLowTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryLowTest> RegisterEvent();
};

CommonEventBatteryLowTest::CommonEventBatteryLowTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryOkayTest : public CommonEventSubscriber {
public:
    CommonEventBatteryOkayTest() = default;
    explicit CommonEventBatteryOkayTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryOkayTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryOkayTest> RegisterEvent();
};

CommonEventBatteryOkayTest::CommonEventBatteryOkayTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryChargingTest : public CommonEventSubscriber {
public:
    CommonEventBatteryChargingTest() = default;
    explicit CommonEventBatteryChargingTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryChargingTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryChargingTest> RegisterEvent();
};

CommonEventBatteryChargingTest::CommonEventBatteryChargingTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryDischargingTest : public CommonEventSubscriber {
public:
    CommonEventBatteryDischargingTest() = default;
    explicit CommonEventBatteryDischargingTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryDischargingTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryDischargingTest> RegisterEvent();
};

CommonEventBatteryDischargingTest::CommonEventBatteryDischargingTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryDisconnectTest : public CommonEventSubscriber {
public:
    CommonEventBatteryDisconnectTest() = default;
    explicit CommonEventBatteryDisconnectTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryDisconnectTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryDisconnectTest> RegisterEvent();
};

CommonEventBatteryDisconnectTest::CommonEventBatteryDisconnectTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventBatteryConnectTest : public CommonEventSubscriber {
public:
    CommonEventBatteryConnectTest() = default;
    explicit CommonEventBatteryConnectTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventBatteryConnectTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventBatteryConnectTest> RegisterEvent();
};

CommonEventBatteryConnectTest::CommonEventBatteryConnectTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventChargeTypeChangedTest : public CommonEventSubscriber {
public:
    CommonEventChargeTypeChangedTest() = default;
    explicit CommonEventChargeTypeChangedTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventChargeTypeChangedTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventChargeTypeChangedTest> RegisterEvent();
};

CommonEventChargeTypeChangedTest::CommonEventChargeTypeChangedTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

class CommonEventDumpCapacityTest : public CommonEventSubscriber {
public:
    CommonEventDumpCapacityTest() = default;
    explicit CommonEventDumpCapacityTest(const CommonEventSubscribeInfo& subscriberInfo);
    virtual ~CommonEventDumpCapacityTest() {};
    virtual void OnReceiveEvent(const CommonEventData& data);
    static shared_ptr<CommonEventDumpCapacityTest> RegisterEvent();
};

CommonEventDumpCapacityTest::CommonEventDumpCapacityTest(const CommonEventSubscribeInfo& subscriberInfo)
    : CommonEventSubscriber(subscriberInfo)
{
}

void CommonEventBatteryChangedTest::OnReceiveEvent(const CommonEventData& data)
{
    g_action = data.GetWant().GetAction();
    g_cv.notify_one();
}

void CommonEventBatteryLowTest::OnReceiveEvent(const CommonEventData& data)
{
    g_action = data.GetWant().GetAction();
    g_cv.notify_one();
}

void CommonEventBatteryOkayTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultCapacity = -1;
    int capacity = data.GetWant().GetIntParam(KEY_CAPACITY, defaultCapacity);
    g_cv.notify_one();
    EXPECT_EQ(capacity, static_cast<int32_t>(BatteryCapacityLevel::LEVEL_HIGH)) << "COMMON_EVENT_BATTERY_OKAY";
}

void CommonEventBatteryChargingTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultChargeState = -1;
    int chargeState = data.GetWant().GetIntParam(KEY_CHARGE_STATE, defaultChargeState);
    g_cv.notify_one();
    EXPECT_EQ(chargeState, static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_ENABLE)) << "COMMON_EVENT_CHARGING";
}

void CommonEventBatteryDischargingTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultChargeState = -1;
    int chargeState = data.GetWant().GetIntParam(KEY_CHARGE_STATE, defaultChargeState);
    g_cv.notify_one();
    EXPECT_EQ(chargeState, static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_NONE)) << "COMMON_EVENT_DISCHARGING";
}

void CommonEventBatteryDisconnectTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultMaxVoltage = -1;
    int maxVoltage = data.GetWant().GetIntParam(KEY_PLUGGED_MAX_VOLTAGE, defaultMaxVoltage);
    g_cv.notify_one();
    EXPECT_NE(maxVoltage, static_cast<int32_t>(BatteryPluggedType::PLUGGED_TYPE_NONE))
        << "COMMON_EVENT_POWER_DISCONNECTED";
}

void CommonEventBatteryConnectTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultMaxVoltage = -1;
    int maxVoltage = data.GetWant().GetIntParam(KEY_PLUGGED_MAX_VOLTAGE, defaultMaxVoltage);
    g_cv.notify_one();
    EXPECT_NE(maxVoltage, static_cast<int32_t>(BatteryPluggedType::PLUGGED_TYPE_USB)) << "COMMON_EVENT_POWER_CONNECTED";
}

void CommonEventChargeTypeChangedTest::OnReceiveEvent(const CommonEventData& data)
{
    g_action = data.GetWant().GetAction();
    g_cv.notify_one();
}

void CommonEventDumpCapacityTest::OnReceiveEvent(const CommonEventData& data)
{
    int defaultCapacity = -1;
    int defaultCapacityLevel = -1;
    int defaultChargeState = -1;
    g_capacity = data.GetWant().GetIntParam(KEY_CAPACITY, defaultCapacity);
    g_capacityLevel = data.GetWant().GetIntParam(KEY_CAPACITY_LEVEL, defaultCapacityLevel);
    g_chargeState = data.GetWant().GetIntParam(KEY_CHARGE_STATE, defaultChargeState);
    g_cv.notify_one();
}

shared_ptr<CommonEventBatteryChangedTest> CommonEventBatteryChangedTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryChangedTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryLowTest> CommonEventBatteryLowTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BATTERY_LOW);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryLowTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryOkayTest> CommonEventBatteryOkayTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BATTERY_OKAY);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryOkayTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryChargingTest> CommonEventBatteryChargingTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_CHARGING);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryChargingTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryDischargingTest> CommonEventBatteryDischargingTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_DISCHARGING);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryDischargingTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryDisconnectTest> CommonEventBatteryDisconnectTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryDisconnectTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventBatteryConnectTest> CommonEventBatteryConnectTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_POWER_CONNECTED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventBatteryConnectTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventChargeTypeChangedTest> CommonEventChargeTypeChangedTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_CHARGE_TYPE_CHANGED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventChargeTypeChangedTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

shared_ptr<CommonEventDumpCapacityTest> CommonEventDumpCapacityTest::RegisterEvent()
{
    bool succeed = false;
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    auto subscriberPtr = std::make_shared<CommonEventDumpCapacityTest>(subscribeInfo);
    for (int32_t tryTimes = 0; tryTimes < RETRY_TIMES; tryTimes++) {
        succeed = CommonEventManager::SubscribeCommonEvent(subscriberPtr);
    }
    if (!succeed) {
        return nullptr;
    }
    return subscriberPtr;
}

void BatteryEventSystemTest::SetUpTestCase(void)
{
    g_isMock = TestUtils::IsMock();
    GTEST_LOG_(INFO) << " g_isMock: " << g_isMock;
}

void BatteryEventSystemTest::TearDownTestCase(void)
{
    g_isMock = false;
    TestUtils::ResetOnline();
}

namespace {

/*
 * @tc.number: BatteryEventSystemTest001
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest001, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryLowTest> subscriber = CommonEventBatteryLowTest::RegisterEvent();
    if (g_isMock) {
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "5");
        system("hidumper -s 3302 -a -r");
        std::unique_lock<std::mutex> lck(g_mtx);
        if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
            g_cv.notify_one();
        }
        EXPECT_EQ(CommonEventSupport::COMMON_EVENT_BATTERY_LOW, g_action);
    }
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
}

/*
 * @tc.number: BatteryEventSystemTest002
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest002, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryChangedTest> subscriber = CommonEventBatteryChangedTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "40");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    EXPECT_EQ(CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED, g_action);
    CommonEventManager::UnSubscribeCommonEvent(subscriber);
}

/*
 * @tc.number: BatteryEventSystemTest003
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest003, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryChargingTest> subscriber = CommonEventBatteryChargingTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/type", "Charging");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest004
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest004, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryDischargingTest> subscriber = CommonEventBatteryDischargingTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/type", "DisCharging");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest005
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest005, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryOkayTest> subscriber = CommonEventBatteryOkayTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "90");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest006
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest006, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryDisconnectTest> subscriber = CommonEventBatteryDisconnectTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "Disconnect");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest007
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest007, TestSize.Level0)
{
    shared_ptr<CommonEventBatteryConnectTest> subscriber = CommonEventBatteryConnectTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest008
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Verify the receive the common event
 * @tc.require: issueI6KRS8
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest008, TestSize.Level0)
{
    shared_ptr<CommonEventChargeTypeChangedTest> subscriber = CommonEventChargeTypeChangedTest::RegisterEvent();
    TestUtils::WriteMock(MOCK_BATTERY_PATH + "/charge_type", "1");
    system("hidumper -s 3302 -a -r");
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}

/*
 * @tc.number: BatteryEventSystemTest009
 * @tc.name: BatteryEventSystemTest
 * @tc.desc: Test capacity and unplugged dump, verify the receive the common event
 * @tc.require: issueI6Z8RB
 */
HWTEST_F(BatteryEventSystemTest, BatteryEventSystemTest009, TestSize.Level0)
{
    shared_ptr<CommonEventDumpCapacityTest> subscriber = CommonEventDumpCapacityTest::RegisterEvent();
    int32_t capacity = 2;
    std::string baseCmdStr = "hidumper -s 3302 -a";
    std::string cmdStr = baseCmdStr;
    cmdStr.append(" \"--capacity ").append(ToString(capacity)).append("\"");
    system(cmdStr.c_str());
    std::unique_lock<std::mutex> lck(g_mtx);
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    EXPECT_EQ(g_capacity, capacity);
    EXPECT_EQ(capacity, BatterySrvClient::GetInstance().GetCapacity());
    EXPECT_EQ(g_capacityLevel, static_cast<int32_t>(BatteryCapacityLevel::LEVEL_CRITICAL));
    EXPECT_TRUE(BatteryCapacityLevel::LEVEL_CRITICAL == BatterySrvClient::GetInstance().GetCapacityLevel());

    system("hidumper -s 3302 -a -u");
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    EXPECT_EQ(g_chargeState, static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_NONE));
    EXPECT_TRUE(BatteryPluggedType::PLUGGED_TYPE_NONE == BatterySrvClient::GetInstance().GetPluggedType());
    EXPECT_TRUE(BatteryChargeState::CHARGE_STATE_NONE == BatterySrvClient::GetInstance().GetChargingStatus());
    EXPECT_EQ(g_capacity, capacity);
    EXPECT_EQ(capacity, BatterySrvClient::GetInstance().GetCapacity());

    capacity = 91;
    cmdStr = baseCmdStr;
    cmdStr.append(" \"--capacity ").append(ToString(capacity)).append("\"");
    system(cmdStr.c_str());
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    EXPECT_EQ(g_capacity, capacity);
    EXPECT_EQ(capacity, BatterySrvClient::GetInstance().GetCapacity());
    EXPECT_EQ(g_capacityLevel, static_cast<int32_t>(BatteryCapacityLevel::LEVEL_HIGH));
    EXPECT_TRUE(BatteryCapacityLevel::LEVEL_HIGH == BatterySrvClient::GetInstance().GetCapacityLevel());
    EXPECT_EQ(g_chargeState, static_cast<int32_t>(BatteryChargeState::CHARGE_STATE_NONE));
    EXPECT_TRUE(BatteryPluggedType::PLUGGED_TYPE_NONE == BatterySrvClient::GetInstance().GetPluggedType());
    EXPECT_TRUE(BatteryChargeState::CHARGE_STATE_NONE == BatterySrvClient::GetInstance().GetChargingStatus());

    system("hidumper -s 3302 -a -r");
    if (g_cv.wait_for(lck, std::chrono::seconds(TIME_OUT)) == std::cv_status::timeout) {
        g_cv.notify_one();
    }
    EXPECT_EQ(g_capacity, BatterySrvClient::GetInstance().GetCapacity());
    EXPECT_EQ(g_chargeState, static_cast<int32_t>(BatterySrvClient::GetInstance().GetChargingStatus()));
    auto ret = CommonEventManager::UnSubscribeCommonEvent(subscriber);
    EXPECT_TRUE(ret);
}
} // namespace

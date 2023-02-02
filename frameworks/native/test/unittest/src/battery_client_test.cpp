/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_client_test.h"

#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "battery_log.h"
#include "battery_srv_client.h"
#include "test_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
bool g_isMock = false;
constexpr int32_t BATTERY_EMERGENCY_THRESHOLD = 5;
constexpr int32_t BATTERY_LOW_THRESHOLD = 20;
constexpr int32_t BATTERY_NORMAL_THRESHOLD = 90;
constexpr int32_t BATTERY_HIGH_THRESHOLD = 99;
constexpr int32_t BATTERY_HIGH_FULL = 100;
const std::string MOCK_BATTERY_PATH = "/data/service/el0/battery/";
}

void BatteryClientTest::SetUpTestCase(void)
{
    g_isMock = TestUtils::IsMock();
    GTEST_LOG_(INFO) << " g_isMock: " << g_isMock;
}

void BatteryClientTest::TearDownTestCase(void)
{
    g_isMock = false;
    TestUtils::ResetOnline();
}

void BatteryClientTest::SetUp(void)
{
}

void BatteryClientTest::TearDown(void)
{
}

namespace {
/**
 * @tc.name: BatteryClient001
 * @tc.desc: Test IBatterySrv interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient001 start.");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "22");
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::capacity=%{public}d", capacity);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient001 executing, capacity=" << capacity;
        ASSERT_TRUE(capacity == 22);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::capacity=%{public}d", capacity);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient001 executing, capacity=" << capacity;
        ASSERT_TRUE(capacity <= 100 && capacity >= 0);
    }

    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient001 end.");
}

/**
 * @tc.name: BatteryClient002
 * @tc.desc: Test IBatterySrv interface GetChargingStatus
 * @tc.type: FUNC
 */

HWTEST_F (BatteryClientTest, BatteryClient002, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Not charging");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient002 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
            chargeStateArr[static_cast<int32_t>(tempChargeState)]);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient002 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient002 end.");
}

/**
 * @tc.name: BatteryClient003
 * @tc.desc: Test IBatterySrv interface GetHealthStatus
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient003, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient003 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Cold");
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient003 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
            healthStateArr[static_cast<int32_t>(tempHealthState)]);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient003 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient003 end.");
}

/**
 * @tc.name: BatteryClient004
 * @tc.desc: Test IBatterySrv interface GetPresent
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient004, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient004 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempPresent = BatterySrvClient.GetPresent();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", "0");
        auto present = BatterySrvClient.GetPresent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient004 executing, present=" << present;
        ASSERT_FALSE(present);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", std::to_string(tempPresent));
    } else {
        auto present = BatterySrvClient.GetPresent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient004 executing, present=" << present;
        ASSERT_TRUE(present);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient004 end.");
}

/**
 * @tc.name: BatteryClient005
 * @tc.desc: Test IBatterySrv interface GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient005, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient005 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempVoltage = BatterySrvClient.GetVoltage();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");
        auto voltage = BatterySrvClient.GetVoltage();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient005 executing, voltage=" << voltage;
        ASSERT_TRUE(voltage == 4654321);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", std::to_string(tempVoltage));
    } else {
        auto voltage = BatterySrvClient.GetVoltage();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient005 executing, voltage=" << voltage;
        ASSERT_TRUE(voltage >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient005 end.");
}

/**
 * @tc.name: BatteryClient006
 * @tc.desc: Test IBatterySrv interface GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient006, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient006 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempTempPresent = BatterySrvClient.GetBatteryTemperature();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", "222");
        auto temperature = BatterySrvClient.GetBatteryTemperature();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient006 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature == 222);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", std::to_string(tempTempPresent));
    } else {
        auto temperature = BatterySrvClient.GetBatteryTemperature();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient006 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature >= 0 && temperature <= 600);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient006 end.");
}

/**
 * @tc.name: BatteryClient007
 * @tc.desc: Test IBatterySrv interface GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient007, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient007 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        std::string tempTechnology = BatterySrvClient.GetTechnology();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos-fgu/technology", "H2");
        auto technology = BatterySrvClient.GetTechnology();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::technology=%{public}s", technology.c_str());
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient007 executing, technology=" << technology;
        ASSERT_TRUE(technology == "H2");

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos-fgu/technology", tempTechnology);
    } else {
        auto technology = BatterySrvClient.GetTechnology();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::technology=%{public}s", technology.c_str());
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient007 executing, technology=" << technology;
        ASSERT_TRUE(technology == "Li-poly");
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient007 end.");
}

/**
 * @tc.name: BatteryClient008
 * @tc.desc: Test IBatterySrv interface GetPluggedType
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient008, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient008 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB");
        auto pluggedType = BatterySrvClient.GetPluggedType();
        
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient008 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient008 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient008 end.");
}

/**
 * @tc.name: BatteryClient009
 * @tc.desc: Test IBatterySrv interface GetCurrentNow
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient009, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient009 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCurrnow = BatterySrvClient.GetNowCurrent();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/current_now", "4654321");
        auto currnow = BatterySrvClient.GetNowCurrent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::current=%{public}d", currnow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient009 executing, currnow=" << currnow;
        ASSERT_EQ(currnow, 4654321);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/current_now", std::to_string(tempCurrnow));
    } else {
        auto currnow = BatterySrvClient.GetNowCurrent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::currnow=%{public}d", currnow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient009 executing, currnow=" << currnow;
        ASSERT_TRUE(currnow >= -20000 && currnow <= 20000);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient009 end.");
}

/**
 * @tc.name: BatteryClient010
 * @tc.desc: Test IBatterySrv interface GetRemainEnergy
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient010, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient010 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargenow = BatterySrvClient.GetRemainEnergy();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/charge_now", "4654321");
        auto chargenow = BatterySrvClient.GetRemainEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargenow=%{public}d", chargenow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient010 executing, chargenow=" << chargenow;
        ASSERT_EQ(chargenow, 4654321);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/charge_now", std::to_string(tempChargenow));
    } else {
        auto chargenow = BatterySrvClient.GetRemainEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargenow=%{public}d", chargenow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient010 executing, chargenow=" << chargenow;
        ASSERT_TRUE(chargenow >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient010 end.");
}

/**
 * @tc.name: BatteryClient011
 * @tc.desc: Test IBatterySrv interface GetTotalEnergy
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient011, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient011 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempTotalenergy = BatterySrvClient.GetTotalEnergy();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/charge_full", "4654321");
        auto totalenergy = BatterySrvClient.GetTotalEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::totalenergy=%{public}d", totalenergy);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient011 executing, totalenergy=" << totalenergy;
        ASSERT_EQ(totalenergy, 4654321);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/charge_full", std::to_string(tempTotalenergy));
    } else {
        auto totalenergy = BatterySrvClient.GetTotalEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::totalenergy=%{public}d", totalenergy);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient011 executing, totalenergy=" << totalenergy;
        ASSERT_TRUE(totalenergy >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient011 end.");
}

/**
 * @tc.name: BatteryClient012
 * @tc.desc: Test IBatterySrv interface GetCapacityLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient012, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient012 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "3");
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient012 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_CRITICAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient012 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        if (capacity < BATTERY_EMERGENCY_THRESHOLD) {
            ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_CRITICAL);
        }
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient012 end.");
}

/**
 * @tc.name: BatteryClient013
 * @tc.desc: Test IBatterySrv interface GetCapacityLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient013, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient013 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "11");
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient013 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_LOW);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient013 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        if (capacity >= BATTERY_EMERGENCY_THRESHOLD && capacity <= BATTERY_LOW_THRESHOLD) {
            ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_LOW);
        }
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient013 end.");
}

/**
 * @tc.name: BatteryClient014
 * @tc.desc: Test IBatterySrv interface GetCapacityLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient014, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient014 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "80");
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient014 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NORMAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient014 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        if (capacity > BATTERY_LOW_THRESHOLD && capacity <= BATTERY_NORMAL_THRESHOLD) {
            ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NORMAL);
        }
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient014 end.");
}

/**
 * @tc.name: BatteryClient015
 * @tc.desc: Test IBatterySrv interface GetCapacityLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient015, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient015 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "94");
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient015 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_HIGH);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient015 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        if (capacity > BATTERY_NORMAL_THRESHOLD && capacity <= BATTERY_HIGH_THRESHOLD) {
            ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_HIGH);
        }
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient015 end.");
}

/**
 * @tc.name: BatteryClient016
 * @tc.desc: Test IBatterySrv interface GetCapacityLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient016, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient016 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "100");
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
           static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient016 executing, batterylevel="
           << static_cast<int32_t>(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_FULL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
    } else {
        auto batterylevel = BatterySrvClient.GetCapacityLevel();
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d",
            static_cast<int32_t>(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient016 executing, batterylevel="
            << static_cast<int32_t>(batterylevel);
        if (capacity == BATTERY_HIGH_FULL) {
            ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_FULL);
        }
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient016 end.");
}


/**
 * @tc.name: BatteryClient017
 * @tc.desc: Test IBatterySrv interface GetRemainingChargeTime
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient017, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient017 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto remainingChargeTime = BatterySrvClient.GetRemainingChargeTime();
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient017 executing, remainingChargeTime=" << remainingChargeTime;
        ASSERT_TRUE(remainingChargeTime >= 0);
    } else {
        auto remainingChargeTime = BatterySrvClient.GetRemainingChargeTime();
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient017 executing, remainingChargeTime=" << remainingChargeTime;
        ASSERT_TRUE(remainingChargeTime >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient017 end.");
}

/**
 * @tc.name: BatteryClient018
 * @tc.desc: Test BatteryInfo operator== and operator!=
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient018, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient018 start.");
    BatteryInfo info1;
    BatteryInfo info2;
    ASSERT_TRUE(info1 == info2);

    info1.SetCapacity(100);
    info2.SetCapacity(0);
    ASSERT_TRUE(info1 != info2);

    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient018 end.");
}
}
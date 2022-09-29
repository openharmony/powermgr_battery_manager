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

#include "battery_sttest.h"

#include <csignal>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <streambuf>
#include <cstring>
#include <thread>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#include "power_supply_provider.h"
#include "battery_service.h"
#include "hdf_device_desc.h"
#include "battery_log.h"
#include "battery_srv_client.h"
#include "battery_thread_test.h"
#include "animation_label.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace std;

namespace {
const std::string MOCK_BATTERY_PATH = "/data/service/el0/battery/";
bool g_isMock = false;
}

void BatterySttest::SetUpTestCase(void)
{
    g_isMock = TestUtils::IsMock();
    GTEST_LOG_(INFO) << " g_isMock: " << g_isMock;
}

void BatterySttest::TearDownTestCase(void)
{
    g_isMock = false;
    TestUtils::ResetOnline();
}

void BatterySttest::SetUp(void)
{
}

void BatterySttest::TearDown(void)
{
}

namespace {
/**
 * @tc.name: BatteryST001
 * @tc.desc: Test IBatterySrv interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST001 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempCapacity = BatterySrvClient.GetCapacity();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "44");
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST001::capacity=%{public}d.", capacity);
        GTEST_LOG_(INFO) << "BatteryST001 executing, capacity=" << capacity;
        ASSERT_TRUE(capacity == 44);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", std::to_string(tempCapacity));
        GTEST_LOG_(INFO) << "BatteryST001 executing, capacity=" << tempCapacity;
    } else {
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST001::capacity=%{public}d", capacity);
        GTEST_LOG_(INFO) << "BatteryST001 executing, capacity=" << capacity;
        ASSERT_TRUE(capacity <= 100 && capacity >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST001 end.");
}

/**
 * @tc.name: BatteryST002
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is none
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryST002 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Discharging");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST002::capacity=%{public}d.", chargeState);
        GTEST_LOG_(INFO) << "BatteryST002 executing, chargeState=" << static_cast<int32_t>(chargeState);

        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_NONE);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
                chargeStateArr[static_cast<int32_t>(tempChargeState)]);
        GTEST_LOG_(INFO) << "BatteryST002 executing, chargeState=" << static_cast<int32_t>(tempChargeState);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST002::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryST002 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST002 end.");
}

/**
 * @tc.name: BatteryST003
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is enable
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST003, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST003 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Charging");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        GTEST_LOG_(INFO) << "BatteryST003 executing, chargeState=" << static_cast<int32_t>(chargeState);

        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
                chargeStateArr[static_cast<int32_t>(tempChargeState)]);
        GTEST_LOG_(INFO) << "BatteryST003 executing, chargeState=" << static_cast<int32_t>(tempChargeState);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryST003::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryST003 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST003 end.");
}

/**
 * @tc.name: BatteryST004
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is full
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST004, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "enter. BatteryST004 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Full");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        GTEST_LOG_(INFO) << "BatteryST004 executing, chargeState=" << static_cast<int32_t>(chargeState);

        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
                chargeStateArr[static_cast<int32_t>(tempChargeState)]);
        GTEST_LOG_(INFO) << "BatteryST004 executing, chargeState=" << static_cast<int32_t>(tempChargeState);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST004::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryST004 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST004 end.");
}

/**
 * @tc.name: BatteryST005
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is disable
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST005, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST005 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Not charging");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        GTEST_LOG_(INFO) << "BatteryST005 executing, chargeState=" << static_cast<int32_t>(chargeState);

        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
                chargeStateArr[static_cast<int32_t>(tempChargeState)]);
        GTEST_LOG_(INFO) << "BatteryST005 executing, chargeState=" << static_cast<int32_t>(tempChargeState);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST005::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryST005 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST005 end.");
}

/**
 * @tc.name: BatteryST006
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is butt
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST006, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST006 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempChargeState = BatterySrvClient.GetChargingStatus();
        std::string chargeStateArr[] = {"Discharging", "Charging", "Not charging", "Full", "Unknown"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status", "Unknown");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        GTEST_LOG_(INFO) << "BatteryST006 executing, chargeState=" << static_cast<int32_t>(chargeState);

        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/status",
                chargeStateArr[static_cast<int32_t>(tempChargeState)]);
        GTEST_LOG_(INFO) << "BatteryST006 executing, chargeState=" << static_cast<int32_t>(tempChargeState);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST006::chargeState=%{public}d",
            static_cast<int32_t>(chargeState));
        GTEST_LOG_(INFO) << "BatteryST006 executing, chargeState="
            << static_cast<int32_t>(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST006 end.");
}

/**
 * @tc.name: BatteryST007
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_GOOD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST007, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST007 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Good");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST007 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST007 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST007::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST007 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST007 end.");
}

/**
 * @tc.name: BatteryST008
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_COLD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST008, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST008 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Cold");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST008 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST008 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST008::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST008 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST008 end.");
}

/**
 * @tc.name: BatteryST009
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_OVERHEAT
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST009, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST009 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Hot");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST009 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERHEAT);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST009 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST009::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST009 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST009 end.");
}

/**
 * @tc.name: BatteryST010
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_OVERVOLTAGE
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST010, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST010 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Over voltage");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST010 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERVOLTAGE);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST010 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST010::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST010 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST010 end.");
}

/**
 * @tc.name: BatteryST011
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_DEAD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST011, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST011 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Dead");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST011 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_DEAD);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST011 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST011::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST011 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST011 end.");
}

/**
 * @tc.name: BatteryST012
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST012, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST012 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempHealthState = BatterySrvClient.GetHealthStatus();
        std::string healthStateArr[] = {"Unknown", "Good", "Hot", "Over voltage", "Cold", "Dead"};
        
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health", "Unknown");
        auto healthState = BatterySrvClient.GetHealthStatus();
        GTEST_LOG_(INFO) << "BatteryST012 executing, healthState=" << static_cast<int32_t>(healthState);

        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/health",
                healthStateArr[static_cast<int32_t>(tempHealthState)]);
        GTEST_LOG_(INFO) << "BatteryST012 executing, healthState=" << static_cast<int32_t>(tempHealthState);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST012::healthState=%{public}d",
            static_cast<int32_t>(healthState));
        GTEST_LOG_(INFO) << "BatteryST012 executing, healthState="
            << static_cast<int32_t>(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST012 end.");
}

/**
 * @tc.name: BatteryST013
 * @tc.desc: Test IBatterySrv interface GetPresent when present is true
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST013, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST013 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempPresent = BatterySrvClient.GetPresent();

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", "1");
        auto present = BatterySrvClient.GetPresent();
        GTEST_LOG_(INFO) << "BatteryST013 executing, present=" << present;

        ASSERT_TRUE(present);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", std::to_string(tempPresent));
        GTEST_LOG_(INFO) << "BatteryST013 executing, tempPresent=" << static_cast<int32_t>(tempPresent);
    } else {
        auto present = BatterySrvClient.GetPresent();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST013::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryST013 executing, present=" << present;
        ASSERT_TRUE(present);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST013 end.");
}

/**
 * @tc.name: BatteryST014
 * @tc.desc: Test IBatterySrv interface GetPresent when present is false
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST014, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST014 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempPresent = BatterySrvClient.GetPresent();

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", "0");
        auto present = BatterySrvClient.GetPresent();
        GTEST_LOG_(INFO) << "BatteryST014 executing, present=" << present;

        ASSERT_FALSE(present);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", std::to_string(tempPresent));
        GTEST_LOG_(INFO) << "BatteryST014 executing, tempPresent=" << static_cast<int32_t>(tempPresent);
    } else {
        auto present = BatterySrvClient.GetPresent();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST014::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryST014 executing, present=" << present;
        ASSERT_TRUE(present);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST014 end.");
}

/**
 * @tc.name: BatteryST015
 * @tc.desc: Test IBatterySrv interface GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST015, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST015 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempVoltagenow = BatterySrvClient.GetVoltage();

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");
        auto voltage = BatterySrvClient.GetVoltage();
        GTEST_LOG_(INFO) << "BatteryST015 executing, voltage=" << voltage;

        ASSERT_TRUE(voltage == 4654321);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", std::to_string(tempVoltagenow));
        GTEST_LOG_(INFO) << "BatteryST015 executing, tempPresent=" << static_cast<int32_t>(tempVoltagenow);
    } else {
        auto voltage = BatterySrvClient.GetVoltage();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST015::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryST015 executing, voltage=" << voltage;
        ASSERT_TRUE(voltage >= 0);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST015 end.");
}

/**
 * @tc.name: BatteryST016
 * @tc.desc: Test IBatterySrv interface GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST016, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST016 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        auto tempTempPresent = BatterySrvClient.GetBatteryTemperature();

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", "234");
        auto temperature = BatterySrvClient.GetBatteryTemperature();
        GTEST_LOG_(INFO) << "BatteryST016 executing, temperature=" << temperature;

        ASSERT_TRUE(temperature == 234);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", std::to_string(tempTempPresent));
        GTEST_LOG_(INFO) << "BatteryST016 executing, temperature=" << static_cast<int32_t>(tempTempPresent);
    } else {
        auto temperature = BatterySrvClient.GetBatteryTemperature();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST016::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryST016 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature >= 0 && temperature <= 600);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST016 end.");
}

/**
 * @tc.name: BatteryST017
 * @tc.desc: Test IBatterySrv interface GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST017, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST017 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        std::string tempTechnology = BatterySrvClient.GetTechnology();
    
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos-fgu/technology", "H2");
        auto technology = BatterySrvClient.GetTechnology();
        GTEST_LOG_(INFO) << "BatteryST017 executing, technology=" << technology;

        ASSERT_TRUE(technology == "H2");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos-fgu/technology", tempTechnology);
        GTEST_LOG_(INFO) << "BatteryST017 executing, technology=" << tempTechnology;
    } else {
        auto technology = BatterySrvClient.GetTechnology();
        BATTERY_HILOGD(LABEL_TEST, "BatteryST017::technology=%{public}s", technology.c_str());
        GTEST_LOG_(INFO) << "BatteryST017 executing, technology=" << technology;
        ASSERT_TRUE(technology == "Li-poly");
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST058 end.");
}

/**
 * @tc.name: BatteryST018
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_AC
 * @tc.type: FUNC
 */

HWTEST_F (BatterySttest, BatteryST018, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST018 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "Mains");

        auto pluggedType = BatterySrvClient.GetPluggedType();
        GTEST_LOG_(INFO) << "BatteryST018 executing, pluggedType=" << static_cast<int32_t>(pluggedType);

        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryST018::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryST018 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST018 end.");
}

/**
 * @tc.name: BatteryST019
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_USB
 * @tc.type: FUNC
 */

HWTEST_F (BatterySttest, BatteryST019, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST019 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB");

        auto pluggedType = BatterySrvClient.GetPluggedType();
        GTEST_LOG_(INFO) << "BatteryST019 executing, pluggedType=" << static_cast<int32_t>(pluggedType);

        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryST019::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryST019 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST019 end.");
}
/**
 * @tc.name: BatteryST020
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_WIRELESS
 * @tc.type: FUNC
 */

HWTEST_F (BatterySttest, BatteryST020, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST020 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "Wireless");

        auto pluggedType = BatterySrvClient.GetPluggedType();
        GTEST_LOG_(INFO) << "BatteryST020 executing, pluggedType=" << static_cast<int32_t>(pluggedType);

        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryST020::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryST020 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST020 end.");
}

/**
 * @tc.name: BatteryST021
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_BUTT
 *           ParsePluggedType return HDF_ERR_NOT_SUPPORT, g_service->GetPluggedType FAIL
 * @tc.type: FUNC
 */

HWTEST_F (BatterySttest, BatteryST021, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryST021 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/ohos_charger/type", "Unknown");

        auto pluggedType = BatterySrvClient.GetPluggedType();
        GTEST_LOG_(INFO) << "BatteryST021 executing, pluggedType=" << static_cast<int32_t>(pluggedType);

        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryST021::pluggedType=%{public}d",
            static_cast<int32_t>(pluggedType));
        GTEST_LOG_(INFO) << "BatteryST021 executing, pluggedType="
            << static_cast<int32_t>(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryST021 end.");
}
}

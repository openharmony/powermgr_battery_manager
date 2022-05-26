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

#include "battery_client_test.h"

#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "battery_log.h"
#include "battery_service.h"
#include "battery_srv_client.h"
#include "test_utils.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
bool g_isMock = false;
static sptr<BatteryService> g_service;
}

void BatteryClientTest::SetUpTestCase(void)
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
    auto& client = BatterySrvClient::GetInstance();
    GTEST_LOG_(INFO) << "is mock: " << client.GetPresent() << " g_isMock: " << g_isMock;
    if (!client.GetPresent()) {
        g_isMock = true;
        TestUtils::InitTest();
        g_service->ChangePath("/data/local/tmp");
    }
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
        TestUtils::WriteMock("/data/local/tmp/battery/capacity", "22");
        auto capacity = BatterySrvClient.GetCapacity();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::capacity=%{public}d", capacity);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient001 executing, capacity=" << capacity;
        ASSERT_TRUE(capacity == 22);
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
        TestUtils::WriteMock("/data/local/tmp/battery/status", "Not charging");
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargeState=%{public}d", int(chargeState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient002 executing, chargeState=" << int(chargeState);
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE);
    } else {
        auto chargeState = BatterySrvClient.GetChargingStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargeState=%{public}d", int(chargeState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient002 executing, chargeState=" << int(chargeState);
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
        TestUtils::WriteMock("/data/local/tmp/battery/health", "Cold");
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::healthState=%{public}d", int(healthState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient003 executing, healthState=" << int(healthState);
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD);
    } else {
        auto healthState = BatterySrvClient.GetHealthStatus();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::healthState=%{public}d", int(healthState));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient003 executing, healthState=" << int(healthState);
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
        TestUtils::WriteMock("/data/local/tmp/battery/present", "0");
        auto present = BatterySrvClient.GetPresent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient004 executing, present=" << present;
        ASSERT_FALSE(present);
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
        TestUtils::WriteMock("/data/local/tmp/battery/voltage_avg", "4654321");
        TestUtils::WriteMock("/data/local/tmp/battery/voltage_now", "4654321");
        auto voltage = BatterySrvClient.GetVoltage();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient005 executing, voltage=" << voltage;
        ASSERT_TRUE(voltage == 4654321);
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
        TestUtils::WriteMock("/data/local/tmp/battery/temp", "222");
        auto temperature = BatterySrvClient.GetBatteryTemperature();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient006 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature == 222);
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
        TestUtils::WriteMock("/data/local/tmp/ohos-fgu/technology", "H2");
        auto technology = BatterySrvClient.GetTechnology();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::technology=%{public}s", technology.c_str());
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient007 executing, technology=" << technology;
        ASSERT_TRUE(technology == "H2");
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
        TestUtils::WriteMock("/data/local/tmp/ohos_charger/online", "1");
        TestUtils::WriteMock("/data/local/tmp/ohos_charger/type", "USB");
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::pluggedType=%{public}d", int(pluggedType));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient008 executing, pluggedType=" << int(pluggedType);
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    } else {
        auto pluggedType = BatterySrvClient.GetPluggedType();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::pluggedType=%{public}d", int(pluggedType));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient008 executing, pluggedType=" << int(pluggedType);
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
        TestUtils::WriteMock("/data/local/tmp/battery/current_now", "4654321");
        auto currnow = BatterySrvClient.GetNowCurrent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::current=%{public}d", currnow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient009 executing, currnow=" << currnow;
        ASSERT_EQ(currnow, 4654321);
    } else {
        auto currnow = BatterySrvClient.GetNowCurrent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::currnow=%{public}d", currnow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient009 executing, currnow=" << currnow;
        ASSERT_TRUE(currnow >= 0);
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
        TestUtils::WriteMock("/data/local/tmp/battery/charge_now", "4654321");
        auto chargenow = BatterySrvClient.GetRemainEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::chargenow=%{public}d", chargenow);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient010 executing, chargenow=" << chargenow;
        ASSERT_EQ(chargenow, 4654321);
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
        TestUtils::WriteMock("/data/local/tmp/battery/charge_full", "4654321");
        auto totalenergy = BatterySrvClient.GetTotalEnergy();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::totalenergy=%{public}d", totalenergy);
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient011 executing, totalenergy=" << totalenergy;
        ASSERT_EQ(totalenergy, 4654321);
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
 * @tc.desc: Test IBatterySrv interface GetBatteryLevel
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient012, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient012 start.");
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/capacity", "22");
        auto batterylevel = BatterySrvClient.GetBatteryLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d", int(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient012 executing, batterylevel=" << int(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_HIGH ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_NORMAL ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_LOW ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_CRITICAL ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_FULL);
    } else {
        auto batterylevel = BatterySrvClient.GetBatteryLevel();
        BATTERY_HILOGI(LABEL_TEST, "BatteryClientTest::batterylevel=%{public}d", int(batterylevel));
        GTEST_LOG_(INFO) << "BatteryClient::BatteryClient012 executing, batterylevel=" << int(batterylevel);
        ASSERT_TRUE(batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_HIGH ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_NORMAL ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_LOW ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_CRITICAL ||
            batterylevel == OHOS::PowerMgr::BatteryLevel::LEVEL_FULL);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryClient::BatteryClient012 end.");
}
}

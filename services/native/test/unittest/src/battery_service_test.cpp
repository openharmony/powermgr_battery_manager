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

#include "battery_service_test.h"
#include <string>
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "battery_log.h"
#include "battery_service.h"
#include "test_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS::HDI::Battery;
using namespace OHOS::EventFwk;
using namespace OHOS;
using namespace std;

namespace {
bool g_isMock = false;
static sptr<BatteryService> g_service;
}

void BatteryServiceTest::SetUpTestCase(void)
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
    GTEST_LOG_(INFO) << "is mock: " << g_service->GetPresent() << " g_isMock: " << g_isMock;
    if (!g_service->GetPresent()) {
        g_isMock = true;
        TestUtils::InitTest();
        g_service->ChangePath("/data/local/tmp");
    }
}

void BatteryServiceTest::TearDownTestCase(void)
{
    g_isMock = false;
    TestUtils::ResetOnline();
}

void BatteryServiceTest::SetUp(void)
{
}

void BatteryServiceTest::TearDown(void)
{
}

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions GetCapacity in BatteryService
 * @tc.type: FUNC
 */
static HWTEST_F (BatteryServiceTest, BatteryService001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService001 start.");
    int32_t capacity = -1;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/capacity", "50");
        capacity = g_service->GetCapacity();
        ASSERT_TRUE(capacity == 50);
    } else {
        capacity = g_service->GetCapacity();
        ASSERT_TRUE(capacity >= 0 && capacity <= 100);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::capacity=%{public}d", capacity);
    GTEST_LOG_(INFO) << "BatteryService::BatteryService001 executing, capacity=" << capacity;
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService001 end.");
}

/**
 * @tc.name: BatteryService002
 * @tc.desc: Test functions GetChargingStatus in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService002, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService002 start.");
    OHOS::PowerMgr::BatteryChargeState chargeState = OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/status", "Charging");
        chargeState = g_service->GetChargingStatus();
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE);
    } else {
        chargeState = g_service->GetChargingStatus();
        ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE ||
            chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::chargeState=%{public}d", int(chargeState));
    GTEST_LOG_(INFO) << "BatteryService::BatteryService002 executing, chargeState=" << int(chargeState);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService002 end.");
}

/**
 * @tc.name: BatteryService003
 * @tc.desc: Test functions GetHealthStatus in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService003, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService003 start.");
    OHOS::PowerMgr::BatteryHealthState healthState = OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_BUTT;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/health", "Good");
        healthState =  g_service->GetHealthStatus();
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    } else {
        healthState =  g_service->GetHealthStatus();
        ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
    }
    GTEST_LOG_(INFO) << "BatteryService::BatteryService003 executing, healthState=" << int(healthState);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService003 end.");
}

/**
 * @tc.name: BatteryService004
 * @tc.desc: Test functions GetPresent in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService004, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService004 start.");
    bool present = false;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/present", "0");
        present = g_service->GetPresent();
        ASSERT_FALSE(present);
    } else {
        present = g_service->GetPresent();
        ASSERT_TRUE(present);
    }
    GTEST_LOG_(INFO) << "BatteryService::BatteryService004 executing, present=" << present;
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService004 end.");
}

/**
 * @tc.name: BatteryService005
 * @tc.desc: Test functions GetVoltage in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService005, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService005 start.");
    int32_t voltage = -1;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/voltage_avg", "4123456");
        TestUtils::WriteMock("/data/local/tmp/battery/voltage_now", "4123456");
        voltage = g_service->GetVoltage();
        ASSERT_TRUE(voltage == 4123456);
    } else {
        voltage = g_service->GetVoltage();
        ASSERT_TRUE(voltage > 0);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::voltage=%{public}d", voltage);
    GTEST_LOG_(INFO) << "BatteryService::BatteryService005 executing, voltage=" << voltage;
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService005 end.");
}

/**
 * @tc.name: BatteryService006
 * @tc.desc: Test functions GetBatteryTemperature in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService006, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService006 start.");
    int32_t temperature = -1;
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/battery/temp", "333");
        temperature = g_service->GetBatteryTemperature();
        ASSERT_TRUE(temperature == 333);
    } else {
        temperature = g_service->GetBatteryTemperature();
        ASSERT_TRUE(temperature > 0);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::voltage=%{public}d", temperature);
    GTEST_LOG_(INFO) << "BatteryService::BatteryService006 executing, temperature=" << temperature;
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService006 end.");
}

/**
 * @tc.name: BatteryService007
 * @tc.desc: Test functions GetTechnology in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService007, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService007 start.");
    std::string technology = "";
    if (g_isMock) {
        TestUtils::WriteMock("/data/local/tmp/ohos-fgu/technology", "Li");
        technology = g_service->GetTechnology();
        ASSERT_TRUE(technology == "Li");
    } else {
        technology = g_service->GetTechnology();
        ASSERT_TRUE(technology == "Li-poly");
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::technology=%{public}s", technology.c_str());
    GTEST_LOG_(INFO) << "BatteryService::BatteryService007 executing, technology=" << technology;
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService007 end.");
}

/**
 * @tc.name: BatteryService008
 * @tc.desc: Test functions GetPluggedType in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService008, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService008 start.");
    OHOS::PowerMgr::BatteryPluggedType pluggedType = OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_BUTT;
    if (g_isMock) {
        TestUtils::ResetOnline();
        TestUtils::WriteMock("/data/local/tmp/Wireless/online", "1");
        TestUtils::WriteMock("/data/local/tmp/Wireless/type", "Wireless");
        pluggedType = g_service->GetPluggedType();
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    } else {
        pluggedType = g_service->GetPluggedType();
        ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::pluggedType=%{public}d", int(pluggedType));
    GTEST_LOG_(INFO) << "BatteryService::BatteryService008 executing, pluggedType=" << int(pluggedType);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService008 end.");
}

/**
 * @tc.name: BatteryService009
 * @tc.desc: Test functions OnStart in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService009, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService009 start.");
    g_service->OnStart();
    bool ready = g_service->IsServiceReady();
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::ready=%{public}d", ready);
    ASSERT_TRUE(ready);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService009 end.");
}

/**
 * @tc.name: BatteryService010
 * @tc.desc: Test functions OnStop in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService010, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService010 start.");
    g_service->OnStop();
    bool ready = g_service->IsServiceReady();
    BATTERY_HILOGI(LABEL_TEST, "BatteryServiceTest::ready=%{public}d", ready);
    ASSERT_FALSE(ready);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService::BatteryService010 end.");
}

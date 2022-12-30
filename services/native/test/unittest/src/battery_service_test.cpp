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

#include "battery_service_test.h"

#include <memory>
#include <string>
#include <fcntl.h>

#include "battery_info.h"
#include "battery_log.h"
#include "battery_service.h"
#include "common_event_data.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "securec.h"
#include "test_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS::EventFwk;
using namespace OHOS::HDI::Battery;
using namespace OHOS;
using namespace std;

namespace {
sptr<BatteryService> g_service;
bool g_isMock = TestUtils::IsMock();
const std::string MOCK_BATTERY_PATH = "/data/service/el0/battery/";
}

void BatteryServiceTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
}

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions RegisterBatteryHdiCallback
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService001 start.");
    EXPECT_TRUE(g_service->RegisterBatteryHdiCallback());
    BATTERY_HILOGD(LABEL_TEST, "BatteryService001 end.");
}

/**
 * @tc.name: BatteryService003
 * @tc.desc: Test functions DestoryBatteryDialog
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService003, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService003 start.");
    EXPECT_TRUE(g_service->DestoryBatteryDialog());
    BATTERY_HILOGD(LABEL_TEST, "BatteryService003 end.");
}

/**
 * @tc.name: BatteryService004
 * @tc.desc: Test functions GetVoltage
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService004, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService004 start.");
    if (g_isMock) {
        auto tempVoltage = g_service->GetVoltage();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");
        auto voltage = g_service->GetVoltage();
        ASSERT_TRUE(voltage == 4654321);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", std::to_string(tempVoltage));
    } else {
        auto voltage = g_service->GetVoltage();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService004::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryService004 executing, voltage=" << voltage;
        ASSERT_TRUE(voltage >= 0);
    }

    BATTERY_HILOGD(LABEL_TEST, "BatteryService004 end.");
}

/**
 * @tc.name: BatteryService005
 * @tc.desc: Test functions GetPresent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService005, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService005 start.");
    if (g_isMock) {
        auto tempPresent = g_service->GetPresent();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", "0");
        auto present = g_service->GetPresent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService005::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryService005 executing, present=" << present;
        ASSERT_FALSE(present);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", std::to_string(tempPresent));
    } else {
        auto present = g_service->GetPresent();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService005::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryService005 executing, present=" << present;
        ASSERT_TRUE(present);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryService005 end.");
}

/**
 * @tc.name: BatteryService006
 * @tc.desc: Test functions GetBatteryTemperature
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService006, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService006 start.");
    if (g_isMock) {
        auto tempTempPresent = g_service->GetBatteryTemperature();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", "222");
        auto temperature = g_service->GetBatteryTemperature();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService006::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService006 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature == 222);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", std::to_string(tempTempPresent));
    } else {
        auto temperature = g_service->GetBatteryTemperature();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService006::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService006 executing, temperature=" << temperature;
        ASSERT_TRUE(temperature >= 0 && temperature <= 600);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryService006 end.");
}

/**
 * @tc.name: BatteryService012
 * @tc.desc: Test functions RegisterHdiStatusListener
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService007, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService007 start.");
    EXPECT_TRUE(g_service->RegisterHdiStatusListener());
    BATTERY_HILOGD(LABEL_TEST, "BatteryService007 end.");
}

/**
 * @tc.name: BatteryService008
 * @tc.desc: Test functions OnStart and OnStop and OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService008, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService008 start.");
    g_service->OnStop();
    g_service->OnStop();
    g_service->OnStart();
    g_service->OnStart();
    int32_t systemAbilityId = 3602; // MISCDEVICE_SERVICE_ABILITY_ID
    g_service->OnAddSystemAbility(systemAbilityId, std::string("BatteryService008"));
    BATTERY_HILOGD(LABEL_TEST, "BatteryService008 end.");
}

/**
 * @tc.name: BatteryService009
 * @tc.desc: Test functions MockUnplugged, with parameter is false
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService009, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService009 start.");
    g_service->MockUnplugged(false);
    EXPECT_EQ(g_service->IsMockUnplugged(), false);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService009 end.");
}

/**
 * @tc.name: BatteryService010
 * @tc.desc: Test functions GetRemainEnergy
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService010, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService010 start.");
    EXPECT_NE(g_service->GetRemainEnergy(), ERR_NO_INIT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService010 end.");
}

/**
 * @tc.name: BatteryService011
 * @tc.desc: Test functions GetNowCurrent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService011, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService011 start.");
    EXPECT_NE(g_service->GetNowCurrent(), ERR_NO_INIT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService011 end.");
}

/**
 * @tc.name: BatteryService012
 * @tc.desc: Test functions GetTechnology
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService012, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService012 start.");
    EXPECT_FALSE(g_service->GetTechnology().empty());
    BATTERY_HILOGD(LABEL_TEST, "BatteryService012 end.");
}

/**
 * @tc.name: BatteryService013
 * @tc.desc: Test functions GetPluggedType
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService013, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService013 start.");
    BatteryPluggedType pluggedType = g_service->GetPluggedType();
    EXPECT_TRUE(
        pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE && pluggedType <= BatteryPluggedType::PLUGGED_TYPE_BUTT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService013 end.");
}

/**
 * @tc.name: BatteryService014
 * @tc.desc: Test functions GetCurrentAverage
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService014, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService014 start.");
    EXPECT_NE(g_service->GetCurrentAverage(), ERR_NO_INIT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService014 end.");
}

/**
 * @tc.name: BatteryService015
 * @tc.desc: Test functions GetHealthStatus
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService015, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService015 start.");
    BatteryHealthState healthState = g_service->GetHealthStatus();
    EXPECT_TRUE(healthState >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
        healthState <= BatteryHealthState::HEALTH_STATE_BUTT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService015 end.");
}

/**
 * @tc.name: BatteryService016
 * @tc.desc: Test functions GetChargingStatus
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService016, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService016 start.");
    BatteryChargeState chargeState = g_service->GetChargingStatus();
    EXPECT_TRUE(
        chargeState >= BatteryChargeState::CHARGE_STATE_NONE && chargeState <= BatteryChargeState::CHARGE_STATE_BUTT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService016 end.");
}

/**
 * @tc.name: BatteryService017
 * @tc.desc: Test functions GetTotalEnergy
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService017, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryService017 start.");
    EXPECT_NE(g_service->GetTotalEnergy(), ERR_NO_INIT);
    BATTERY_HILOGD(LABEL_TEST, "BatteryService017 end.");
}

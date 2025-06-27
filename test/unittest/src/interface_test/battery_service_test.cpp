/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#ifdef GTEST
#define private   public
#define protected public
#endif

#include <fcntl.h>
#include <memory>
#include <string>

#include "battery_info.h"
#include "battery_log.h"
#include "battery_service.h"
#include "common_event_data.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "securec.h"
#include "test_utils.h"
#include "battery_xcollie.h"

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
constexpr int DELAY_TIME_US = 200000;
constexpr int TEST_CAPACITY_MIN = 0;
constexpr int TEST_CAPACITY_MAX = 100;
constexpr int TEST_CAPACITY_FIRST = 50;
constexpr int TEST_CAPACITY_SECOND = 60;
}

void BatteryServiceTest::TearDownTestCase(void)
{
    g_service->Reset();
}

/**
 * @tc.name: BatteryService000
 * @tc.desc: Test functions DelayedSpSingleton GetInstance
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService000, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService000 function start!");
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    EXPECT_TRUE(g_service != nullptr);
    g_service->OnStart();
    g_service = nullptr;
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    EXPECT_TRUE(g_service != nullptr);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService000 function end!");
}

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions RegisterBatteryHdiCallback
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService001 function start!");
    EXPECT_TRUE(g_service->RegisterBatteryHdiCallback());
    BATTERY_HILOGI(LABEL_TEST, "BatteryService001 function end!");
}

/**
 * @tc.name: BatteryService004
 * @tc.desc: Test functions GetVoltage
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService004 function start!");
    if (g_isMock) {
        auto tempVoltage = g_service->GetVoltageInner();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");
        auto voltage = g_service->GetVoltageInner();
        EXPECT_TRUE(voltage == 4654321);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", std::to_string(tempVoltage));
    } else {
        auto voltage = g_service->GetVoltageInner();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService004::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryService004 executing, voltage=" << voltage;
        EXPECT_TRUE(voltage >= 0);
    }

    BATTERY_HILOGI(LABEL_TEST, "BatteryService004 function end!");
}

/**
 * @tc.name: BatteryService005
 * @tc.desc: Test functions GetPresent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService005 function start!");
    if (g_isMock) {
        auto tempPresent = g_service->GetPresentInner();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", "0");
        auto present = g_service->GetPresentInner();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService005::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryService005 executing, present=" << present;
        EXPECT_FALSE(present);
        auto presentIdl = false;
        g_service->GetPresent(presentIdl);
        EXPECT_EQ(present, presentIdl);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/present", std::to_string(tempPresent));
    } else {
        auto present = g_service->GetPresentInner();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService005::present=%{public}d", present);
        GTEST_LOG_(INFO) << "BatteryService005 executing, present=" << present;
        EXPECT_TRUE(present);
        auto presentIdl = false;
        g_service->GetPresent(presentIdl);
        EXPECT_EQ(present, presentIdl);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService005 function end!");
}

/**
 * @tc.name: BatteryService006
 * @tc.desc: Test functions GetBatteryTemperature
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService006 function start!");
    if (g_isMock) {
        auto tempTempPresent = g_service->GetBatteryTemperatureInner();
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", "222");
        auto temperature = g_service->GetBatteryTemperatureInner();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService006::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService006 executing, temperature=" << temperature;
        EXPECT_TRUE(temperature == 222);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", std::to_string(tempTempPresent));
    } else {
        auto temperature = g_service->GetBatteryTemperatureInner();
        BATTERY_HILOGI(LABEL_TEST, "BatteryService006::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService006 executing, temperature=" << temperature;
        EXPECT_TRUE(temperature >= 0 && temperature <= 600);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService006 function end!");
}

/**
 * @tc.name: BatteryService012
 * @tc.desc: Test functions RegisterHdiStatusListener
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService007 function start!");
    EXPECT_TRUE(g_service->RegisterHdiStatusListener());
    BATTERY_HILOGI(LABEL_TEST, "BatteryService007 function end!");
}

/**
 * @tc.name: BatteryService008
 * @tc.desc: Test functions OnStart and OnStop and OnAddSystemAbility
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService008 function start!");
    g_service->OnStart();
    g_service->OnStart();
    g_service->OnStop();
    g_service->OnStop();
    int32_t systemAbilityId = 3602; // MISCDEVICE_SERVICE_ABILITY_ID
    g_service->OnAddSystemAbility(systemAbilityId, std::string("BatteryService008"));
    g_service->OnAddSystemAbility(0, std::string("BatteryService008"));
    g_service->ready_ = true;
    g_service->OnStart();
    EXPECT_TRUE(g_service->ready_);
    g_service->OnStop();
    EXPECT_FALSE(g_service->ready_);

    g_service->OnStart();
    g_service->iBatteryInterface_ = V2_0::IBatteryInterface::Get();
    BATTERY_HILOGI(LABEL_TEST, "BatteryService008 function end!");
}

/**
 * @tc.name: BatteryService009
 * @tc.desc: Test functions MockUnplugged
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService009 function start!");
    usleep(DELAY_TIME_US);
    g_service->MockUnplugged();
    EXPECT_EQ(g_service->IsMockUnplugged(), true);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService009 function end!");
}

/**
 * @tc.name: BatteryService010
 * @tc.desc: Test functions GetRemainEnergy
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService010 function start!");
    EXPECT_NE(g_service->GetRemainEnergyInner(), ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService010 function end!");
}

/**
 * @tc.name: BatteryService011
 * @tc.desc: Test functions GetNowCurrent
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService011, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService011 function start!");
    EXPECT_NE(g_service->GetNowCurrentInner(), ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService011 function end!");
}

/**
 * @tc.name: BatteryService012
 * @tc.desc: Test functions GetTechnology
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService012, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService012 function start!");
    EXPECT_FALSE(g_service->GetTechnologyInner().empty());
    BATTERY_HILOGI(LABEL_TEST, "BatteryService012 function end!");
}

/**
 * @tc.name: BatteryService013
 * @tc.desc: Test functions GetPluggedType
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService013, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService013 function start!");
    BatteryPluggedType pluggedType = g_service->GetPluggedTypeInner();
    EXPECT_TRUE(
        pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE && pluggedType <= BatteryPluggedType::PLUGGED_TYPE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService013 function end!");
}

/**
 * @tc.name: BatteryService014
 * @tc.desc: Test functions GetCurrentAverage
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService014, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService014 function start!");
    EXPECT_NE(g_service->GetCurrentAverageInner(), ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService014 function end!");
}

/**
 * @tc.name: BatteryService015
 * @tc.desc: Test functions GetHealthStatus
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService015, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService015 function start!");
    BatteryHealthState healthState = g_service->GetHealthStatusInner();
    EXPECT_TRUE(healthState >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
        healthState <= BatteryHealthState::HEALTH_STATE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService015 function end!");
}

/**
 * @tc.name: BatteryService016
 * @tc.desc: Test functions GetChargingStatus
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService016, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService016 function start!");
    BatteryChargeState chargeState = g_service->GetChargingStatusInner();
    EXPECT_TRUE(
        chargeState >= BatteryChargeState::CHARGE_STATE_NONE && chargeState <= BatteryChargeState::CHARGE_STATE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService016 function end!");
}

/**
 * @tc.name: BatteryService017
 * @tc.desc: Test functions GetTotalEnergy
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryServiceTest, BatteryService017, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService017 function start!");
    EXPECT_NE(g_service->GetTotalEnergyInner(), ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService017 function end!");
}

/**
 * @tc.name: BatteryService018
 * @tc.desc: Test functions HandleBatteryCallbackEvent
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService018, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService018 function start!");
    V2_0::BatteryInfo event;
    event.capacity = 90; // Prevent shutdown
    EXPECT_EQ(g_service->HandleBatteryCallbackEvent(event), ERR_OK);

    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-u";
    args.push_back(arg);
    g_service->isBootCompleted_ = true;
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);

    EXPECT_EQ(g_service->HandleBatteryCallbackEvent(event), ERR_OK);

    args.clear();
    arg = u"-r";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);

    EXPECT_EQ(g_service->HandleBatteryCallbackEvent(event), ERR_OK);
    EXPECT_EQ(g_service->HandleBatteryCallbackEvent(event), ERR_OK);

    BATTERY_HILOGI(LABEL_TEST, "BatteryService018 function end!");
}

/**
 * @tc.name: BatteryService019
 * @tc.desc: Test functions ChangePath
 * @tc.type: FUNC
 * @tc.require: issueI6O3E1
 */
static HWTEST_F(BatteryServiceTest, BatteryService019, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService019 function start!");
    usleep(DELAY_TIME_US);
    g_service->iBatteryInterface_ = nullptr;
    auto ret = g_service->ChangePath("/data/service/el0/battery");
    EXPECT_FALSE(ret);

    g_service->iBatteryInterface_ = V2_0::IBatteryInterface::Get();
    ret = g_service->ChangePath("/data/service/el0/battery");
    EXPECT_TRUE(ret);

    if (!g_isMock) {
        ret = g_service->ChangePath("/sys/class/power_supply");
        EXPECT_TRUE(ret);
    }

    BATTERY_HILOGI(LABEL_TEST, "BatteryService019 function end!");
}

/**
 * @tc.name: BatteryService020
 * @tc.desc: Test functions CalculateRemainingChargeTime
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService020, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService020 function start!");
    g_service->CalculateRemainingChargeTime(101, BatteryChargeState::CHARGE_STATE_DISABLE);
    EXPECT_FALSE(g_service->chargeFlag_);

    g_service->chargeFlag_ = true;
    g_service->lastCapacity_ = 50;
    g_service->CalculateRemainingChargeTime(30, BatteryChargeState::CHARGE_STATE_ENABLE);
    EXPECT_EQ(g_service->lastCapacity_, 30);

    g_service->lastCapacity_ = 50;
    g_service->chargeFlag_ = true;
    g_service->CalculateRemainingChargeTime(51, BatteryChargeState::CHARGE_STATE_ENABLE);

    BATTERY_HILOGI(LABEL_TEST, "BatteryService020 function end!");
}

/**
 * @tc.name: BatteryService021
 * @tc.desc: Test functions GetCapacityLevel
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService021, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService021 function start!");
    if (g_isMock) {
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "1");
        auto level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_SHUTDOWN);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "4");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_CRITICAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "10");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_WARNING);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "15");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_LOW);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "90");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_NORMAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "99");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_HIGH);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "100");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_FULL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "200");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_NONE);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "50");
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService021 function end!");
}

/**
 * @tc.name: BatteryService022
 * @tc.desc: Test functions GetChargeType
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService022, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService022 function start!");
    ChargeType chargeType = g_service->GetChargeType();
    EXPECT_TRUE(chargeType >= ChargeType::NONE && chargeType <= ChargeType::WIRELESS_SUPER_QUICK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService022 function end!");
}

/**
 * @tc.name: BatteryService023
 * @tc.desc: Test functions OnStop
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService023, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService023 function start!");
    g_service->ready_ = true;
    g_service->OnStop();
    EXPECT_FALSE(g_service->ready_);

    g_service->ready_ = true;
    g_service->iBatteryInterface_ = nullptr;
    g_service->hdiServiceMgr_ = nullptr;
    g_service->OnStop();
    EXPECT_FALSE(g_service->ready_);
    g_service->OnStart();

    BATTERY_HILOGI(LABEL_TEST, "BatteryService023 function end!");
}

/**
 * @tc.name: BatteryService024
 * @tc.desc: Test functions GetVoltage
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService024, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService024 function start!");
    if (g_isMock) {
        int32_t tempVoltage = 0;
        EXPECT_EQ(g_service->GetVoltage(tempVoltage), ERR_OK);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");
        int32_t voltage = 0;
        g_service->GetVoltage(voltage);
        EXPECT_TRUE(voltage == 4654321);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/voltage_now", std::to_string(tempVoltage));
    } else {
        int32_t voltage = 0;
        g_service->GetVoltage(voltage);
        BATTERY_HILOGI(LABEL_TEST, "BatteryService024::voltage=%{public}d", voltage);
        GTEST_LOG_(INFO) << "BatteryService024 executing, voltage=" << voltage;
        EXPECT_TRUE(voltage >= 0);
    }

    BATTERY_HILOGI(LABEL_TEST, "BatteryService024 function end!");
}

/**
 * @tc.name: BatteryService025
 * @tc.desc: Test functions IsBatteryConfigSupported
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService025, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService025 function start!");
    string sceneName = "testScene";
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_FAILURE);
    bool result = false;
    g_service->IsBatteryConfigSupported(sceneName, result, batteryErr);
    EXPECT_EQ(batteryErr, static_cast<int32_t>(BatteryError::ERR_FAILURE));
    BATTERY_HILOGI(LABEL_TEST, "BatteryService025 function end!");
}

/**
 * @tc.name: BatteryService026
 * @tc.desc: Test functions GetBatteryTemperature
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService026, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService026 function start!");
    if (g_isMock) {
        int32_t tempTemperature = 0;
        g_service->GetBatteryTemperature(tempTemperature);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", "222");
        int32_t temperature = 0;
        g_service->GetBatteryTemperature(temperature);
        BATTERY_HILOGI(LABEL_TEST, "BatteryService026::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService026 executing, temperature=" << temperature;
        EXPECT_TRUE(temperature == 222);
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/temp", std::to_string(tempTemperature));
    } else {
        int32_t temperature = 0;
        g_service->GetBatteryTemperature(temperature);
        BATTERY_HILOGI(LABEL_TEST, "BatteryService026::temperature=%{public}d", temperature);
        GTEST_LOG_(INFO) << "BatteryService026 executing, temperature=" << temperature;
        EXPECT_TRUE(temperature >= 0 && temperature <= 600);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService026 function end!");
}

/**
 * @tc.name: BatteryService027
 * @tc.desc: Test functions GetRemainEnergy
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService027, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService027 function start!");
    int32_t remainEnergy = 0;
    g_service->GetRemainEnergy(remainEnergy);
    EXPECT_NE(remainEnergy, ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService027 function end!");
}

/**
 * @tc.name: BatteryService028
 * @tc.desc: Test functions GetNowCurrent
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService028, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService028 function start!");
    int32_t nowCurrent = 0;
    g_service->GetNowCurrent(nowCurrent);
    EXPECT_NE(nowCurrent, ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService028 function end!");
}

/**
 * @tc.name: BatteryService029
 * @tc.desc: Test functions GetTechnology
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService029, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService029 function start!");
    std::string technology;
    g_service->GetTechnology(technology);
    EXPECT_FALSE(technology.empty());
    BATTERY_HILOGI(LABEL_TEST, "BatteryService029 function end!");
}

/**
 * @tc.name: BatteryService030
 * @tc.desc: Test functions GetPluggedType
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService030, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService030 function start!");
    uint32_t pluggedTypeTemp = 0;
    g_service->GetPluggedType(pluggedTypeTemp);
    BatteryPluggedType pluggedType = static_cast<BatteryPluggedType>(pluggedTypeTemp);
    EXPECT_TRUE(
        pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE && pluggedType <= BatteryPluggedType::PLUGGED_TYPE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService030 function end!");
}

/**
 * @tc.name: BatteryService031
 * @tc.desc: Test functions GetCurrentAverage
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService031, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService031 function start!");
    int32_t currentAverage = 0;
    g_service->GetCurrentAverage(currentAverage);
    EXPECT_NE(currentAverage, ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService031 function end!");
}

/**
 * @tc.name: BatteryService032
 * @tc.desc: Test functions GetHealthStatus
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService032, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService032 function start!");
    uint32_t healthStateTemp = 0;
    g_service->GetHealthStatus(healthStateTemp);
    BatteryHealthState healthState = static_cast<BatteryHealthState>(healthStateTemp);
    EXPECT_TRUE(healthState >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
        healthState <= BatteryHealthState::HEALTH_STATE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService032 function end!");
}

/**
 * @tc.name: BatteryService033
 * @tc.desc: Test functions GetChargingStatus
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService033, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService033 function start!");
    uint32_t chargingStateTemp = 0;
    g_service->GetChargingStatus(chargingStateTemp);
    BatteryChargeState chargeState = static_cast<BatteryChargeState>(chargingStateTemp);
    EXPECT_TRUE(
        chargeState >= BatteryChargeState::CHARGE_STATE_NONE && chargeState <= BatteryChargeState::CHARGE_STATE_BUTT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService033 function end!");
}

/**
 * @tc.name: BatteryService034
 * @tc.desc: Test functions GetTotalEnergy
 * @tc.type: FUNC
 * @tc.require:
 */
static HWTEST_F(BatteryServiceTest, BatteryService034, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService034 function start!");
    int32_t totalEnergy = 0;
    g_service->GetTotalEnergy(totalEnergy);
    EXPECT_NE(totalEnergy, ERR_NO_INIT);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService034 function end!");
}

/**
 * @tc.name: BatteryService035
 * @tc.desc: Test functions GetCapacityLevel
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService035, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService035 function start!");
    if (g_isMock) {
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "1");
        uint32_t level = 0;
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_SHUTDOWN);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "4");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_CRITICAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "10");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_WARNING);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "15");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_LOW);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "90");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_NORMAL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "99");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_HIGH);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "100");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_FULL);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "200");
        g_service->GetCapacityLevel(level);
        EXPECT_EQ(static_cast<BatteryCapacityLevel>(level), BatteryCapacityLevel::LEVEL_NONE);

        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "50");
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService035 function end!");
}

/**
 * @tc.name: BatteryService036
 * @tc.desc: Test functions GetCapacity
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryServiceTest, BatteryService036, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService036 function start!");
    int32_t capacity = 0;
    auto ret = g_service->GetCapacity(capacity);
    EXPECT_EQ(ret, ERR_OK);
    ASSERT_TRUE(capacity <= 100 && capacity >= 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService036 function end!");
}

/**
 * @tc.name: BatteryService037
 * @tc.desc: Test functions GetRemainingChargeTime
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryServiceTest, BatteryService037, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService037 function start!");
    int64_t remainTime = 0;
    auto ret = g_service->GetRemainingChargeTime(remainTime);
    EXPECT_EQ(ret, ERR_OK);
    ASSERT_TRUE(remainTime >= 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService037 function end!");
}

/**
 * @tc.name: BatteryService038
 * @tc.desc: Test functions GetRemainingChargeTime
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(BatteryServiceTest, BatteryService038, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService038 function start!");
    auto ret = g_service->CapacityLevelCompare(TEST_CAPACITY_FIRST, TEST_CAPACITY_MIN, TEST_CAPACITY_MAX);
    ASSERT_TRUE(ret);
    ret = g_service->CapacityLevelCompare(TEST_CAPACITY_FIRST, TEST_CAPACITY_SECOND, TEST_CAPACITY_MAX);
    ASSERT_FALSE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryService038 function end!");
}

/**
 * @tc.name: BatteryService039
 * @tc.desc: Test functions GetCapacityLevelInner
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService039, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService039 function start!");
    if (g_isMock) {
        auto tempCapacityThreshold = g_service->shutdownCapacityThreshold_;
        g_service->shutdownCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "1");
        auto level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_CRITICAL);
        g_service->shutdownCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->criticalCapacityThreshold_;
        g_service->criticalCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "4");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_WARNING);
        g_service->criticalCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->warningCapacityThreshold_;
        g_service->warningCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "10");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_LOW);
        g_service->warningCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->lowCapacityThreshold_;
        g_service->lowCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "15");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_NORMAL);
        g_service->lowCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->normalCapacityThreshold_;
        g_service->normalCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "90");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_HIGH);
        g_service->normalCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->highCapacityThreshold_;
        g_service->highCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "99");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_FULL);
        g_service->highCapacityThreshold_ = tempCapacityThreshold;
        tempCapacityThreshold = g_service->fullCapacityThreshold_;
        g_service->fullCapacityThreshold_ = 0;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "100");
        level = g_service->GetCapacityLevelInner();
        EXPECT_EQ(level, BatteryCapacityLevel::LEVEL_NONE);
        g_service->fullCapacityThreshold_ = tempCapacityThreshold;
        TestUtils::WriteMock(MOCK_BATTERY_PATH + "/battery/capacity", "50");
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryService039 function end!");
}

/**
 * @tc.name: BatteryService040
 * @tc.desc: Test functions SetBatteryConfig
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService040, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService040 function start!");
    string sceneName = "testScene";
    string value = "";
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_FAILURE);
    g_service->SetBatteryConfig(sceneName, value, batteryErr);
    EXPECT_EQ(batteryErr, static_cast<int32_t>(BatteryError::ERR_SYSTEM_API_DENIED));
    BATTERY_HILOGI(LABEL_TEST, "BatteryService040 function end!");
}

/**
 * @tc.name: BatteryService041
 * @tc.desc: Test functions GetBatteryConfig
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryServiceTest, BatteryService041, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryService041 function start!");
    string sceneName = "testScene";
    string result = "";
    int32_t batteryErr = static_cast<int32_t>(BatteryError::ERR_FAILURE);
    g_service->GetBatteryConfig(sceneName, result, batteryErr);
    EXPECT_EQ(batteryErr, static_cast<int32_t>(BatteryError::ERR_FAILURE));
    BATTERY_HILOGI(LABEL_TEST, "BatteryService041 function end!");
}

/**
 * @tc.name: BatteryXCollie001
 * @tc.desc: Test functions BatteryXCollie default
 * @tc.type: FUNC
 */
HWTEST_F(BatteryServiceTest, BatteryXCollie001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie001 function start!");
    BatteryXCollie batteryXCollie("BatteryService::SetBatteryConfig");
    EXPECT_FALSE(batteryXCollie.isCanceled_);
    batteryXCollie.CancelBatteryXCollie();
    EXPECT_TRUE(batteryXCollie.isCanceled_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie001 function end!");
}

/**
 * @tc.name: BatteryXCollie002
 * @tc.desc: Test functions BatteryXCollie isRecovery=true
 * @tc.type: FUNC
 */
HWTEST_F(BatteryServiceTest, BatteryXCollie002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie002 function start!");
    BatteryXCollie batteryXCollie("BatteryService::SetBatteryConfig", true);
    EXPECT_FALSE(batteryXCollie.isCanceled_);
    batteryXCollie.CancelBatteryXCollie();
    EXPECT_TRUE(batteryXCollie.isCanceled_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie002 function end!");
}

/**
 * @tc.name: BatteryXCollie003
 * @tc.desc: Test functions BatteryXCollie logtag empty
 * @tc.type: FUNC
 */
HWTEST_F(BatteryServiceTest, BatteryXCollie003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie003 function start!");
    std::string emptyString;
    BatteryXCollie batteryXCollie(emptyString);
    EXPECT_FALSE(batteryXCollie.isCanceled_);
    batteryXCollie.CancelBatteryXCollie();
    EXPECT_TRUE(batteryXCollie.isCanceled_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryXCollie003 function end!");
}
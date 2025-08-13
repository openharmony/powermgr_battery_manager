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

#include <gtest/gtest.h>

#include "ohos.batteryInfo.proj.hpp"
#include "ohos.batteryInfo.impl.hpp"
#include "ohos.batteryInfo.user.hpp"
#include "taihe/runtime.hpp"
#include "battery_srv_client.h"
#include "battery_info.h"
#include "battery_srv_errors.h"
#include "battery_log.h"

using namespace taihe;
using namespace ohos::batteryInfo;
using namespace OHOS::PowerMgr;

using namespace testing;
using namespace testing::ext;

namespace {
BatteryError g_returnError = BatteryError::ERR_OK;
uint32_t g_count = 0;
OHOS::PowerMgr::BatteryChargeState g_chargeState = OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT;
OHOS::PowerMgr::BatteryHealthState g_healthState = OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_BUTT;
OHOS::PowerMgr::BatteryPluggedType g_pluggedType = OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_BUTT;
OHOS::PowerMgr::BatteryCapacityLevel g_capacityLevel = OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NONE;
}

namespace taihe {
void set_business_error(int32_t err_code, taihe::string_view msg)
{
    (void)err_code;
    (void)msg;
}
}

namespace OHOS::PowerMgr {
BatteryError BatterySrvClient::SetBatteryConfig(const std::string& sceneName, const std::string& value)
{
    return g_returnError;
}

BatteryError BatterySrvClient::GetBatteryConfig(const std::string& sceneName, std::string& result)
{
    return g_returnError;
}

BatteryError BatterySrvClient::IsBatteryConfigSupported(const std::string& sceneName, bool& result)
{
    return g_returnError;
}

int32_t BatterySrvClient::GetCapacity()
{
    ++g_count;
    return INVALID_BATT_INT_VALUE;
}

BatteryChargeState BatterySrvClient::GetChargingStatus()
{
    ++g_count;
    return g_chargeState;
}

BatteryHealthState BatterySrvClient::GetHealthStatus()
{
    ++g_count;
    return g_healthState;
}

BatteryPluggedType BatterySrvClient::GetPluggedType()
{
    ++g_count;
    return g_pluggedType;
}

int32_t BatterySrvClient::GetVoltage()
{
    ++g_count;
    return INVALID_BATT_BOOL_VALUE;
}

bool BatterySrvClient::GetPresent()
{
    ++g_count;
    return INVALID_BATT_BOOL_VALUE;
}

std::string BatterySrvClient::GetTechnology()
{
    ++g_count;
    return "";
}

int32_t BatterySrvClient::GetBatteryTemperature()
{
    ++g_count;
    return INVALID_BATT_TEMP_VALUE;
}

int32_t BatterySrvClient::GetNowCurrent()
{
    ++g_count;
    return INVALID_BATT_INT_VALUE;
}

int32_t BatterySrvClient::GetRemainEnergy()
{
    ++g_count;
    return INVALID_BATT_INT_VALUE;
}

int32_t BatterySrvClient::GetTotalEnergy()
{
    ++g_count;
    return INVALID_BATT_INT_VALUE;
}

BatteryCapacityLevel BatterySrvClient::GetCapacityLevel()
{
    ++g_count;
    return g_capacityLevel;
}

int64_t BatterySrvClient::GetRemainingChargeTime()
{
    ++g_count;
    return INVALID_REMAINING_CHARGE_TIME_VALUE;
}
}

namespace {
class BatteryTaiheNativeTest : public ::testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown()
    {
        g_returnError = BatteryError::ERR_OK;
        g_count = 0;
        g_chargeState = OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT;
        g_healthState = OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_BUTT;
        g_pluggedType = OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_BUTT;
        g_capacityLevel = OHOS::PowerMgr::BatteryCapacityLevel::LEVEL_NONE;
    }
};

/**
 * @tc.name: BatteryTaiheNativeTest_001
 * @tc.desc: test battery taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_001 start");
    string_view sceneName = "BatteryTaiheNativeTest";
    string_view sceneValue = "001";
    int32_t ret = SetBatteryConfig(sceneName, sceneValue);
    EXPECT_EQ(ret, static_cast<int32_t>(BatteryError::ERR_OK));

    g_returnError = BatteryError::ERR_FAILURE;
    ret = SetBatteryConfig(sceneName, sceneValue);
    EXPECT_EQ(ret, static_cast<int32_t>(BatteryError::ERR_FAILURE));

    g_returnError = BatteryError::ERR_CONNECTION_FAIL;
    ret = SetBatteryConfig(sceneName, sceneValue);
    EXPECT_EQ(ret, static_cast<int32_t>(BatteryError::ERR_CONNECTION_FAIL));
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_001 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_002
 * @tc.desc: test battery taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_002 start");
    string_view sceneName = "BatteryTaiheNativeTest_002";
    taihe::string ret = GetBatteryConfig(sceneName);
    EXPECT_EQ(ret, "");

    g_returnError = BatteryError::ERR_FAILURE;
    ret = GetBatteryConfig(sceneName);
    EXPECT_EQ(ret, "");

    g_returnError = BatteryError::ERR_CONNECTION_FAIL;
    ret = GetBatteryConfig(sceneName);
    EXPECT_EQ(ret, "");
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_002 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_003
 * @tc.desc: test battery taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_003 start");
    string_view sceneName = "BatteryTaiheNativeTest_003";
    bool ret = IsBatteryConfigSupported(sceneName);
    EXPECT_FALSE(ret);

    g_returnError = BatteryError::ERR_FAILURE;
    ret = IsBatteryConfigSupported(sceneName);
    EXPECT_FALSE(ret);

    g_returnError = BatteryError::ERR_CONNECTION_FAIL;
    ret = IsBatteryConfigSupported(sceneName);
    EXPECT_FALSE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_003 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_004
 * @tc.desc: test battery taihe native
 * @tc.type: FUNC
 * @tc.require: issue#ICAK9Z
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_004 start");
    BatterySOC();
    ChargingStatus();
    HealthStatus();
    PluggedType();
    Voltage();
    Technology();
    BatteryTemperature();
    IsBatteryPresent();
    GetCapacityLevel();
    EstimatedRemainingChargeTime();
    TotalEnergy();
    NowCurrent();
    RemainingEnergy();
    EXPECT_EQ(g_count, 13); // func call counts
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_004 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_005
 * @tc.desc: test battery taihe native ChargingStatus
 * @tc.type: FUNC
 * @tc.require: issue#ICSS7U
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_005 start");
    using OHOS::PowerMgr::BatteryChargeState;
    for (int i = static_cast<int>(BatteryChargeState::CHARGE_STATE_NONE);
        i <= static_cast<int>(BatteryChargeState::CHARGE_STATE_BUTT); ++i) {
        g_chargeState = BatteryChargeState(i);
        ohos::batteryInfo::BatteryChargeState testChargeState = ChargingStatus();
        if (g_chargeState == BatteryChargeState::CHARGE_STATE_BUTT) {
            EXPECT_EQ(testChargeState, ohos::batteryInfo::BatteryChargeState::from_value(0));
        } else {
            EXPECT_EQ(testChargeState, ohos::batteryInfo::BatteryChargeState::from_value(i));
        }
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_005 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_006
 * @tc.desc: test battery taihe native HealthStatus
 * @tc.type: FUNC
 * @tc.require: issue#ICSS7U
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_006 start");
    using OHOS::PowerMgr::BatteryHealthState;
    for (int i = static_cast<int>(BatteryHealthState::HEALTH_STATE_UNKNOWN);
        i <= static_cast<int>(BatteryHealthState::HEALTH_STATE_BUTT); ++i) {
        g_healthState = BatteryHealthState(i);
        ohos::batteryInfo::BatteryHealthState testHealthState = HealthStatus();
        if (g_healthState == BatteryHealthState::HEALTH_STATE_BUTT) {
            EXPECT_EQ(testHealthState, ohos::batteryInfo::BatteryHealthState::from_value(0));
        } else {
            EXPECT_EQ(testHealthState, ohos::batteryInfo::BatteryHealthState::from_value(i));
        }
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_006 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_007
 * @tc.desc: test battery taihe native PluggedType
 * @tc.type: FUNC
 * @tc.require: issue#ICSS7U
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_007 start");
    using OHOS::PowerMgr::BatteryPluggedType;
    for (int i = static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_NONE);
        i <= static_cast<int>(BatteryPluggedType::PLUGGED_TYPE_BUTT); ++i) {
        g_pluggedType = BatteryPluggedType(i);
        ohos::batteryInfo::BatteryPluggedType testPluggedType = PluggedType();
        if (g_pluggedType == BatteryPluggedType::PLUGGED_TYPE_BUTT) {
            EXPECT_EQ(testPluggedType, ohos::batteryInfo::BatteryPluggedType::from_value(0));
        } else {
            EXPECT_EQ(testPluggedType, ohos::batteryInfo::BatteryChargeState::from_value(i));
        }
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_007 end");
}

/**
 * @tc.name: BatteryTaiheNativeTest_008
 * @tc.desc: test battery taihe native GetCapacityLevel
 * @tc.type: FUNC
 * @tc.require: issue#ICSS7U
 */
HWTEST_F(BatteryTaiheNativeTest, BatteryTaiheNativeTest_008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_008 start");
    using OHOS::PowerMgr::BatteryCapacityLevel;
    for (int i = static_cast<int>(BatteryCapacityLevel::LEVEL_NONE);
        i <= static_cast<int>(BatteryCapacityLevel::LEVEL_RESERVED); ++i) {
        g_capacityLevel = BatteryCapacityLevel(i);
        ohos::batteryInfo::BatteryCapacityLevel testCapacityLevel = GetCapacityLevel();
        if (g_capacityLevel == BatteryCapacityLevel::LEVEL_RESERVED) {
            EXPECT_EQ(testCapacityLevel, ohos::batteryInfo::BatteryCapacityLevel::from_value(0));
        } else {
            EXPECT_EQ(testCapacityLevel, ohos::batteryInfo::BatteryCapacityLevel::from_value(i));
        }
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryTaiheNativeTest_008 end");
}
}

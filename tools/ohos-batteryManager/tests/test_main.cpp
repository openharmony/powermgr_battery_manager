/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <string>

#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_srv_errors.h"
#include "cli_handler.h"

using namespace testing::ext;

namespace {
int32_t g_capacityRet = 0;
int32_t g_totalEnergyRet = 0;
int32_t g_remainEnergyRet = 0;
constexpr int32_t MOCK_CAPACITY = 85;
constexpr int32_t MOCK_TOTAL_ENERGY = 4000;
constexpr int32_t MOCK_REMAIN_ENERGY = 3200;
}

namespace OHOS::PowerMgr {
int32_t BatterySrvClient::GetCapacity()
{
    return g_capacityRet;
}

int32_t BatterySrvClient::GetTotalEnergy()
{
    return g_totalEnergyRet;
}

int32_t BatterySrvClient::GetRemainEnergy()
{
    return g_remainEnergyRet;
}
}

class BatteryManagerCliTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BatteryManagerCliTest::SetUpTestCase() {}
void BatteryManagerCliTest::TearDownTestCase() {}
void BatteryManagerCliTest::SetUp()
{
    g_capacityRet = MOCK_CAPACITY;
    g_totalEnergyRet = MOCK_TOTAL_ENERGY;
    g_remainEnergyRet = MOCK_REMAIN_ENERGY;
}
void BatteryManagerCliTest::TearDown() {}

/**
 * @tc.name: BatteryManagerCliTest_001
 * @tc.desc: Test no arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_001, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char* argv[] = { prog };
    EXPECT_EQ(HandleCommand(1, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_002
 * @tc.desc: Test unknown command returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_002, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "unknown-cmd";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_003
 * @tc.desc: Test "help" subcommand is not supported (treated as unknown command)
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_003, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "help";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_004
 * @tc.desc: Test capacity command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_004, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_005
 * @tc.desc: Test capacity command with extra arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_005, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_006
 * @tc.desc: Test capacity command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_006, TestSize.Level1)
{
    g_capacityRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_007
 * @tc.desc: Test total-energy command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_007, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "total-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_008
 * @tc.desc: Test total-energy command with extra arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_008, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "total-energy";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_009
 * @tc.desc: Test total-energy command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_009, TestSize.Level1)
{
    g_totalEnergyRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "total-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_010
 * @tc.desc: Test remain-energy command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_010, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "remain-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_011
 * @tc.desc: Test remain-energy command with extra arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_011, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "remain-energy";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_012
 * @tc.desc: Test remain-energy command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_012, TestSize.Level1)
{
    g_remainEnergyRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "remain-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_013
 * @tc.desc: Test capacity with boundary value 0
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_013, TestSize.Level1)
{
    g_capacityRet = 0;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_014
 * @tc.desc: Test capacity with boundary value 100
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_014, TestSize.Level1)
{
    g_capacityRet = 100;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_015
 * @tc.desc: Test capacity with negative value should still succeed
 * (API may return negative values in edge cases, CLI should not reject)
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_015, TestSize.Level1)
{
    g_capacityRet = -1;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    // Should succeed because -1 != INVALID_BATT_INT_VALUE
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_016
 * @tc.desc: Test all commands can be called sequentially
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_016, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char capacityCmd[] = "capacity";
    char totalEnergyCmd[] = "total-energy";
    char remainEnergyCmd[] = "remain-energy";

    char* argv1[] = { prog, capacityCmd };
    EXPECT_EQ(HandleCommand(2, argv1), 0);

    char* argv2[] = { prog, totalEnergyCmd };
    EXPECT_EQ(HandleCommand(2, argv2), 0);

    char* argv3[] = { prog, remainEnergyCmd };
    EXPECT_EQ(HandleCommand(2, argv3), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_017
 * @tc.desc: Test total-energy command with zero value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_017, TestSize.Level1)
{
    g_totalEnergyRet = 0;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "total-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_018
 * @tc.desc: Test remain-energy command with zero value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_018, TestSize.Level1)
{
    g_remainEnergyRet = 0;
    char prog[] = "ohos-batteryManager";
    char cmd[] = "remain-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_019
 * @tc.desc: Test --help flag at top level shows full help and returns success
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_019, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char helpFlag[] = "--help";
    char* argv[] = { prog, helpFlag };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_020
 * @tc.desc: Test capacity --help shows subcommand help and returns success
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_020, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "capacity";
    char helpFlag[] = "--help";
    char* argv[] = { prog, cmd, helpFlag };
    EXPECT_EQ(HandleCommand(3, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_021
 * @tc.desc: Test total-energy --help shows subcommand help and returns success
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_021, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "total-energy";
    char helpFlag[] = "--help";
    char* argv[] = { prog, cmd, helpFlag };
    EXPECT_EQ(HandleCommand(3, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_022
 * @tc.desc: Test remain-energy --help shows subcommand help and returns success
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_022, TestSize.Level1)
{
    char prog[] = "ohos-batteryManager";
    char cmd[] = "remain-energy";
    char helpFlag[] = "--help";
    char* argv[] = { prog, cmd, helpFlag };
    EXPECT_EQ(HandleCommand(3, argv), 0);
}

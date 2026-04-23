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

#include "battery_info.h"
#include "battery_srv_client.h"
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
    char prog[] = "ohos-battery-manager";
    char* argv[] = { prog };
    EXPECT_EQ(HandleCommand(1, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_002
 * @tc.desc: Test unknown command returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_002, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "unknown-cmd";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_003
 * @tc.desc: Test help command returns success
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_003, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "help";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_004
 * @tc.desc: Test capacity command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_004, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
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
    char prog[] = "ohos-battery-manager";
    char cmd[] = "capacity";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_006
 * @tc.desc: Test total-energy command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_006, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "total-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_007
 * @tc.desc: Test total-energy command with extra arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_007, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "total-energy";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_008
 * @tc.desc: Test remain-energy command returns success with valid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_008, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "remain-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 0);
}

/**
 * @tc.name: BatteryManagerCliTest_009
 * @tc.desc: Test remain-energy command with extra arguments returns error
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_009, TestSize.Level1)
{
    char prog[] = "ohos-battery-manager";
    char cmd[] = "remain-energy";
    char extra[] = "extra_arg";
    char* argv[] = { prog, cmd, extra };
    EXPECT_EQ(HandleCommand(3, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_010
 * @tc.desc: Test capacity command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_010, TestSize.Level1)
{
    g_capacityRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-battery-manager";
    char cmd[] = "capacity";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_011
 * @tc.desc: Test total-energy command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_011, TestSize.Level1)
{
    g_totalEnergyRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-battery-manager";
    char cmd[] = "total-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

/**
 * @tc.name: BatteryManagerCliTest_012
 * @tc.desc: Test remain-energy command fails when service returns invalid value
 */
HWTEST_F(BatteryManagerCliTest, BatteryManagerCliTest_012, TestSize.Level1)
{
    g_remainEnergyRet = OHOS::PowerMgr::INVALID_BATT_INT_VALUE;
    char prog[] = "ohos-battery-manager";
    char cmd[] = "remain-energy";
    char* argv[] = { prog, cmd };
    EXPECT_EQ(HandleCommand(2, argv), 1);
}

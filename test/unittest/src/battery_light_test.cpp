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

#include "battery_light_test.h"
#include "battery_config.h"
#include "battery_config_test.h"
#include "battery_light.h"
#include "power_common.h"
#include <memory>

namespace OHOS {
namespace PowerMgr {
using namespace testing::ext;
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;

namespace {
BatteryLight g_light;
uint64_t g_token;
PermissionDef g_infoManagerTestPermDef = {
    .permissionName = "ohos.permission.SYSTEM_LIGHT_CONTROL",
    .bundleName = "accesstoken_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "test battery light",
    .descriptionId = 1,
    .availableLevel = APL_NORMAL
};

PermissionStateFull g_infoManagerTestState = {
    .grantFlags = {1},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.SYSTEM_LIGHT_CONTROL",
    .resDeviceID = {"local"}
};

HapPolicyParams g_infoManagerTestPolicyPrams = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {g_infoManagerTestPermDef},
    .permStateList = {g_infoManagerTestState}
};

HapInfoParams g_infoManagerTestInfoParms = {
    .bundleName = "batterylight_test",
    .userID = 1,
    .instIndex = 0,
    .appIDDesc = "BatteryLightTest"
};
} // namespace

AccessTokenID BatteryLightTest::tokenID_ = 0;

void BatteryLightTest::SetUpTestCase()
{
    AccessTokenIDEx tokenIdEx = {0};
    tokenIdEx = AccessTokenKit::AllocHapToken(g_infoManagerTestInfoParms, g_infoManagerTestPolicyPrams);
    tokenID_ = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(0, tokenID_);
    g_token = GetSelfTokenID();
    ASSERT_EQ(0, SetSelfTokenID(tokenID_));
    BatteryConfig::GetInstance().ParseConfig();
    g_light.InitLight();
    g_light.TurnOff();
    GTEST_LOG_(INFO) << "The current device supports Light: " << g_light.isAvailable();
}

void BatteryLightTest::TearDownTestCase()
{
    AccessTokenKit::DeleteToken(tokenID_);
    SetSelfTokenID(g_token);
}

void BatteryLightTest::TearDown()
{
    g_light.TurnOff();
}

/**
 * @tc.name: BatteryLight001
 * @tc.desc: Turn off light
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryLightTest, BatteryLight001, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    g_light.TurnOff();
    EXPECT_TRUE(g_light.GetLightColor() == 0U);
}

/**
 * @tc.name: BatteryLight002
 * @tc.desc: Turn on light
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryLightTest, BatteryLight002, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    uint32_t color = 0U;
    g_light.TurnOn(color);
    EXPECT_TRUE(g_light.GetLightColor() == color);
}

/**
 * @tc.name: BatteryLight003
 * @tc.desc: Update light color according to the capacity value 1
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryLightTest, BatteryLight003, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    int32_t capacity = 1;
    g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, capacity);
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::RED_LIGHT);
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, capacity));
}

/**
 * @tc.name: BatteryLight004
 * @tc.desc: Turn on light set light color
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryLightTest, BatteryLight004, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    uint32_t color = 0x7fffffff;
    g_light.TurnOn(color);
    EXPECT_TRUE(g_light.GetLightColor() == color);
}

/**
 * @tc.name: BatteryLight005
 * @tc.desc: ChargingStatus is Discharging, Not bright lights
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight005, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    // First turn on the light
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 11));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::YELLOW_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::YELLOW_LIGHT);

    EXPECT_FALSE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_NONE, 0));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::LIGHT_OFF;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
}

/**
 * @tc.name: BatteryLight006
 * @tc.desc: ChargingStatus is Charging, capacity is 9, the red light on
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight006, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 9));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::RED_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::RED_LIGHT);
}

/**
 * @tc.name: BatteryLight007
 * @tc.desc: ChargingStatus is Charging, capacity is 89, the yellow light on
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight007, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 89));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::YELLOW_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::YELLOW_LIGHT);
}

/**
 * @tc.name: BatteryLight008
 * @tc.desc: ChargingStatus is Charging, capacity is 100, the green light on
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight008, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 100));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);
}

/**
 * @tc.name: BatteryLight009
 * @tc.desc: ChargingStatus is Charging, capacity is -1, Light does not change
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight009, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    // First turn on the light
    EXPECT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 91));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);

    // Capacity invalid value, Not bright lights
    EXPECT_FALSE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, -1));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    EXPECT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);
}

/**
 * @tc.name: BatteryLight010
 * @tc.desc: The call interface was not initialized
 * @tc.type: FUNC
 */
HWTEST_F(BatteryLightTest, BatteryLight010, TestSize.Level1)
{
    BatteryLight light;
    uint32_t initColor = light.GetLightColor();
    GTEST_LOG_(INFO) << "initial:" << initColor;
    light.TurnOn(123);
    EXPECT_EQ(light.GetLightColor(), initColor);
    light.TurnOff();
    EXPECT_EQ(light.GetLightColor(), initColor);
}
} // namespace PowerMgr
} // namespace OHOS

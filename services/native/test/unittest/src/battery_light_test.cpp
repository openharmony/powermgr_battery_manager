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

#include "battery_light_test.h"
#include <memory>
#include "battery_config.h"
#include "battery_config_test.h"
#include "battery_light.h"
#include "power_common.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* SYSTEM_BATTERY_CONFIG_PATH = "/system/etc/battery/battery_config.json";

BatteryLight g_light;
}
void BatteryLightTest::SetUpTestCase()
{
    BatteryConfig::GetInstance().ParseConfig(SYSTEM_BATTERY_CONFIG_PATH);
    g_light.InitLight();

    GTEST_LOG_(INFO) << "available battery light " << g_light.isAvailable();
}

void BatteryLightTest::TearDown(void)
{
    g_light.TurnOff();
}

/**
 * @tc.name: BatteryLight001
 * @tc.desc: ChargingStatus is Discharging, Not bright lights
 * @tc.type: FUNC
 */
HWTEST_F (BatteryLightTest, BatteryLight001, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    // First turn on the light
    ASSERT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 11));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::YELLOW_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::YELLOW_LIGHT);

    ASSERT_FALSE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_NONE, 0));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::LIGHT_OFF;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
}

/**
 * @tc.name: BatteryLight002
 * @tc.desc: ChargingStatus is Charging, capacity is 9, the red light on
 * @tc.type: FUNC
 */
HWTEST_F (BatteryLightTest, BatteryLight002, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    ASSERT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 9));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::RED_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::RED_LIGHT);
}

/**
 * @tc.name: BatteryLight003
 * @tc.desc: ChargingStatus is Charging, capacity is 89, the yellow light on
 * @tc.type: FUNC
 */
HWTEST_F (BatteryLightTest, BatteryLight003, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    ASSERT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 89));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::YELLOW_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::YELLOW_LIGHT);
}

/**
 * @tc.name: BatteryLight004
 * @tc.desc: ChargingStatus is Charging, capacity is 100, the green light on
 * @tc.type: FUNC
 */
HWTEST_F (BatteryLightTest, BatteryLight004, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    ASSERT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 100));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);
}

/**
 * @tc.name: BatteryLight005
 * @tc.desc: ChargingStatus is Charging, capacity is -1, Light does not change
 * @tc.type: FUNC
 */
HWTEST_F (BatteryLightTest, BatteryLight005, TestSize.Level1)
{
    RETURN_IF(!g_light.isAvailable());
    GTEST_LOG_(INFO) << "initial:" << g_light.GetLightColor();
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::LIGHT_OFF);
    // First turn on the light
    ASSERT_TRUE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, 91));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);

    // Capacity invalid value, Not bright lights
    ASSERT_FALSE(g_light.UpdateColor(BatteryChargeState::CHARGE_STATE_ENABLE, -1));
    GTEST_LOG_(INFO) << "actual:" << g_light.GetLightColor() << "=expect:" << BatteryConfigTest::GREEN_LIGHT;
    ASSERT_EQ(g_light.GetLightColor(), BatteryConfigTest::GREEN_LIGHT);
}
} // namespace PowerMgr
} // namespace OHOS

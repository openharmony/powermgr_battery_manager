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

#include "battery_notify_test.h"

#include <string>

#include "battery_log.h"
#include "battery_notify.h"
using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
BatteryInfo* g_batteryInfo;
std::shared_ptr<BatteryNotify> g_batteryNotify;

void BatteryNotifyTest::SetUpTestCase()
{
    g_batteryNotify = std::make_shared<BatteryNotify>();
}

void BatteryNotifyTest::SetUp()
{
    g_batteryInfo = new BatteryInfo();
    const int32_t capacity = 100;
    g_batteryInfo->SetCapacity(capacity);
    const int32_t voltage = 1;
    g_batteryInfo->SetVoltage(voltage);
    const int32_t temperature = 25;
    g_batteryInfo->SetTemperature(temperature);
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_UNKNOWN;
    g_batteryInfo->SetHealthState(healthState);
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
    g_batteryInfo->SetPluggedType(pluggedType);
    const int32_t plugMaxCur = 50;
    g_batteryInfo->SetPluggedMaxCurrent(plugMaxCur);
    const int32_t plugMaxVol = 50;
    g_batteryInfo->SetPluggedMaxVoltage(plugMaxVol);
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_NONE;
    g_batteryInfo->SetChargeState(chargeState);
    const int32_t chargeCounter = 1;
    g_batteryInfo->SetChargeCounter(chargeCounter);
    const int32_t totalEnergy = 50;
    g_batteryInfo->SetTotalEnergy(totalEnergy);
    const int32_t curAverage = 10;
    g_batteryInfo->SetCurAverage(curAverage);
    const int32_t remainEnergy = 30;
    g_batteryInfo->SetRemainEnergy(remainEnergy);
    g_batteryInfo->SetPresent(true);
    const string tec = "H2";
    g_batteryInfo->SetTechnology(tec);
    const int32_t nowCur = 10;
    g_batteryInfo->SetNowCurrent(nowCur);
}

void BatteryNotifyTest::TearDown()
{
    if (g_batteryInfo != nullptr) {
        delete g_batteryInfo;
        g_batteryInfo = nullptr;
    }
}

/**
 * @tc.name: BatteryNotify001
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify001, TestSize.Level1)
{
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify002
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify002, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_GOOD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify003
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify003, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_OVERHEAT;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify004
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify004, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_OVERVOLTAGE;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify005
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify005, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_COLD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify006
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify006, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_DEAD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify007
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify007, TestSize.Level1)
{
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_BUTT;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify008
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify008, TestSize.Level1)
{
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify009
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify009, TestSize.Level1)
{
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_AC;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify010
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify010, TestSize.Level1)
{
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_USB;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify011
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify011, TestSize.Level1)
{
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_WIRELESS;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify012
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify012, TestSize.Level1)
{
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify013
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify013, TestSize.Level1)
{
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_ENABLE;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify014
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify014, TestSize.Level1)
{
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_DISABLE;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify015
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify015, TestSize.Level1)
{
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_FULL;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify016
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify016, TestSize.Level1)
{
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_BUTT;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: BatteryNotify017
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify017, TestSize.Level1)
{
    const int32_t capacity = -100;
    g_batteryInfo->SetCapacity(capacity);
    g_batteryNotify->PublishEvents(*g_batteryInfo);
}

/**
 * @tc.name: BatteryNotify018
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify018, TestSize.Level1)
{
    for (int i = 0; i < 2; i++) {
        const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_ENABLE;
        g_batteryInfo->SetChargeState(chargeState);
        g_batteryNotify->PublishEvents(*g_batteryInfo);
    }
}

/**
 * @tc.name: BatteryNotify019
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify019, TestSize.Level1)
{
    const int32_t capacity = -100;
    g_batteryInfo->SetCapacity(capacity);
    for (int i = 0; i < 2; i++) {
        g_batteryNotify->PublishEvents(*g_batteryInfo);
    }
}
} // namespace PowerMgr
} // namespace OHOS

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

#include "battery_srv_proxy_mock_test.h"
#include "battery_srv_proxy.h"
#include "ipc_object_stub.h"
#include "mock_remote_object.h"
#include <hisysevent.h>

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
std::shared_ptr<BatterySrvProxy> g_proxy;
sptr<IRemoteObject> remoteObj;
}

void BatteryProxyMockTest::SetUpTestCase()
{
#ifdef ENABLE_REMOTE_INTERFACE
    remoteObj = new MockRemoteObject();
#else
    remoteObj = new IPCObjectStub();
#endif
    g_proxy = std::make_shared<BatterySrvProxy>(remoteObj);
}

void BatteryProxyMockTest::TearDownTestCase()
{
    remoteObj = nullptr;
    g_proxy = nullptr;
}

void BatteryProxyMockTest::SetUp() {}

void BatteryProxyMockTest::TearDown() {}

namespace {
/**
 * @tc.name: BatteryProxyMockTest_001
 * @tc.desc: test BatterySrvProxy::GetCapacity() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_001, TestSize.Level0)
{
    int32_t capacity = g_proxy->GetCapacity();
    EXPECT_TRUE(capacity == INVALID_BATT_INT_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_002
 * @tc.desc: test BatterySrvProxy::GetChargingStatus() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_002, TestSize.Level0)
{
    BatteryChargeState chargeState = g_proxy->GetChargingStatus();
    EXPECT_TRUE(chargeState >= BatteryChargeState::CHARGE_STATE_NONE &&
        chargeState <= BatteryChargeState::CHARGE_STATE_BUTT); // the enum value range of BatteryChargeState
}

/**
 * @tc.name: BatteryProxyMockTest_003
 * @tc.desc: test BatterySrvProxy::GetHealthStatus() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_003, TestSize.Level0)
{
    BatteryHealthState healthState = g_proxy->GetHealthStatus();
    EXPECT_TRUE(healthState >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
        healthState <= BatteryHealthState::HEALTH_STATE_BUTT); // the enum value range of BatteryHealthState
}

/**
 * @tc.name: BatteryProxyMockTest_004
 * @tc.desc: test BatterySrvProxy::GetPluggedType() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_004, TestSize.Level0)
{
    BatteryPluggedType pluggedType = g_proxy->GetPluggedType();
    EXPECT_TRUE(pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE &&
        pluggedType <= BatteryPluggedType::PLUGGED_TYPE_BUTT); // the enum value range of BatteryPluggedType
}

/**
 * @tc.name: BatteryProxyMockTest_005
 * @tc.desc: test BatterySrvProxy::GetPresent() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_005, TestSize.Level0)
{
    bool isPresent = g_proxy->GetPresent();
    EXPECT_TRUE(isPresent == INVALID_BATT_BOOL_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_006
 * @tc.desc: test BatterySrvProxy::GetTchnology() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_006, TestSize.Level0)
{
    std::string technology = g_proxy->GetTechnology();
    EXPECT_TRUE(technology == INVALID_STRING_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_007
 * @tc.desc: test BatterySrvProxy::GetTotalEnergy() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_007, TestSize.Level0)
{
    int32_t totalEnergy = g_proxy->GetTotalEnergy();
    EXPECT_TRUE(totalEnergy == INVALID_BATT_INT_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_008
 * @tc.desc: test BatterySrvProxy::GetCurrentAverage() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_008, TestSize.Level0)
{
    int32_t currentAverage = g_proxy->GetCurrentAverage();
    EXPECT_TRUE(currentAverage == INVALID_BATT_INT_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_009
 * @tc.desc: test BatterySrvProxy::GetNowCurrent() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_009, TestSize.Level0)
{
    int32_t nowCurrent = g_proxy->GetNowCurrent();
    EXPECT_TRUE(nowCurrent == INVALID_BATT_INT_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_010
 * @tc.desc: test BatterySrvProxy::GetRemainEnergy() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_010, TestSize.Level0)
{
    int32_t remainEnergy = g_proxy->GetRemainEnergy();
    EXPECT_TRUE(remainEnergy == INVALID_BATT_INT_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_011
 * @tc.desc: test BatterySrvProxy::GetBatteryTemperature() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_011, TestSize.Level0)
{
    int32_t temperature = g_proxy->GetBatteryTemperature();
    EXPECT_TRUE(temperature == INVALID_BATT_TEMP_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_012
 * @tc.desc: test BatterySrvProxy::GetCapacityLevel() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_012, TestSize.Level0)
{
    BatteryCapacityLevel batteryLevel = g_proxy->GetCapacityLevel();
    EXPECT_TRUE(batteryLevel >= BatteryCapacityLevel::LEVEL_NONE &&
        batteryLevel <= BatteryCapacityLevel::LEVEL_RESERVED); // the enum value range of BatteryCapacityLevel
}

/**
 * @tc.name: BatteryProxyMockTest_013
 * @tc.desc: test BatterySrvProxy::GetRemainingChargeTime() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_013, TestSize.Level0)
{
    int64_t remainChargeTime = g_proxy->GetRemainingChargeTime();
    EXPECT_TRUE(remainChargeTime == INVALID_REMAINING_CHARGE_TIME_VALUE);
}

/**
 * @tc.name: BatteryProxyMockTest_014
 * @tc.desc: test BatterySrvProxy::GetVoltage() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_014, TestSize.Level0)
{
    int32_t voltage = g_proxy->GetVoltage();
    EXPECT_TRUE(voltage == INVALID_BATT_INT_VALUE);
}
} // namespace

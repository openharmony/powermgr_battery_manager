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
#include "battery_log.h"
#include "battery_info.h"
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
#include <hisysevent.h>
#endif
using namespace testing::ext;
#ifdef HAS_HIVIEWDFX_HISYSEVENT_PART
using namespace OHOS::HiviewDFX;
#endif
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
std::shared_ptr<BatterySrvProxy> g_proxy;
sptr<IRemoteObject> remoteObj;
constexpr int32_t INVALID_VALUE = -1;
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
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_001 function start!");
    int32_t capacity = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetCapacity(capacity);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(capacity == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_001 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_002
 * @tc.desc: test BatterySrvProxy::GetChargingStatus() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_002, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_002 function start!");
    uint32_t chargeStateValue = static_cast<uint32_t>(BatteryChargeState::CHARGE_STATE_NONE);
    int32_t errCode = g_proxy->GetChargingStatus(chargeStateValue);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    BatteryChargeState chargeState = static_cast<BatteryChargeState>(chargeStateValue);
    EXPECT_TRUE(chargeState >= BatteryChargeState::CHARGE_STATE_NONE &&
        chargeState <= BatteryChargeState::CHARGE_STATE_BUTT); // the enum value range of BatteryChargeState
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_002 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_003
 * @tc.desc: test BatterySrvProxy::GetHealthStatus() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_003, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_003 function start!");
    uint32_t healthStateValue = static_cast<uint32_t>(BatteryHealthState::HEALTH_STATE_UNKNOWN);
    int32_t errCode = g_proxy->GetHealthStatus(healthStateValue);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    BatteryHealthState healthState = static_cast<BatteryHealthState>(healthStateValue);
    EXPECT_TRUE(healthState >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
        healthState <= BatteryHealthState::HEALTH_STATE_BUTT); // the enum value range of BatteryHealthState
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_003 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_004
 * @tc.desc: test BatterySrvProxy::GetPluggedType() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_004, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_004 function start!");
    uint32_t pluggedTypeValue = static_cast<uint32_t>(BatteryPluggedType::PLUGGED_TYPE_NONE);
    int32_t errCode = g_proxy->GetPluggedType(pluggedTypeValue);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    BatteryPluggedType pluggedType = static_cast<BatteryPluggedType>(pluggedTypeValue);
    EXPECT_TRUE(pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE &&
        pluggedType <= BatteryPluggedType::PLUGGED_TYPE_BUTT); // the enum value range of BatteryPluggedType
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_004 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_005
 * @tc.desc: test BatterySrvProxy::GetPresent() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_005, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_005 function start!");
    bool isPresent = INVALID_BATT_BOOL_VALUE;
    int32_t errCode = g_proxy->GetPresent(isPresent);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(isPresent == INVALID_BATT_BOOL_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_005 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_006
 * @tc.desc: test BatterySrvProxy::GetTchnology() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_006, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_006 function start!");
    std::string technology = INVALID_STRING_VALUE;
    int32_t errCode = g_proxy->GetTechnology(technology);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(technology == INVALID_STRING_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_006 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_007
 * @tc.desc: test BatterySrvProxy::GetTotalEnergy() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_007, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_007 function start!");
    int32_t totalEnergy = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetTotalEnergy(totalEnergy);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(totalEnergy == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_007 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_008
 * @tc.desc: test BatterySrvProxy::GetCurrentAverage() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_008, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_008 function start!");
    int32_t currentAverage = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetCurrentAverage(currentAverage);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(currentAverage == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_008 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_009
 * @tc.desc: test BatterySrvProxy::GetNowCurrent() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_009, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_009 function start!");
    int32_t nowCurrent = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetNowCurrent(nowCurrent);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(nowCurrent == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_009 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_010
 * @tc.desc: test BatterySrvProxy::GetRemainEnergy() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_010, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_010 function start!");
    int32_t remainEnergy = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetRemainEnergy(remainEnergy);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(remainEnergy == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_010 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_011
 * @tc.desc: test BatterySrvProxy::GetBatteryTemperature() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_011, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_011 function start!");
    int32_t temperature = INVALID_BATT_TEMP_VALUE;
    int32_t errCode = g_proxy->GetBatteryTemperature(temperature);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(temperature == INVALID_BATT_TEMP_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_011 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_012
 * @tc.desc: test BatterySrvProxy::GetCapacityLevel() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_012, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_012 function start!");
    uint32_t batteryLevelValue = 0;
    int32_t errCode = g_proxy->GetCapacityLevel(batteryLevelValue);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    BatteryCapacityLevel batteryLevel = static_cast<BatteryCapacityLevel>(batteryLevelValue);
    EXPECT_TRUE(batteryLevel >= BatteryCapacityLevel::LEVEL_NONE &&
        batteryLevel <= BatteryCapacityLevel::LEVEL_RESERVED); // the enum value range of BatteryCapacityLevel
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_012 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_013
 * @tc.desc: test BatterySrvProxy::GetRemainingChargeTime() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_013, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_013 function start!");
    int64_t remainChargeTime = INVALID_REMAINING_CHARGE_TIME_VALUE;
    int32_t errCode = g_proxy->GetRemainingChargeTime(remainChargeTime);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(remainChargeTime == INVALID_REMAINING_CHARGE_TIME_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_013 function end!");
}

/**
 * @tc.name: BatteryProxyMockTest_014
 * @tc.desc: test BatterySrvProxy::GetVoltage() when an exception is raised
 * @tc.type: FUNC
 * @tc.require: issueI5X13X
 */
HWTEST_F(BatteryProxyMockTest, BatteryProxyMockTest_014, TestSize.Level0)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_014 function start!");
    int32_t voltage = INVALID_BATT_INT_VALUE;
    int32_t errCode = g_proxy->GetVoltage(voltage);
    EXPECT_TRUE(errCode == INVALID_VALUE || errCode == ERR_INVALID_VALUE || errCode == ERR_INVALID_DATA);
    EXPECT_TRUE(voltage == INVALID_BATT_INT_VALUE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryProxyMockTest_014 function end!");
}
} // namespace

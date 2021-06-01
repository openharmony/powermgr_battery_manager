/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <csignal>
#include "battery_srv_client.h"
#include "battery_service.h"
#include "power_common.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "string_ex.h"
#include "sys_param.h"
#include <iostream>

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

void BatteryServiceTest::SetUpTestCase(void)
{
}

void BatteryServiceTest::TearDownTestCase(void)
{
}

void BatteryServiceTest::SetUp(void)
{
}

void BatteryServiceTest::TearDown(void)
{
}

bool BatteryServiceTest::IsBatterySupported()
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto presentState = BatterySrvClient.GetPresent();
    auto isCar = SysParam::IsDeviceType(DeviceType::DEVICE_CAR);
    auto isTv = SysParam::IsDeviceType(DeviceType::DEVICE_TV);

    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::isCar = %{public}d, isTv = %{public}d, present=%{public}d",
        isCar, isTv, presentState);
    if (!presentState || isCar || isTv) {
        return false;
    }

    return true;
}

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions to get status of Present
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService001, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto present = BatterySrvClient.GetPresent();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::present=%{public}d", present);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(present);
    } else {
        ASSERT_FALSE(present);
    }
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService001 end.");
}

/**
 * @tc.name: BatteryService002
 * @tc.desc: Test functions to get status of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService002, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargingStatus = BatterySrvClient.GetChargingStatus();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::status=%{public}d", chargingStatus);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(chargingStatus != BatteryChargeState::CHARGE_STATE_BUTT);
    } else {
        POWER_HILOGE(MODULE_BATT_SERVICE, "BatteryServiceTest::test is disabled, do nothing");
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService002 end.");
}

/**
 * @tc.name: BatteryService003
 * @tc.desc: Test functions to get value of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService003, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthStatus = BatterySrvClient.GetHealthStatus();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::health=%{public}d", healthStatus);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(healthStatus != BatteryHealthState::HEALTH_STATE_BUTT);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService003 end.");
}

/**
 * @tc.name: BatteryService004
 * @tc.desc: Test functions of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService004, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto capacity = BatterySrvClient.GetCapacity();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::soc=%{public}d", capacity);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(capacity != INVALID_BATT_INT_VALUE);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService004 end.");
}

/**
 * @tc.name: BatteryService005
 * @tc.desc: Test functions of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService005, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto voltage = BatterySrvClient.GetVoltage();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::voltage=%{public}d", voltage);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(voltage != INVALID_BATT_INT_VALUE);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService005 end.");
}

/**
 * @tc.name: BatteryService006
 * @tc.desc: Test functions of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService006, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto temp = BatterySrvClient.GetBatteryTemperature();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::temp=%{public}d", temp);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(temp != INVALID_BATT_TEMP_VALUE);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService006 end.");
}

/**
 * @tc.name: BatteryService007
 * @tc.desc: Test functions of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService007, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto technology = BatterySrvClient.GetTechnology();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::technology=%{public}s", technology.c_str());
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(technology != INVALID_STRING_VALUE);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService007 end.");
}

/**
 * @tc.name: BatteryService008
 * @tc.desc: Test functions of BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService008, TestSize.Level1)
{
    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto type = BatterySrvClient.GetPluggedType();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::type=%{public}d", type);
    if (BatteryServiceTest::IsBatterySupported()) {
        ASSERT_TRUE(type != BatteryPluggedType::PLUGGED_TYPE_BUTT);
    }

    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService008 end.");
}


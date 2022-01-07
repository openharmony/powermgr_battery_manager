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

#include <csignal>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>
#include <sys/stat.h>

#include "power_common.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "string_ex.h"
#include "sys_param.h"

#include "batteryd_client.h"
#include "battery_srv_client.h"
#include "battery_service.h"
#include "battery_client_test.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

static sptr<BatteryService> g_service;

void BatteryClientTest::SetUpTestCase(void)
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
}

void BatteryClientTest::TearDownTestCase(void)
{
    g_service->OnStop();
    DelayedSpSingleton<BatteryService>::DestroyInstance();
}

void BatteryClientTest::SetUp(void)
{
}

void BatteryClientTest::TearDown(void)
{
}

namespace {
std::string CreateFile(std::string path, std::string content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        POWER_HILOGI(MODULE_BATT_SERVICE, "Cannot create file %{public}s", path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

void MockFileInit()
{
    std::string path = "/data/local/tmp";
    mkdir("/data/local/tmp/battery", S_IRWXU);
    mkdir("/data/local/tmp/ohos_charger", S_IRWXU);
    mkdir("/data/local/tmp/ohos-fgu", S_IRWXU);
    POWER_HILOGI(MODULE_BATT_SERVICE, "MockFileInit enter.");
    sleep(1);

    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    CreateFile("/data/local/tmp/ohos_charger/health", "Unknown");
    CreateFile("/data/local/tmp/ohos-fgu/temp", "345");
    BatterydClient::ChangePath(path);
}

/**
 * @tc.name: BatteryClient001
 * @tc.desc: Test IBatterySrv interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient001, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient001 start.");
    MockFileInit();
    CreateFile("/data/local/tmp/battery/capacity", "22");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto capacity = BatterySrvClient.GetCapacity();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryClientTest::capacity=%{public}d", capacity);
    GTEST_LOG_(INFO) << "BatteryClient::BatteryClient001 executing, capacity=" << capacity;

    ASSERT_TRUE(capacity == 22);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient001 end.");
}

/**
 * @tc.name: BatteryClient002
 * @tc.desc: Test IBatterySrv interface GetChargingStatus
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient002, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient002 start.");
    CreateFile("/data/local/tmp/battery/status", "Not charging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryClient::BatteryClient002 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_DISABLE);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient002 end.");
}

/**
 * @tc.name: BatteryClient003
 * @tc.desc: Test IBatterySrv interface GetHealthStatus
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient003, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient003 start.");
    CreateFile("/data/local/tmp/battery/health", "Cold");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryClient::BatteryClient003 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState(4));
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient003 end.");
}

/**
 * @tc.name: BatteryClient004
 * @tc.desc: Test IBatterySrv interface GetPresent
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient004, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient004 start.");
    CreateFile("/data/local/tmp/battery/present", "0");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto present = BatterySrvClient.GetPresent();
    GTEST_LOG_(INFO) << "BatteryClient::BatteryClient004 executing, present=" << present;

    ASSERT_FALSE(present);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient004 end.");
}

/**
 * @tc.name: BatteryClient005
 * @tc.desc: Test IBatterySrv interface GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient005, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient005 start.");
    CreateFile("/data/local/tmp/battery/voltage_avg", "4654321");
    CreateFile("/data/local/tmp/battery/voltage_now", "4654321");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto voltage = BatterySrvClient.GetVoltage();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryClientTest::voltage=%{public}d", voltage);

    ASSERT_TRUE(voltage == 4654321);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient005 end.");
}

/**
 * @tc.name: BatteryClient006
 * @tc.desc: Test IBatterySrv interface GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient006, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient006 start.");
    CreateFile("/data/local/tmp/battery/temp", "222");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto temperature = BatterySrvClient.GetBatteryTemperature();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryClientTest::voltage=%{public}d", temperature);

    ASSERT_TRUE(temperature == 222);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient006 end.");
}

/**
 * @tc.name: BatteryClient007
 * @tc.desc: Test IBatterySrv interface GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient007, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient007 start.");
    CreateFile("/data/local/tmp/ohos-fgu/technology", "H2");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto technology = BatterySrvClient.GetTechnology();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryClientTest::technology=%{public}s", technology.c_str());

    ASSERT_TRUE(technology == "H2");
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient007 end.");
}

/**
 * @tc.name: BatteryClient008
 * @tc.desc: Test IBatterySrv interface GetPluggedType
 * @tc.type: FUNC
 */
HWTEST_F (BatteryClientTest, BatteryClient008, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient008 start.");
    CreateFile("/data/local/tmp/ohos_charger/type", "USB");
    CreateFile("/data/local/tmp/battery/type", "USB");
    CreateFile("/data/local/tmp/ohos-fgu/type", "USB");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryClientTest::pluggedType=%{public}d", pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_USB);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryClient::BatteryClient008 end.");
}
}

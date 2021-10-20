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

#include <iostream>
#include <csignal>
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

#include "battery_service.h"
#include "power_common.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "string_ex.h"
#include "sys_param.h"
#include "batteryd_client.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS::EventFwk;
using namespace OHOS;
using namespace std;

static sptr<BatteryService> service;

void BatteryServiceTest::SetUpTestCase(void)
{
    service = DelayedSpSingleton<BatteryService>::GetInstance();
    service->OnStart();
}

void BatteryServiceTest::TearDownTestCase(void)
{
    service->OnStop();
    DelayedSpSingleton<BatteryService>::DestroyInstance();
}

void BatteryServiceTest::SetUp(void)
{
}

void BatteryServiceTest::TearDown(void)
{
}

const char *CreateFile(const char *path, const char *content)
{
    std::ofstream stream(path);
    if (!stream.is_open()) {
        POWER_HILOGI(MODULE_BATT_SERVICE, "Cannot create file %{public}s", path);
        return nullptr;
    }
    stream << content << std::endl;
    stream.close();
    return path;
}

void MockFileInit()
{
    std::string path = "/data/local/tmp";
    mkdir("/data/local/tmp/battery", S_IRWXU);
    mkdir("/data/local/tmp/bq2560x_charger", S_IRWXU);
    mkdir("/data/local/tmp/sc27xx-fgu", S_IRWXU);
    POWER_HILOGI(MODULE_BATT_SERVICE, "MockFileInit enter.");
    sleep(1);

    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    CreateFile("/data/local/tmp/bq2560x_charger/health", "Unknown");
    CreateFile("/data/local/tmp/sc27xx-fgu/temp", "345");
    CreateFile("/data/local/tmp/battery/capacity", "50");
    CreateFile("/data/local/tmp/battery/status", "Charging");
    CreateFile("/data/local/tmp/battery/health", "Good");
    CreateFile("/data/local/tmp/battery/present", "1");
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");
    CreateFile("/data/local/tmp/battery/temp", "333");
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li");
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Wireless");
    BatterydClient::ChangePath(path);
}

/**
 * @tc.name: BatteryService001
 * @tc.desc: Test functions GetCapacity in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService001, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService001 start.");
    MockFileInit();
    CreateFile("/data/local/tmp/battery/capacity", "50");

    auto capacity = service->GetCapacity();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::capacity=%{public}d", capacity);
    GTEST_LOG_(INFO) << "BatteryService::BatteryService001 executing, capacity=" << capacity;

    ASSERT_TRUE(capacity == 50);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService001 end.");
}

/**
 * @tc.name: BatteryService002
 * @tc.desc: Test functions GetChargingStatus in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService002, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService002 start.");
    CreateFile("/data/local/tmp/battery/status", "Charging");

    auto chargeState = service->GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryService::BatteryService002 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_ENABLE);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService002 end.");
}

/**
 * @tc.name: BatteryService003
 * @tc.desc: Test functions GetHealthStatus in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService003, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService003 start.");
    CreateFile("/data/local/tmp/battery/health", "Good");

    auto healthState = service->GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryService::BatteryService003 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState(1));
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService003 end.");
}

/**
 * @tc.name: BatteryService004
 * @tc.desc: Test functions GetPresent in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService004, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService004 start.");
    CreateFile("/data/local/tmp/battery/present", "1");

    auto present = service->GetPresent();
    GTEST_LOG_(INFO) << "BatteryService::BatteryService004 executing, present=" << present;

    ASSERT_TRUE(present);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService004 end.");
}

/**
 * @tc.name: BatteryService005
 * @tc.desc: Test functions GetVoltage in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService005, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService005 start.");
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");

    auto voltage = service->GetVoltage();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::voltage=%{public}d", voltage);

    ASSERT_TRUE(voltage == 4123456);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService005 end.");
}

/**
 * @tc.name: BatteryService006
 * @tc.desc: Test functions GetBatteryTemperature in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService006, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService006 start.");
    CreateFile("/data/local/tmp/battery/temp", "333");

    auto temperature = service->GetBatteryTemperature();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::voltage=%{public}d", temperature);

    ASSERT_TRUE(temperature == 333);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService006 end.");
}

/**
 * @tc.name: BatteryService007
 * @tc.desc: Test functions GetTechnology in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService007, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService007 start.");
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li");

    auto technology = service->GetTechnology();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::technology=%{public}s", technology.c_str());

    ASSERT_TRUE(technology == "Li");
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService007 end.");
}

/**
 * @tc.name: BatteryService008
 * @tc.desc: Test functions GetPluggedType in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService008, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService008 start.");
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Wireless");

    auto pluggedType = service->GetPluggedType();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::pluggedType=%{public}d", pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService008 end.");
}

/**
 * @tc.name: BatteryService009
 * @tc.desc: Test functions OnStart in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService009, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService009 start.");

    service->OnStart();
    bool ready = service->IsServiceReady();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::ready=%{public}d", ready);

    ASSERT_TRUE(ready);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService009 end.");
}

/**
 * @tc.name: BatteryService010
 * @tc.desc: Test functions OnStop in BatteryService
 * @tc.type: FUNC
 */
HWTEST_F (BatteryServiceTest, BatteryService010, TestSize.Level1)
{
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService010 start.");

    service->OnStop();
    bool ready = service->IsServiceReady();
    POWER_HILOGI(MODULE_BATT_SERVICE, "BatteryServiceTest::ready=%{public}d", ready);

    ASSERT_FALSE(ready);
    POWER_HILOGD(MODULE_BATT_SERVICE, "BatteryService::BatteryService010 end.");
}

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

#include "module_test.h"

#include <iostream>
#include <csignal>
#include <fstream>
#include <memory>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>
#include <sys/stat.h>

#include "utils/hdf_log.h"
#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_thread_test.h"
#include "batteryd_client.h"
#include "battery_service.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_0;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

void ModuleTest::SetUpTestCase(void)
{
}

void ModuleTest::TearDownTestCase(void)
{
}

void ModuleTest::SetUp(void)
{
}

void ModuleTest::TearDown(void)
{
}

std::string CreateFile(std::string path, std::string content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: enter, Cannot create file %{public}s", __func__, path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

static void MockFileInit()
{
    std::string path = "/data/local/tmp";
    mkdir("/data/local/tmp/battery", S_IRWXU);
    mkdir("/data/local/tmp/ohos_charger", S_IRWXU);
    mkdir("/data/local/tmp/ohos-fgu", S_IRWXU);
    HDF_LOGD("%{public}s: enter.", __func__);

    sleep(1);
    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    CreateFile("/data/local/tmp/ohos_charger/health", "Unknown");
    CreateFile("/data/local/tmp/ohos-fgu/temp", "345");
    BatterydClient::ChangePath(path);
}

/**
 * @tc.name: ModuleTest001
 * @tc.desc: Test BindBatterydSubscriber return ERR_OK
 * @tc.type: FUNC
 */
static HWTEST_F (ModuleTest, ModuleTest001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest001 start.", __func__);
    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    HDF_LOGD("%{public}s: enter. ModuleTest001::errCode=%{public}d.", __func__, errCode);

    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    HDF_LOGD("%{public}s: enter. ModuleTest001 end.", __func__);
}

/**
 * @tc.name: ModuleTest002
 * @tc.desc: Test UnbindBatterydSubscriber return ERR_OK
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest002 start.", __func__);
    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    HDF_LOGD("%{public}s: enter. ModuleTest001::errCode=%{public}d.", __func__, errCode);

    errCode = BatterydClient::UnbindBatterydSubscriber();
    HDF_LOGD("%{public}s: enter. ModuleTest002::errCode=%{public}d.", __func__, errCode);

    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    HDF_LOGD("%{public}s: enter. ModuleTest002 end.", __func__);
}

/**
 * @tc.name: ModuleTest003
 * @tc.desc: Test BindBatterydSubscriber once but Unbind more, not return ERR_OK
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest003 start.", __func__);
    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    errCode = BatterydClient::UnbindBatterydSubscriber();
    HDF_LOGD("%{public}s: enter. ModuleTest003::errCode=%{public}d.", __func__, errCode);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    errCode = BatterydClient::UnbindBatterydSubscriber();
    HDF_LOGD("%{public}s: enter. ModuleTest003::errCode=%{public}d.", __func__, errCode);
    ASSERT_TRUE(errCode != OHOS::ERR_OK);

    HDF_LOGD("%{public}s: enter. ModuleTest003 end.", __func__);
}

/**
 * @tc.name: ModuleTest004
 * @tc.desc: Test multi times call BindBatterydSubscriber return ERR_OK
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest004 start.", __func__);
    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);
    HDF_LOGD("%{public}s: enter. ModuleTest004::errCode=%{public}d.", __func__, errCode);
    errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    HDF_LOGD("%{public}s: enter. ModuleTest004 end.", __func__);
}

/**
 * @tc.name: ModuleTest005
 * @tc.desc: Test functions of HdiClient GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest005, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest005 start.", __func__);
    MockFileInit();
    CreateFile("/data/local/tmp/battery/temp", "234");

    auto temperature = BatterydClient::GetTemperature();
    HDF_LOGD("%{public}s: enter. ModuleTest005::temperature=%{public}d.", __func__, temperature);

    ASSERT_TRUE(temperature == 234);

    HDF_LOGD("%{public}s: enter. ModuleTest005 end.", __func__);
}

/**
 * @tc.name: ModuleTest006
 * @tc.desc: Test functions of HdiClient GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest006, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest006 start.", __func__);
    CreateFile("/data/local/tmp/battery/voltage_avg", "4000000");
    CreateFile("/data/local/tmp/battery/voltage_now", "4000000");

    auto voltage = BatterydClient::GetVoltage();
    HDF_LOGD("%{public}s: enter. ModuleTest006::voltage=%{public}d.", __func__, voltage);

    ASSERT_TRUE(voltage == 4000000);

    HDF_LOGD("%{public}s: enter. ModuleTest006 end.", __func__);
}

/**
 * @tc.name: ModuleTest007
 * @tc.desc: Test functions of HdiClient GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest007, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest007 start.", __func__);
    CreateFile("/data/local/tmp/battery/capacity", "22");

    auto capacity = BatterydClient::GetCapacity();
    HDF_LOGD("%{public}s: enter. ModuleTest007::capacity=%{public}d.", __func__, capacity);

    ASSERT_TRUE(capacity == 22);

    HDF_LOGD("%{public}s: enter. ModuleTest007 end.", __func__);
}

/**
 * @tc.name: ModuleTest008
 * @tc.desc: Test functions of HdiClient GetHealthState
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest008, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest008 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Cold");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. ModuleTest008::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_COLD);

    HDF_LOGD("%{public}s: enter. ModuleTest008 end.", __func__);
}

/**
 * @tc.name: ModuleTest009
 * @tc.desc: Test functions of HdiClient GetPluggedType
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest009, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest009 start.", __func__);
    CreateFile("/data/local/tmp/ohos_charger/type", "USB");
    CreateFile("/data/local/tmp/battery/type", "USB");
    CreateFile("/data/local/tmp/ohos-fgu/type", "USB");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. ModuleTest009::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_USB);

    HDF_LOGD("%{public}s: enter. ModuleTest009 end.", __func__);
}

/**
 * @tc.name: ModuleTest010
 * @tc.desc: Test functions of HdiClient GetChargeState
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest010, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest010 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Charging");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. ModuleTest010::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_ENABLE);

    HDF_LOGD("%{public}s: enter. ModuleTest010 end.", __func__);
}

/**
 * @tc.name: ModuleTest011
 * @tc.desc: Test functions of HdiClient GetPresent
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest011, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest011 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "0");

    auto present = BatterydClient::GetPresent();
    HDF_LOGD("%{public}s: enter. ModuleTest011::present=%{public}d.", __func__, present);

    ASSERT_FALSE(present);

    HDF_LOGD("%{public}s: enter. ModuleTest011 end.", __func__);
}

/**
 * @tc.name: ModuleTest012
 * @tc.desc: Test functions of HdiClient GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (ModuleTest, ModuleTest012, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. ModuleTest012 start.", __func__);
    CreateFile("/data/local/tmp/ohos-fgu/technology", "LiFePO4");

    auto technology = BatterydClient::GetTechnology();
    HDF_LOGD("%{public}s: enter. ModuleTest012::technology=%{public}s.", __func__, technology.c_str());

    ASSERT_TRUE(technology == "LiFePO4");

    HDF_LOGD("%{public}s: enter. ModuleTest012 end.", __func__);
}

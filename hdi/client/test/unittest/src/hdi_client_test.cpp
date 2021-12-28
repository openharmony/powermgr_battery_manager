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

#include "hdi_client_test.h"

#include <csignal>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <streambuf>
#include <string>
#include <thread>
#include <vector>
#include <sys/stat.h>

#include "utils/hdf_log.h"
#include "batteryd_client.h"
#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_thread_test.h"
#include "battery_service.h"
#include "hdf_device_desc.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace OHOS::HDI::Battery::V1_0;
using namespace std;

void HdiClientTest::SetUpTestCase(void)
{
}

void HdiClientTest::TearDownTestCase(void)
{
}

void HdiClientTest::SetUp(void)
{
}

void HdiClientTest::TearDown(void)
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
    mkdir("/data/local/tmp/bq2560x_charger", S_IRWXU);
    mkdir("/data/local/tmp/sc27xx-fgu", S_IRWXU);
    HDF_LOGD("%{public}s: enter.", __func__);

    sleep(1);
    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    CreateFile("/data/local/tmp/bq2560x_charger/health", "Unknown");
    CreateFile("/data/local/tmp/sc27xx-fgu/temp", "345");
    BatterydClient::ChangePath(path);
}

/**
 * @tc.name: HdiClient001
 * @tc.desc: Test functions of HdiClient GetTemperature
 * @tc.type: FUNC
 */
static HWTEST_F (HdiClientTest, HdiClient001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient001 start.", __func__);
    MockFileInit();
    CreateFile("/data/local/tmp/battery/temp", "567");

    auto temperature = BatterydClient::GetTemperature();
    HDF_LOGD("%{public}s: enter. HdiClient001::temperature=%{public}d.", __func__, temperature);

    ASSERT_TRUE(temperature == 567);

    HDF_LOGD("%{public}s: enter. HdiClient001 end.", __func__);
}

/**
 * @tc.name: HdiClient002
 * @tc.desc: Test functions of HdiClient GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient002 start.", __func__);
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");

    auto voltage = BatterydClient::GetVoltage();
    HDF_LOGD("%{public}s: enter. HdiClient002::voltage=%{public}d.", __func__, voltage);

    ASSERT_TRUE(voltage == 4123456);

    HDF_LOGD("%{public}s: enter. HdiClient002 end.", __func__);
}

/**
 * @tc.name: HdiClient003
 * @tc.desc: Test functions of HdiClient GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient003 start.", __func__);
    CreateFile("/data/local/tmp/battery/capacity", "11");

    auto capacity = BatterydClient::GetCapacity();
    HDF_LOGD("%{public}s: enter. HdiClient003::capacity=%{public}d.", __func__, capacity);

    ASSERT_TRUE(capacity == 11);

    HDF_LOGD("%{public}s: enter. HdiClient003 end.", __func__);
}

/**
 * @tc.name: HdiClient004
 * @tc.desc: Test functions of HdiClient GetHealthState
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient004 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Good");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. HdiClient004::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_GOOD);

    HDF_LOGD("%{public}s: enter. HdiClient004 end.", __func__);
}

/**
 * @tc.name: HdiClient005
 * @tc.desc: Test functions of HdiClient GetPluggedType
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient005, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient005 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Mains");
    CreateFile("/data/local/tmp/battery/type", "Mains");
    CreateFile("/data/local/tmp/sc27xx-fgu/type", "Mains");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. HdiClient005::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);

    HDF_LOGD("%{public}s: enter. HdiClient005 end.", __func__);
}

/**
 * @tc.name: HdiClient006
 * @tc.desc: Test functions of HdiClient GetChargeState
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient006, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient006 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Not charging");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. HdiClient006::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_DISABLE);

    HDF_LOGD("%{public}s: enter. HdiClient006 end.", __func__);
}

/**
 * @tc.name: HdiClient007
 * @tc.desc: Test functions of HdiClient GetPresent
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient007, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient007 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "1");

    auto present = BatterydClient::GetPresent();
    HDF_LOGD("%{public}s: enter. HdiClient007::present=%{public}d.", __func__, present);

    ASSERT_TRUE(present);

    HDF_LOGD("%{public}s: enter. HdiClient007 end.", __func__);
}

/**
 * @tc.name: HdiClient008
 * @tc.desc: Test functions of HdiClient GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient008, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient008 start.", __func__);
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li");

    auto technology = BatterydClient::GetTechnology();
    HDF_LOGD("%{public}s: enter. HdiClient008::technology=%{public}s.", __func__, technology.c_str());

    ASSERT_TRUE(technology == "Li");

    HDF_LOGD("%{public}s: enter. HdiClient008 end.", __func__);
}

/**
 * @tc.name: HdiClient009
 * @tc.desc: Test functions of HdiClient BindBatterydSubscriber
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient009, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient009 start.", __func__);

    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);
    HDF_LOGD("%{public}s: enter. HdiClient009::errCode=%{public}d.", __func__, errCode);

    HDF_LOGD("%{public}s: enter. HdiClient009 end.", __func__);
}

/**
 * @tc.name: HdiClient010
 * @tc.desc: Test functions of HdiClient UnbindBatterydSubscriber
 * @tc.type: FUNC
 */
HWTEST_F (HdiClientTest, HdiClient010, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiClient010 start.", __func__);

    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    errCode = BatterydClient::UnbindBatterydSubscriber();
    HDF_LOGD("%{public}s: enter. HdiClient010::errCode=%{public}d.", __func__, errCode);

    ASSERT_TRUE(errCode == OHOS::ERR_OK);

    HDF_LOGD("%{public}s: enter. HdiClient010 end.", __func__);
}

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

#include "hdi_service_test.h"

#include <csignal>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
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
#include "battery_vibrate.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_0;
using namespace OHOS::PowerMgr;
using namespace std;

namespace HdiServiceTest {
void HdiServiceTest::SetUpTestCase(void)
{
}

void HdiServiceTest::TearDownTestCase(void)
{
}

void HdiServiceTest::SetUp(void)
{
}

void HdiServiceTest::TearDown(void)
{
}

const char *CreateFile(const char *path, const char *content)
{
    std::ofstream stream(path);
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: Cannot create file %{public}s", __func__, path);
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
    HDF_LOGD("%{public}s: enter.", __func__);

    sleep(1);
    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    CreateFile("/data/local/tmp/bq2560x_charger/health", "Unknown");
    CreateFile("/data/local/tmp/sc27xx-fgu/temp", "345");
    BatterydClient::ChangePath(path);
}

/**
 * @tc.name: HdiService001
 * @tc.desc: Test functions of ParseTemperature
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService001 start.", __func__);
    MockFileInit();
    CreateFile("/data/local/tmp/battery/temp", "567");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t temperature = 0;
    provider->ParseTemperature(&temperature);
    HDF_LOGD("%{public}s: HdiService001::temperature=%{public}d.", __func__, temperature);

    ASSERT_TRUE(temperature == 567);
    HDF_LOGD("%{public}s: HdiService001 end.", __func__);
}

/**
 * @tc.name: HdiService002
 * @tc.desc: Test functions of ParseVoltage
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService002 start.", __func__);
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t voltage = 0;
    provider->ParseVoltage(&voltage);
    HDF_LOGD("%{public}s: HdiService002::voltage=%{public}d.", __func__, voltage);

    ASSERT_TRUE(voltage == 4123456);
    HDF_LOGD("%{public}s: HdiService002 end.", __func__);
}

/**
 * @tc.name: HdiService003
 * @tc.desc: Test functions of ParseCapacity
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService003 start.", __func__);
    CreateFile("/data/local/tmp/battery/capacity", "11");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t capacity = -1;
    provider->ParseCapacity(&capacity);
    HDF_LOGD("%{public}s: HdiService003::capacity=%{public}d.", __func__, capacity);

    ASSERT_TRUE(capacity == 11);
    HDF_LOGD("%{public}s: HdiService003 end.", __func__);
}

/**
 * @tc.name: HdiService004
 * @tc.desc: Test functions of ParseHealthState
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService004 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Good");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t healthState = -1;
    provider->ParseHealthState(&healthState);
    HDF_LOGD("%{public}s: HdiService004::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == 1);
    HDF_LOGD("%{public}s: HdiService004 end.", __func__);
}

/**
 * @tc.name: HdiService005
 * @tc.desc: Test functions of ParsePluggedType
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService005, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService005 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Wireless");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t pluggedType = PowerSupplyProvider::PLUGGED_TYPE_NONE;
    provider->ParsePluggedType(&pluggedType);
    HDF_LOGD("%{public}s: HdiService005::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == PowerSupplyProvider::PLUGGED_TYPE_WIRELESS);
    HDF_LOGD("%{public}s: HdiService005 end.", __func__);
}

/**
 * @tc.name: HdiService006
 * @tc.desc: Test functions of ParseChargeState
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService006, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService006 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Not charging");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t chargeState = PowerSupplyProvider::CHARGE_STATE_RESERVED;
    provider->ParseChargeState(&chargeState);
    HDF_LOGD("%{public}s: HdiService006::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == PowerSupplyProvider::CHARGE_STATE_DISABLE);
    HDF_LOGD("%{public}s: HdiService006 end.", __func__);
}

/**
 * @tc.name: HdiService007
 * @tc.desc: Test functions of ParseChargeCounter
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService007, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService007 start.", __func__);
    CreateFile("/data/local/tmp/battery/charge_counter", "12345");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int32_t chargeCounter = -1;
    provider->ParseChargeCounter(&chargeCounter);
    HDF_LOGD("%{public}s: HdiService007::chargeCounter=%{public}d.", __func__, chargeCounter);

    ASSERT_TRUE(chargeCounter == 12345);
    HDF_LOGD("%{public}s: HdiService007 end.", __func__);
}

/**
 * @tc.name: HdiService008
 * @tc.desc: Test functions of ParsePresent
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService008, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService008 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "2");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    int8_t present = -1;
    provider->ParsePresent(&present);
    HDF_LOGD("%{public}s: HdiService008::present=%{public}d.", __func__, present);

    ASSERT_TRUE(present == 2);
    HDF_LOGD("%{public}s: HdiService008 end.", __func__);
}

/**
 * @tc.name: HdiService009
 * @tc.desc: Test functions to get value of technology
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService009, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService009 start.", __func__);
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li");

    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }
    std::string path = "/data/local/tmp";
    provider->SetSysFilePath(path);
    provider->InitPowerSupplySysfs();

    std::string technology = "invalid";
    provider->ParseTechnology(technology);
    HDF_LOGD("%{public}s: HdiService009::technology=%{public}s.", __func__, technology.c_str());

    ASSERT_TRUE(technology == "Li");
    HDF_LOGD("%{public}s: HdiService009 end.", __func__);
}

/**
 * @tc.name: HdiService010
 * @tc.desc: Test functions to get fd of socket
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService010, TestSize.Level1)
{
    using namespace OHOS::HDI::Battery::V1_0;
    HDF_LOGD("%{public}s: HdiService010 start.", __func__);

    BatteryThread bt;
    auto fd = OpenUeventSocketTest(bt);
    HDF_LOGD("%{public}s: HdiService010::fd=%{public}d.", __func__, fd);

    ASSERT_TRUE(fd > 0);
    close(fd);
    HDF_LOGD("%{public}s: HdiService010 end.", __func__);
}

/**
 * @tc.name: HdiService011
 * @tc.desc: Test functions UpdateEpollInterval when charge-online
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService011, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService011 start.", __func__);
    const int32_t CHARGE_STATE_ENABLE = 1;
    BatteryThread bt;

    UpdateEpollIntervalTest(CHARGE_STATE_ENABLE, bt);
    auto epollInterval = GetEpollIntervalTest(bt);
    HDF_LOGD("%{public}s: HdiService011::epollInterval=%{public}d.", __func__, epollInterval);

    ASSERT_TRUE(epollInterval == 2000);
    HDF_LOGD("%{public}s: HdiService011 end.", __func__);
}

/**
 * @tc.name: HdiService012
 * @tc.desc: Test functions UpdateEpollInterval when charge-offline
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService012, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService012 start.", __func__);
    const int32_t CHARGE_STATE_NONE = 0;
    BatteryThread bt;

    UpdateEpollIntervalTest(CHARGE_STATE_NONE, bt);
    auto epollInterval = GetEpollIntervalTest(bt);
    HDF_LOGD("%{public}s: HdiService012::epollInterval=%{public}d.", __func__, epollInterval);

    ASSERT_TRUE(epollInterval == -1);
    HDF_LOGD("%{public}s: HdiService012 end.", __func__);
}

/**
 * @tc.name: HdiService013
 * @tc.desc: Test functions Init
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService013, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService013 start.", __func__);
    void *service = nullptr;
    BatteryThread bt;

    InitTest(service, bt);
    auto epollFd = GetEpollFdTest(bt);
    HDF_LOGD("%{public}s: HdiService013::epollFd=%{public}d", __func__, epollFd);

    ASSERT_TRUE(epollFd > 0);
    HDF_LOGD("%{public}s: HdiService013 end.", __func__);
}

/**
 * @tc.name: HdiService014
 * @tc.desc: Test functions InitTimer
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService014, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService014 start.", __func__);
    BatteryThread bt;

    InitTimerTest(bt);
    auto timerFd = GetTimerFdTest(bt);
    HDF_LOGD("%{public}s: HdiService014::timerFd==%{public}d", __func__, timerFd);

    ASSERT_TRUE(timerFd > 0);
    HDF_LOGD("%{public}s: HdiService014 end.", __func__);
}

/**
 * @tc.name: HdiService015
 * @tc.desc: Test functions GetLedConf in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService015, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService01 start.", __func__);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    conf->Init();
    std::vector<BatteryConfig::LedConf> ledConf = conf->GetLedConf();

    ASSERT_TRUE(ledConf[0].capacityBegin == 0);
    ASSERT_TRUE(ledConf[0].capacityEnd == 10);
    ASSERT_TRUE(ledConf[0].color == 4);
    ASSERT_TRUE(ledConf[0].brightness == 255);
    ASSERT_TRUE(ledConf[1].capacityBegin == 10);
    ASSERT_TRUE(ledConf[1].capacityEnd == 90);
    ASSERT_TRUE(ledConf[1].color == 6);
    ASSERT_TRUE(ledConf[1].brightness == 255);
    ASSERT_TRUE(ledConf[2].capacityBegin == 90);
    ASSERT_TRUE(ledConf[2].capacityEnd == 100);
    ASSERT_TRUE(ledConf[2].color == 2);
    ASSERT_TRUE(ledConf[2].brightness == 255);
    HDF_LOGD("%{public}s: HdiService015 end.", __func__);
}

/**
 * @tc.name: HdiService016
 * @tc.desc: Test functions GetTempConf in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService016, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService016 start.", __func__);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    conf->Init();
    auto tempConf = conf->GetTempConf();

    ASSERT_TRUE(tempConf.lower == -100);
    ASSERT_TRUE(tempConf.upper == 600);
    HDF_LOGD("%{public}s: HdiService016 end.", __func__);
}

/**
 * @tc.name: HdiService017
 * @tc.desc: Test functions GetCapacityConf in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService017, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService017 start.", __func__);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    conf->Init();
    auto capacityConf = conf->GetCapacityConf();

    ASSERT_TRUE(capacityConf == 3);
    HDF_LOGD("%{public}s: HdiService017 end.", __func__);
}

/**
 * @tc.name: HdiService018
 * @tc.desc: Test functions ParseLedInfo in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService018, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService018 start.", __func__);
    std::string filename = "error_path/system/etc/ledconfig/led_config.json";
    BatteryConfig bc;

    ParseConfigTest(filename, bc);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    auto ledConf = conf->GetLedConf();
    ASSERT_TRUE(ledConf.empty());
    HDF_LOGD("%{public}s: HdiService018 end.", __func__);
}

/**
 * @tc.name: HdiService019
 * @tc.desc: Test functions ParseTemperatureInfo in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService019, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService019 start.", __func__);
    std::string filename = "error_path/system/etc/ledconfig/led_config.json";
    BatteryConfig bc;

    ParseConfigTest(filename, bc);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    auto tempConf = conf->GetTempConf();

    ASSERT_TRUE(tempConf.lower != -100);
    ASSERT_TRUE(tempConf.upper != 600);
    HDF_LOGD("%{public}s: HdiService019 end.", __func__);
}

/**
 * @tc.name: HdiService020
 * @tc.desc: Test functions ParseSocInfo in BatteryConfig
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService020, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService01 start.", __func__);
    std::string filename = "error_path/system/etc/ledconfig/led_config.json";
    BatteryConfig bc;

    ParseConfigTest(filename, bc);
    std::unique_ptr<BatteryConfig> conf = std::make_unique<BatteryConfig>();
    auto capacityConf = conf->GetCapacityConf();

    ASSERT_TRUE(capacityConf != 3);
    HDF_LOGD("%{public}s: HdiService020 end.", __func__);
}

/**
 * @tc.name: HdiService021
 * @tc.desc: Test functions VibrateInit in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService021, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService021 start.", __func__);

    std::unique_ptr<BatteryVibrate> vibrate = std::make_unique<BatteryVibrate>();
    auto ret = vibrate->VibrateInit();
    ASSERT_TRUE(ret == -1);

    HDF_LOGD("%{public}s: HdiService021 end.", __func__);
}

/**
 * @tc.name: HdiService022
 * @tc.desc: Test functions CycleMatters in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService022, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService022 start.", __func__);
    ChargerThread ct;

    ChargerThreadInitTest(ct);
    CycleMattersTest(ct);
    auto getBatteryInfo = GetBatteryInfoTest(ct);

    ASSERT_TRUE(getBatteryInfo);
    HDF_LOGD("%{public}s: HdiService022 end.", __func__);
}

/**
 * @tc.name: HdiService023
 * @tc.desc: Test functions BatteryHostServiceStub::Init
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService023, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService023 start.", __func__);
    std::unique_ptr<BatteryHostServiceStub> stub = std::make_unique<BatteryHostServiceStub>();
    auto ret = stub->Init();
    HDF_LOGD("%{public}s: HdiService023::ret==%{public}d", __func__, ret);

    ASSERT_TRUE(ret == HDF_SUCCESS);
    HDF_LOGD("%{public}s: HdiService023 end.", __func__);
}

/**
 * @tc.name: HdiService024
 * @tc.desc: Test functions SetTimerInterval
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService024, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService024 start.", __func__);
    BatteryThread bt;

    SetTimerFdTest(2, bt);
    SetTimerIntervalTest(5, bt);
    int interval = GetTimerIntervalTest(bt);
    HDF_LOGD("%{public}s: HdiService024::interval==%{public}d", __func__, interval);

    ASSERT_TRUE(interval == 5);
    HDF_LOGD("%{public}s: HdiService024 end.", __func__);
}

/**
 * @tc.name: HdiService025
 * @tc.desc: Test functions HandleBacklight
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService025, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService025 start.", __func__);
    std::unique_ptr<BatteryBacklight> backlight = std::make_unique<BatteryBacklight>();
    auto ret = backlight->HandleBacklight(0);
    HDF_LOGD("%{public}s: HdiService025::ret==%{public}d", __func__, ret);
    backlight->TurnOnScreen();

    ASSERT_TRUE(ret != -1);
    HDF_LOGD("%{public}s: HdiService024 end.", __func__);
}
}

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

#include "battery_sttest.h"

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
#include <fcntl.h>
#include <sys/types.h>

#include "utils/hdf_log.h"
#include "batteryd_client.h"
#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_service.h"
#include "hdf_device_desc.h"
#include "battery_srv_client.h"
#include "battery_thread_test.h"
#include "animation_label.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace OHOS::HDI::Battery::V1_0;
using namespace std;

static sptr<BatteryService> service;

void BatterySttest::SetUpTestCase(void)
{
    service = DelayedSpSingleton<BatteryService>::GetInstance();
    service->OnStart();
}

void BatterySttest::TearDownTestCase(void)
{
    service->OnStop();
    DelayedSpSingleton<BatteryService>::DestroyInstance();
}

void BatterySttest::SetUp(void)
{
}

void BatterySttest::TearDown(void)
{
}

const char *CreateFile(const char *path, const char *content)
{
    std::ofstream stream(path);
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: enter, Cannot create file %{public}s", __func__, path);
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

int32_t ReadRedLedSysfs()
{
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string redLedPath = "/data/local/tmp/leds/sc27xx:red/brightness";

    int fd = open(redLedPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{private}s", __func__, redLedPath.c_str());
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, redLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t redcolor = strtol(buf, nullptr, strlen);
    close(fd);

    return redcolor;
}

int32_t ReadGreenLedSysfs()
{
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string greenLedPath = "/data/local/tmp/leds/sc27xx:green/brightness";

    int fd = open(greenLedPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{private}s", __func__, greenLedPath.c_str());
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, greenLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t greencolor = strtol(buf, nullptr, strlen);
    close(fd);

    return greencolor;
}

int32_t ReadBlueLedSysfs()
{
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string blueLedPath = "/data/local/tmp/leds/sc27xx:blue/brightness";

    int fd = open(blueLedPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{private}s", __func__, blueLedPath.c_str());
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, blueLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t bluecolor = strtol(buf, nullptr, strlen);
    close(fd);

    return bluecolor;
}

/**
 * @tc.name: BatteryST001
 * @tc.desc: Test functions of HDI Interface GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST001 start.", __func__);
    MockFileInit();
    CreateFile("/data/local/tmp/battery/temp", "567");

    auto temperature = BatterydClient::GetTemperature();
    HDF_LOGD("%{public}s: enter. BatteryST001::temperature=%{public}d.", __func__, temperature);

    ASSERT_TRUE(temperature == 567);
    HDF_LOGD("%{public}s: enter. BatteryST001 end.", __func__);
}

/**
 * @tc.name: BatteryST002
 * @tc.desc: Test functions of HDI Interface GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST002 start.", __func__);
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");

    auto voltage = BatterydClient::GetVoltage();
    HDF_LOGD("%{public}s: enter. BatteryST002::voltage=%{public}d.", __func__, voltage);

    ASSERT_TRUE(voltage == 4123456);
    HDF_LOGD("%{public}s: enter. BatteryST002 end.", __func__);
}

/**
 * @tc.name: BatteryST003
 * @tc.desc: Test functions of HDI Interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST003 start.", __func__);
    CreateFile("/data/local/tmp/battery/capacity", "11");

    auto capacity = BatterydClient::GetCapacity();
    HDF_LOGD("%{public}s: enter. BatteryST003::capacity=%{public}d.", __func__, capacity);

    ASSERT_TRUE(capacity == 11);
    HDF_LOGD("%{public}s: enter. BatteryST003 end.", __func__);
}

/**
 * @tc.name: BatteryST004
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Good"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST004 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Good");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST004::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_GOOD);
    HDF_LOGD("%{public}s: enter. BatteryST004 end.", __func__);
}

/**
 * @tc.name: BatteryST005
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Cold"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST005, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST005 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Cold");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST005::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_COLD);
    HDF_LOGD("%{public}s: enter. BatteryST005 end.", __func__);
}

/**
 * @tc.name: BatteryST006
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Warm"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST006, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST006 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Warm");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST006::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_GOOD);
    HDF_LOGD("%{public}s: enter. BatteryST006 end.", __func__);
}

/**
 * @tc.name: BatteryST007
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Cool"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST007, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST007 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Cool");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST007::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_GOOD);
    HDF_LOGD("%{public}s: enter. BatteryST007 end.", __func__);
}

/**
 * @tc.name: BatteryST008
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Hot"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST008, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST008 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Hot");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST008::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_OVERHEAT);
    HDF_LOGD("%{public}s: enter. BatteryST008 end.", __func__);
}

/**
 * @tc.name: BatteryST009
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Overheat"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST009, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST009 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Overheat");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST009::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_OVERHEAT);
    HDF_LOGD("%{public}s: enter. BatteryST009 end.", __func__);
}

/**
 * @tc.name: BatteryST010
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Over voltage"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST010, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST010 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Over voltage");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST010::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_OVERVOLTAGE);
    HDF_LOGD("%{public}s: enter. BatteryST010 end.", __func__);
}

/**
 * @tc.name: BatteryST011
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Dead"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST011, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST011 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Dead");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST011::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_DEAD);
    HDF_LOGD("%{public}s: enter. BatteryST011 end.", __func__);
}

/**
 * @tc.name: BatteryST012
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Unknown"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST012, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST012 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Unknown");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST012::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_UNKNOWN);
    HDF_LOGD("%{public}s: enter. BatteryST012 end.", __func__);
}

/**
 * @tc.name: BatteryST013
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "Unspecified failure"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST013, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST013 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Unspecified failure");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST013::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_UNKNOWN);
    HDF_LOGD("%{public}s: enter. BatteryST013 end.", __func__);
}

/**
 * @tc.name: BatteryST014
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is "NULL"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST014, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST014 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST014::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_UNKNOWN);
    HDF_LOGD("%{public}s: enter. BatteryST014 end.", __func__);
}

/**
 * @tc.name: BatteryST015
 * @tc.desc: Test functions of HDI Interface GetHealthState when state is not in healthStateEnumMap
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST015, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST015 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "other");

    auto healthState = BatterydClient::GetHealthState();
    HDF_LOGD("%{public}s: enter. BatteryST015::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_UNKNOWN);
    HDF_LOGD("%{public}s: enter. BatteryST015 end.", __func__);
}

/**
 * @tc.name: BatteryST016
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST016, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST016 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST016::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_USB);
    HDF_LOGD("%{public}s: enter. BatteryST016 end.", __func__);
}

/**
 * @tc.name: BatteryST017
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_PD_DRP"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST017, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST017 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_PD_DRP");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST017::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_USB);
    HDF_LOGD("%{public}s: enter. BatteryST017 end.", __func__);
}

/**
 * @tc.name: BatteryST018
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "Wireless"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST018, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST018 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Wireless");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST018::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    HDF_LOGD("%{public}s: enter. BatteryST018 end.", __func__);
}

/**
 * @tc.name: BatteryST019
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "Mains"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST019, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST019 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Mains");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST019::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST019 end.", __func__);
}

/**
 * @tc.name: BatteryST020
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "UPS"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST020, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST020 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "UPS");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST020::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST020 end.", __func__);
}

/**
 * @tc.name: BatteryST021
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_ACA"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST021, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST021 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_ACA");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST021::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST021 end.", __func__);
}

/**
 * @tc.name: BatteryST022
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_C"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST022, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST022 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_C");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST022::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST022 end.", __func__);
}

/**
 * @tc.name: BatteryST023
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_CDP"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST023, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST023 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_CDP");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST023::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST023 end.", __func__);
}

/**
 * @tc.name: BatteryST024
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_DCP"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST024, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST024 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_DCP");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST024::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST024 end.", __func__);
}

/**
 * @tc.name: BatteryST025
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_HVDCP"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST025, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST025 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_HVDCP");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST025::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST025 end.", __func__);
}

/**
 * @tc.name: BatteryST026
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "USB_PD"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST026, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST026 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB_PD");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST026::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST026 end.", __func__);
}

/**
 * @tc.name: BatteryST027
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is "Unknown"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST027, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST027 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Unknown");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST027::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST027 end.", __func__);
}

/**
 * @tc.name: BatteryST028
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is NULL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST028, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST028 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST028::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST028 end.", __func__);
}

/**
 * @tc.name: BatteryST029
 * @tc.desc: Test HDI interface GetPluggedType when pluggedtype is not in pluggedTypeEnumMap
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST029, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST029 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "other");

    auto pluggedType = BatterydClient::GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST029::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST029 end.", __func__);
}

/**
 * @tc.name: BatteryST030
 * @tc.desc: Test HDI interface GetChargeState when state is "Discharging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST030, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST030 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Discharging");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST030::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST030 end.", __func__);
}

/**
 * @tc.name: BatteryST031
 * @tc.desc: Test HDI interface GetChargeState when state is "Charging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST031, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST031 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Charging");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST031::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_ENABLE);
    HDF_LOGD("%{public}s: enter. BatteryST031 end.", __func__);
}

/**
 * @tc.name: BatteryST032
 * @tc.desc: Test HDI interface GetChargeState when state is "Full"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST032, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST032 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Full");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST032::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_FULL);
    HDF_LOGD("%{public}s: enter. BatteryST032 end.", __func__);
}

/**
 * @tc.name: BatteryST033
 * @tc.desc: Test HDI interface GetChargeState when state is "Not charging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST033, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST033 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Not charging");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST033::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_DISABLE);
    HDF_LOGD("%{public}s: enter. BatteryST033 end.", __func__);
}

/**
 * @tc.name: BatteryST034
 * @tc.desc: Test HDI interface GetChargeState when state is "Unknown"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST034, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST034 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Unknown");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST034::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST034 end.", __func__);
}

/**
 * @tc.name: BatteryST035
 * @tc.desc: Test HDI interface GetChargeState when state is NULL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST035, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST035 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST035::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST035 end.", __func__);
}

/**
 * @tc.name: BatteryST036
 * @tc.desc: Test HDI interface GetChargeState when state is not in chargeStateEnumMap
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST036, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST036 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "others");

    auto chargeState = BatterydClient::GetChargeState();
    HDF_LOGD("%{public}s: enter. BatteryST036::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST036 end.", __func__);
}

/**
 * @tc.name: BatteryST037
 * @tc.desc: Test functions of HDI Interface GetPresent when battery is exist
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST037, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST037 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "1");

    auto present = BatterydClient::GetPresent();
    HDF_LOGD("%{public}s: enter. BatteryST037::present=%{public}d.", __func__, present);

    ASSERT_TRUE(present);
    HDF_LOGD("%{public}s: enter. BatteryST037 end.", __func__);
}

/**
 * @tc.name: BatteryST038
 * @tc.desc: Test functions of HDI Interface GetPresent when battery is not exist
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST038, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST038 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "0");

    auto present = BatterydClient::GetPresent();
    HDF_LOGD("%{public}s: enter. BatteryST038::present=%{public}d.", __func__, present);

    ASSERT_TRUE(!present);
    HDF_LOGD("%{public}s: enter. BatteryST038 end.", __func__);
}

/**
 * @tc.name: BatteryST039
 * @tc.desc: Test functions of HDI Interface GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST039, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST039 start.", __func__);
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li");

    auto technology = BatterydClient::GetTechnology();
    HDF_LOGD("%{public}s: enter. BatteryST039::technology=%{public}s.", __func__, technology.c_str());

    ASSERT_TRUE(technology == "Li");
    HDF_LOGD("%{public}s: enter. BatteryST039 end.", __func__);
}

/**
 * @tc.name: BatteryST040
 * @tc.desc: Test functions of HDI Interface BindBatterydSubscriber
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST040, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST040 start.", __func__);

    sptr<BatteryServiceSubscriber> batterydSubscriber = new BatteryServiceSubscriber();
    auto errCode = BatterydClient::BindBatterydSubscriber(batterydSubscriber);
    HDF_LOGD("%{public}s: enter. BatteryST040::errCode=%{public}d.", __func__, errCode);

    ASSERT_TRUE(errCode == OHOS::ERR_OK);
    HDF_LOGD("%{public}s: enter. BatteryST040 end.", __func__);
}

/**
 * @tc.name: BatteryST041
 * @tc.desc: Test functions of HDI Interface UnbindBatterydSubscriber twice
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST041, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST041 start.", __func__);

    auto errCode = BatterydClient::UnbindBatterydSubscriber();
    errCode = BatterydClient::UnbindBatterydSubscriber();
    HDF_LOGD("%{public}s: enter. BatteryST041::errCode=%{public}d.", __func__, errCode);

    ASSERT_TRUE(errCode != OHOS::ERR_OK);
    HDF_LOGD("%{public}s: enter. BatteryST041 end.", __func__);
}

/**
 * @tc.name: BatteryST042
 * @tc.desc: Test IBatterySrv interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST042, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST042 start.", __func__);
    CreateFile("/data/local/tmp/battery/capacity", "44");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto capacity = BatterySrvClient.GetCapacity();
    HDF_LOGD("%{public}s: enter. BatteryST042::capacity=%{public}d.", __func__, capacity);
    GTEST_LOG_(INFO) << "BatteryST042 executing, capacity=" << capacity;

    ASSERT_TRUE(capacity == 44);
    HDF_LOGD("%{public}s: enter. BatteryST042 end.", __func__);
}

/**
 * @tc.name: BatteryST043
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is none
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST043, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST043 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Discharging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST043 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST043 end.", __func__);
}

/**
 * @tc.name: BatteryST044
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is enable
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST044, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST044 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Charging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST044 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_ENABLE);
    HDF_LOGD("%{public}s: enter. BatteryST044 end.", __func__);
}

/**
 * @tc.name: BatteryST045
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is full
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST045, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST045 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Full");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST045 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_FULL);
    HDF_LOGD("%{public}s: enter. BatteryST045 end.", __func__);
}

/**
 * @tc.name: BatteryST046
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is disable
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST046, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST046 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Not charging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST046 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_DISABLE);
    HDF_LOGD("%{public}s: enter. BatteryST046 end.", __func__);
}

/**
 * @tc.name: BatteryST047
 * @tc.desc: Test IBatterySrv interface GetChargingStatus when chargestate value is butt
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST047, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST047 start.", __func__);
    CreateFile("/data/local/tmp/battery/status", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST047 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST047 end.", __func__);
}

/**
 * @tc.name: BatteryST048
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_GOOD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST048, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST048 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Good");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST048 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_GOOD);
    HDF_LOGD("%{public}s: enter. BatteryST048 end.", __func__);
}

/**
 * @tc.name: BatteryST049
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_COLD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST049, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST049 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Cold");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST049 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_COLD);
    HDF_LOGD("%{public}s: enter. BatteryST049 end.", __func__);
}

/**
 * @tc.name: BatteryST050
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_OVERHEAT
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST050, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST050 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Hot");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST050 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_OVERHEAT);
    HDF_LOGD("%{public}s: enter. BatteryST050 end.", __func__);
}

/**
 * @tc.name: BatteryST051
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_OVERVOLTAGE
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST051, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST051 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Over voltage");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST051 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_OVERVOLTAGE);
    HDF_LOGD("%{public}s: enter. BatteryST051 end.", __func__);
}

/**
 * @tc.name: BatteryST052
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_DEAD
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST052, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST052 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Dead");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST052 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_DEAD);
    HDF_LOGD("%{public}s: enter. BatteryST052 end.", __func__);
}

/**
 * @tc.name: BatteryST053
 * @tc.desc: Test IBatterySrv interface GetHealthStatus when healthstatus value is HEALTH_STATE_UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST053, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST053 start.", __func__);
    CreateFile("/data/local/tmp/battery/health", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST053 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == BatteryHealthState::HEALTH_STATE_UNKNOWN);
    HDF_LOGD("%{public}s: enter. BatteryST053 end.", __func__);
}

/**
 * @tc.name: BatteryST054
 * @tc.desc: Test IBatterySrv interface GetPresent when present is true
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST054, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST054 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "1");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto present = BatterySrvClient.GetPresent();
    GTEST_LOG_(INFO) << "BatteryST054 executing, present=" << present;

    ASSERT_TRUE(present);
    HDF_LOGD("%{public}s: enter. BatteryST054 end.", __func__);
}

/**
 * @tc.name: BatteryST055
 * @tc.desc: Test IBatterySrv interface GetPresent when present is false
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST055, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST055 start.", __func__);
    CreateFile("/data/local/tmp/battery/present", "0");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto present = BatterySrvClient.GetPresent();
    GTEST_LOG_(INFO) << "BatteryST055 executing, present=" << present;

    ASSERT_FALSE(present);
    HDF_LOGD("%{public}s: enter. BatteryST055 end.", __func__);
}

/**
 * @tc.name: BatteryST056
 * @tc.desc: Test IBatterySrv interface GetVoltage
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST056, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST056 start.", __func__);
    CreateFile("/data/local/tmp/battery/voltage_avg", "4654321");
    CreateFile("/data/local/tmp/battery/voltage_now", "4654321");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto voltage = BatterySrvClient.GetVoltage();
    GTEST_LOG_(INFO) << "BatteryST056 executing, voltage=" << voltage;

    ASSERT_TRUE(voltage == 4654321);
    HDF_LOGD("%{public}s: enter. BatteryST056 end.", __func__);
}

/**
 * @tc.name: BatteryST057
 * @tc.desc: Test IBatterySrv interface GetTemperature
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST057, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST057 start.", __func__);
    CreateFile("/data/local/tmp/battery/temp", "234");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto temperature = BatterySrvClient.GetBatteryTemperature();
    GTEST_LOG_(INFO) << "BatteryST057 executing, temperature=" << temperature;

    ASSERT_TRUE(temperature == 234);
    HDF_LOGD("%{public}s: enter. BatteryST057 end.", __func__);
}

/**
 * @tc.name: BatteryST058
 * @tc.desc: Test IBatterySrv interface GetTechnology
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST058, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST058 start.", __func__);
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "H2");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto technology = BatterySrvClient.GetTechnology();

    ASSERT_TRUE(technology == "H2");
    HDF_LOGD("%{public}s: enter. BatteryST058 end.", __func__);
}

/**
 * @tc.name: BatteryST059
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_NONE
 *           ParsePluggedType return HDF_ERR_NOT_SUPPORT, BatterydClient::GetPluggedType FAIL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST059, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST059 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "None");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST059 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST059 end.", __func__);
}

/**
 * @tc.name: BatteryST060
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_AC
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST060, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST060 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Mains");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST060 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_AC);
    HDF_LOGD("%{public}s: enter. BatteryST060 end.", __func__);
}

/**
 * @tc.name: BatteryST061
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_USB
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST061, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST061 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST061 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_USB);
    HDF_LOGD("%{public}s: enter. BatteryST061 end.", __func__);
}

/**
 * @tc.name: BatteryST062
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_WIRELESS
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST062, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST062 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Wireless");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST062 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    HDF_LOGD("%{public}s: enter. BatteryST062 end.", __func__);
}

/**
 * @tc.name: BatteryST063
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_BUTT
 *           ParsePluggedType return HDF_ERR_NOT_SUPPORT, BatterydClient::GetPluggedType FAIL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST063, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST063 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST063 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST063 end.", __func__);
}

/**
 * @tc.name: BatteryST064
 * @tc.desc: Test Led color is red when capacity is 0
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST064, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST064 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 0;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST064 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST064 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST064 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST064 end.", __func__);
}

/**
 * @tc.name: BatteryST065
 * @tc.desc: Test Led color is red when capacity range in (0,10), medium value
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST065, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST065 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 5;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST065 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST065 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST065 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST065 end.", __func__);
}

/**
 * @tc.name: BatteryST066
 * @tc.desc: Test Led color is red when capacity range in (0,10), critical value
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST066, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST066 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 9;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST066 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST066 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST066 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST066 end.", __func__);
}

/**
 * @tc.name: BatteryST067
 * @tc.desc: Test Led color is yellow when capacity is 10
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST067, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST067 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 10;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST067 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST067 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST067 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST067 end.", __func__);
}

/**
 * @tc.name: BatteryST068
 * @tc.desc: Test Led color is yellow when capacity is bigger than 10
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST068, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST068 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 50;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST068 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST068 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST068 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST068 end.", __func__);
}

/**
 * @tc.name: BatteryST069
 * @tc.desc: Test Led color is red when capacity range in [10,90), critical value
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST069, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST069 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 89;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST069 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST069 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST069 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST069 end.", __func__);
}

/**
 * @tc.name: BatteryST070
 * @tc.desc: Test Led color is yellow when capacity is 90
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST070, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST070 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 90;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST070 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST070 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST070 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST070 end.", __func__);
}

/**
 * @tc.name: BatteryST071
 * @tc.desc: Test Led color is yellow when capacity is bigger than 90
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST071, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST071 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 95;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST071 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST071 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST071 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST071 end.", __func__);
}

/**
 * @tc.name: BatteryST072
 * @tc.desc: Test Led color is red when capacity range in [90,100), critical value
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST072, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST072 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 99;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST072 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST072 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST072 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST072 end.", __func__);
}

/**
 * @tc.name: BatteryST073
 * @tc.desc: Test Led color is yellow when capacity is 100
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST073, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST073 start.", __func__);

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 99;
    BatteryLed batteryled;
    UpdateLedColorTest(chargestate, capacity, batteryled);

    int32_t value = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST073 executing, red brightness value=" << value;
    ASSERT_TRUE(value == 0);

    value = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST073 executing, green brightness value=" << value;
    ASSERT_TRUE(value == 255);

    value = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST073 executing, blue brightness value=" << value;

    ASSERT_TRUE(value == 0);
    HDF_LOGD("%{public}s: enter. BatteryST073 end.", __func__);
}

/**
 * @tc.name: BatteryST074
 * @tc.desc: Test animatin and led turn on when charging
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST074, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST074 start.", __func__);
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB");
    CreateFile("/data/local/tmp/battery/capacity", "11");
    CreateFile("/data/local/tmp/battery/status", "Charging");
    ChargerThread cthread;
    BatteryThreadTest bthread;
    void *arg = nullptr;
    std::unique_ptr<BatteryHostServiceStub> stub = std::make_unique<BatteryHostServiceStub>();

    stub->Init();
    bthread.Init(cthread);
    bthread.UpdateBatteryInfo(arg, cthread);

    int32_t redcolor = ReadRedLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST074 executing, red brightness value=" << redcolor;

    int32_t greencolor = ReadGreenLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST074 executing, green brightness value=" << greencolor;

    int32_t bluecolor = ReadBlueLedSysfs();
    GTEST_LOG_(INFO) << "BatteryST074 executing, blue brightness value=" << bluecolor;

    ASSERT_TRUE(redcolor == 255 || greencolor == 255 || bluecolor == 255);
    ASSERT_TRUE(AnimationLabel::needStop_ == true);
    HDF_LOGD("%{public}s: enter. BatteryST074 end.", __func__);
}

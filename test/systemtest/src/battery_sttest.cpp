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
#include <cstring>
#include <thread>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#include "utils/hdf_log.h"
#include "power_supply_provider.h"
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

static sptr<BatteryService> g_service;
static std::vector<std::string> g_ledsNodeName;
static const std::string LEDS_BASE_PATH = "/sys/class/leds";
static std::string g_redLedsNode = "red";
static std::string g_greenLedsNode = "green";
static std::string g_blueLedsNode = "blue";
const std::string MOCK_BATTERY_PATH = "/data/service/el0/battery/";

void BatterySttest::SetUpTestCase(void)
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
}

void BatterySttest::TearDownTestCase(void)
{
    g_service->OnStop();
    DelayedSpSingleton<BatteryService>::DestroyInstance();
}

void BatterySttest::SetUp(void)
{
}

void BatterySttest::TearDown(void)
{
}

std::string CreateFile(std::string path, std::string content)
{
    HDF_LOGD("%{public}s: enter. CreateFile enter.", __func__);
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: enter, Cannot create file %{private}s", __func__, path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

static void MockFileInit()
{
    mkdir((MOCK_BATTERY_PATH + "/battery").c_str(), S_IRWXU);
    mkdir((MOCK_BATTERY_PATH + "/ohos_charger").c_str(), S_IRWXU);
    mkdir((MOCK_BATTERY_PATH + "/ohos-fgu").c_str(), S_IRWXU);
    HDF_LOGD("%{public}s: enter.", __func__);

    sleep(1);
    CreateFile(MOCK_BATTERY_PATH + "/battery/online", "1");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Battery");
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/health", "Unknown");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/temp", "345");

    g_service->ChangePath(MOCK_BATTERY_PATH);
}

static void TraversalNode()
{
    HDF_LOGD("%{public}s: enter", __func__);
    string::size_type idx;

    for (auto iter = g_ledsNodeName.begin(); iter != g_ledsNodeName.end(); ++iter) {
        idx = iter->find(g_redLedsNode);
        if (idx == string::npos) {
            HDF_LOGD("%{public}s: not found red leds node", __func__);
        } else {
            g_redLedsNode = *iter;
            HDF_LOGD("%{public}s: red leds node is %{public}s", __func__, iter->c_str());
        }

        idx = iter->find(g_greenLedsNode);
        if (idx == string::npos) {
            HDF_LOGD("%{public}s: not found green leds node", __func__);
        } else {
            g_greenLedsNode = *iter;
            HDF_LOGD("%{public}s: green leds node is %{public}s", __func__, iter->c_str());
        }

        idx = iter->find(g_blueLedsNode);
        if (idx == string::npos) {
            HDF_LOGD("%{public}s: not found blue leds node", __func__);
        } else {
            g_blueLedsNode = *iter;
            HDF_LOGD("%{public}s: blue leds node is %{public}s", __func__, iter->c_str());
        }
    }

    HDF_LOGD("%{public}s: exit", __func__);
}

static int32_t InitLedsSysfs(void)
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;
    int maxSize = 64;

    dir = opendir(LEDS_BASE_PATH.c_str());
    if (dir == nullptr) {
        HDF_LOGE("%{public}s: leds base path is not exist", __func__);
        return HDF_ERR_IO;
    }

    while (true) {
        entry = readdir(dir);
        if (entry == nullptr) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            HDF_LOGD("%{public}s: init leds info of %{public}s", __func__, entry->d_name);
            if (index >= maxSize) {
                HDF_LOGE("%{public}s: too many leds types", __func__);
                break;
            }
            g_ledsNodeName.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalNode();
    HDF_LOGD("%{public}s: index is %{public}d", __func__, index);
    closedir(dir);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

static int32_t ReadRedLedSysfs()
{
    HDF_LOGD("%{public}s: enter", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string sysRedLedPath = LEDS_BASE_PATH + "/" + g_redLedsNode + "/" + "brightness";

    int fd = open(sysRedLedPath.c_str(), O_RDWR);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open sys red led path %{public}s", __func__, g_redLedsNode.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, sysRedLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t redcolor = strtol(buf, nullptr, strlen);
    close(fd);

    HDF_LOGE("%{public}s: read redcolor value is %{public}d", __func__, redcolor);
    return redcolor;
}

static int32_t ReadGreenLedSysfs()
{
    HDF_LOGD("%{public}s: enter", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string sysGreenLedPath = LEDS_BASE_PATH + "/" + g_greenLedsNode + "/" + "brightness";

    int fd = open(sysGreenLedPath.c_str(), O_RDWR);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open sys green led path %{public}s", __func__, g_greenLedsNode.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, sysGreenLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t greencolor = strtol(buf, nullptr, strlen);
    close(fd);

    HDF_LOGE("%{public}s: read greencolor value is %{public}d", __func__, greencolor);
    return greencolor;
}

static int32_t ReadBlueLedSysfs()
{
    HDF_LOGD("%{public}s: enter", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string sysBlueLedPath = LEDS_BASE_PATH + "/" + g_blueLedsNode + "/" + "brightness";

    int fd = open(sysBlueLedPath.c_str(), O_RDWR);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open sys blue led path %{public}s", __func__, g_blueLedsNode.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, sysBlueLedPath.c_str());
        close(fd);
    }

    buf[readSize] = '\0';
    int32_t bluecolor = strtol(buf, nullptr, strlen);
    close(fd);

    HDF_LOGE("%{public}s: read bluecolor value is %{public}d", __func__, bluecolor);
    return bluecolor;
}

/**
 * @tc.name: BatteryST001
 * @tc.desc: Test functions of HDI Interface GetTemperature
 * @tc.type: FUNC
 */
static HWTEST_F (BatterySttest, BatteryST001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST001 start.", __func__);
    MockFileInit();
    CreateFile(MOCK_BATTERY_PATH + "/battery/temp", "567");

    auto temperature = g_service->GetBatteryTemperature();
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/voltage_avg", "4123456");
    CreateFile(MOCK_BATTERY_PATH + "/battery/voltage_now", "4123456");

    auto voltage = g_service->GetVoltage();
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/capacity", "11");

    auto capacity = g_service->GetCapacity();
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Good");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST004::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Cold");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST005::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Warm");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST006::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Cool");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST007::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Hot");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST008::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERHEAT);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Overheat");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST009::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERHEAT);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Over voltage");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST010::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERVOLTAGE);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Dead");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST011::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_DEAD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Unknown");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST012::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Unspecified failure");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST013::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST014::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "other");

    auto healthState = g_service->GetHealthStatus();
    HDF_LOGD("%{public}s: enter. BatteryST015::healthState=%{public}d.", __func__, healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB");
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/online", "1");
    CreateFile(MOCK_BATTERY_PATH + "/battery/online", "1");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/online", "1");

    std::string path = MOCK_BATTERY_PATH + "";
    g_service->ChangePath(path);
    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST016::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_PD_DRP");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_PD_DRP");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_PD_DRP");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST017::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Wireless");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Wireless");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Wireless");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST018::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Mains");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Mains");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Mains");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST019::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "UPS");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "UPS");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "UPS");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST020::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_ACA");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_ACA");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_ACA");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST021::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_C");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_C");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_C");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST022::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_CDP");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_CDP");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_CDP");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST023::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_DCP");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_DCP");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_DCP");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST024::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_HVDCP");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_HVDCP");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_HVDCP");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST025::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB_PD");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB_PD");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB_PD");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST026::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Unknown");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Unknown");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Unknown");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST027::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST028::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "other");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "other");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "other");

    auto pluggedType = g_service->GetPluggedType();
    HDF_LOGD("%{public}s: enter. BatteryST029::pluggedType=%{public}d.", __func__, pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST029 end.", __func__);
}

/**
 * @tc.name: BatteryST030
 * @tc.desc: Test HDI interface GetChargingStatus when state is "Discharging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST030, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST030 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Discharging");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST030::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_NONE);
    HDF_LOGD("%{public}s: enter. BatteryST030 end.", __func__);
}

/**
 * @tc.name: BatteryST031
 * @tc.desc: Test HDI interface GetChargingStatus when state is "Charging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST031, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST031 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Charging");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST031::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE);
    HDF_LOGD("%{public}s: enter. BatteryST031 end.", __func__);
}

/**
 * @tc.name: BatteryST032
 * @tc.desc: Test HDI interface GetChargingStatus when state is "Full"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST032, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST032 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Full");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST032::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
    HDF_LOGD("%{public}s: enter. BatteryST032 end.", __func__);
}

/**
 * @tc.name: BatteryST033
 * @tc.desc: Test HDI interface GetChargingStatus when state is "Not charging"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST033, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST033 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Not charging");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST033::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE);
    HDF_LOGD("%{public}s: enter. BatteryST033 end.", __func__);
}

/**
 * @tc.name: BatteryST034
 * @tc.desc: Test HDI interface GetChargingStatus when state is "Unknown"
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST034, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST034 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Unknown");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST034::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST034 end.", __func__);
}

/**
 * @tc.name: BatteryST035
 * @tc.desc: Test HDI interface GetChargingStatus when state is NULL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST035, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST035 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST035::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT);
    HDF_LOGD("%{public}s: enter. BatteryST035 end.", __func__);
}

/**
 * @tc.name: BatteryST036
 * @tc.desc: Test HDI interface GetChargingStatus when state is not in chargeStateEnumMap
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST036, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST036 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "others");

    auto chargeState = g_service->GetChargingStatus();
    HDF_LOGD("%{public}s: enter. BatteryST036::chargeState=%{public}d.", __func__, chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/present", "1");

    auto present = g_service->GetPresent();
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/present", "0");

    auto present = g_service->GetPresent();
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/technology", "Li");

    auto technology = g_service->GetTechnology();
    HDF_LOGD("%{public}s: enter. BatteryST039::technology=%{public}s.", __func__, technology.c_str());

    ASSERT_TRUE(technology == "Li");
    HDF_LOGD("%{public}s: enter. BatteryST039 end.", __func__);
}

/**
 * @tc.name: BatteryST042
 * @tc.desc: Test IBatterySrv interface GetCapacity
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST042, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST042 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/battery/capacity", "44");

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
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Discharging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST043 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_NONE);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Charging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST044 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_ENABLE);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Full");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST045 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_FULL);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Not charging");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST046 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_DISABLE);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/status", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto chargeState = BatterySrvClient.GetChargingStatus();
    GTEST_LOG_(INFO) << "BatteryST047 executing, chargeState=" << int(chargeState);

    ASSERT_TRUE(chargeState == OHOS::PowerMgr::BatteryChargeState::CHARGE_STATE_BUTT);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Good");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST048 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_GOOD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Cold");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST049 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_COLD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Hot");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST050 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERHEAT);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Over voltage");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST051 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_OVERVOLTAGE);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Dead");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST052 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_DEAD);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/health", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto healthState = BatterySrvClient.GetHealthStatus();
    GTEST_LOG_(INFO) << "BatteryST053 executing, healthState=" << int(healthState);

    ASSERT_TRUE(healthState == OHOS::PowerMgr::BatteryHealthState::HEALTH_STATE_UNKNOWN);
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
    CreateFile(MOCK_BATTERY_PATH + "/battery/present", "1");

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
    CreateFile(MOCK_BATTERY_PATH + "/battery/present", "0");

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
    CreateFile(MOCK_BATTERY_PATH + "/battery/voltage_avg", "4654321");
    CreateFile(MOCK_BATTERY_PATH + "/battery/voltage_now", "4654321");

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
    CreateFile(MOCK_BATTERY_PATH + "/battery/temp", "234");

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
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/technology", "H2");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto technology = BatterySrvClient.GetTechnology();

    ASSERT_TRUE(technology == "H2");
    HDF_LOGD("%{public}s: enter. BatteryST058 end.", __func__);
}

/**
 * @tc.name: BatteryST059
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_NONE
 *           ParsePluggedType return HDF_ERR_NOT_SUPPORT, g_service->GetPluggedType FAIL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST059, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST059 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "None");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "None");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST059 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Mains");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Mains");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Mains");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST060 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_AC);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "USB");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "USB");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "USB");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST061 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_USB);
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
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Wireless");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Wireless");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Wireless");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST062 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_WIRELESS);
    HDF_LOGD("%{public}s: enter. BatteryST062 end.", __func__);
}

/**
 * @tc.name: BatteryST063
 * @tc.desc: Test IBatterySrv interface GetPluggedType when pluggedType value is PLUGGED_TYPE_BUTT
 *           ParsePluggedType return HDF_ERR_NOT_SUPPORT, g_service->GetPluggedType FAIL
 * @tc.type: FUNC
 */
HWTEST_F (BatterySttest, BatteryST063, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST063 start.", __func__);
    CreateFile(MOCK_BATTERY_PATH + "/ohos_charger/type", "Unknown");
    CreateFile(MOCK_BATTERY_PATH + "/battery/type", "Unknown");
    CreateFile(MOCK_BATTERY_PATH + "/ohos-fgu/type", "Unknown");

    auto& BatterySrvClient = BatterySrvClient::GetInstance();
    auto pluggedType = BatterySrvClient.GetPluggedType();
    GTEST_LOG_(INFO) << "BatteryST063 executing, pluggedType=" << int(pluggedType);

    ASSERT_TRUE(pluggedType == OHOS::PowerMgr::BatteryPluggedType::PLUGGED_TYPE_NONE);
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
    InitLedsSysfs();

    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    int32_t capacity = 0;
    BatteryLed batteryled;
    batteryled.InitLedsSysfs();
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

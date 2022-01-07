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
#include <cstring>
#include <thread>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

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
const std::string SYSTEM_BATTERY_PATH = "/sys/class/power_supply";
const std::string POWER_SUPPLY_SUB_PATH = "/sys/class/power_supply/battery";
static std::vector<std::string> g_filenodeName;
static std::map<std::string, std::string> g_nodeInfo;
const int STR_TO_LONG_LEN = 10;

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

struct StringEnumMap {
    const char* str;
    int32_t enumVal;
};

std::string CreateFile(std::string path, std::string content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: Cannot create file %{public}s", __func__, path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

void MockFileInit()
{
    HDF_LOGD("%{public}s: enter.", __func__);
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

    HDF_LOGD("%{public}s: enter.", __func__);
}

static void CheckSubfolderNode(const std::string& path)
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR *dir = nullptr;
    struct dirent* entry = nullptr;
    std::string batteryPath = SYSTEM_BATTERY_PATH + "/" + path;
    HDF_LOGI("%{public}s: subfolder path is:%{public}s", __func__, batteryPath.c_str());

    dir = opendir(batteryPath.c_str());
    if (dir == nullptr) {
        HDF_LOGI("%{public}s: subfolder file is not exist.", __func__);
        return;
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
            continue;
        }
        if ((strcmp(entry->d_name, "type") == 0) && (g_nodeInfo["type"] == "") &&
            (strcasecmp(path.c_str(), "battery") != 0)) {
            g_nodeInfo["type"] = path;
        } else if ((strcmp(entry->d_name, "online") == 0) && (g_nodeInfo["online"] == "")) {
            g_nodeInfo["online"] = path;
        } else if ((strcmp(entry->d_name, "current_max") == 0) && (g_nodeInfo["current_max"] == "")) {
            g_nodeInfo["current_max"] = path;
        } else if ((strcmp(entry->d_name, "voltage_max") == 0) && (g_nodeInfo["voltage_max"] == "")) {
            g_nodeInfo["voltage_max"] = path;
        } else if ((strcmp(entry->d_name, "capacity") == 0) && (g_nodeInfo["capacity"] == "")) {
            g_nodeInfo["capacity"] = path;
        } else if ((strcmp(entry->d_name, "voltage_now") == 0) && (g_nodeInfo["voltage_now"] == "")) {
            g_nodeInfo["voltage_now"] = path;
        } else if ((strcmp(entry->d_name, "temp") == 0) && (g_nodeInfo["temp"] == "")) {
            g_nodeInfo["temp"] = "battery";
        } else if ((strcmp(entry->d_name, "health") == 0) && (g_nodeInfo["health"] == "")) {
            g_nodeInfo["health"] = path;
        } else if ((strcmp(entry->d_name, "status") == 0) && (g_nodeInfo["status"] == "")) {
            g_nodeInfo["status"] = path;
        } else if ((strcmp(entry->d_name, "present") == 0) && (g_nodeInfo["present"] == "")) {
            g_nodeInfo["present"] = path;
        } else if ((strcmp(entry->d_name, "charge_counter") == 0) && (g_nodeInfo["charge_counter"] == "")) {
            g_nodeInfo["charge_counter"] = path;
        } else if ((strcmp(entry->d_name, "technology") == 0) && (g_nodeInfo["technology"] == "")) {
            g_nodeInfo["technology"] = path;
        } else {
            HDF_LOGI("%{public}s: battery node other branch is excute.", __func__);
        }
    }
    closedir(dir);
    HDF_LOGI("%{public}s exit", __func__);
}

static void TraversalBaseNode()
{
    HDF_LOGI("%{public}s enter", __func__);
    g_nodeInfo.insert(std::make_pair("type", ""));
    g_nodeInfo.insert(std::make_pair("online", ""));
    g_nodeInfo.insert(std::make_pair("current_max", ""));
    g_nodeInfo.insert(std::make_pair("voltage_max", ""));
    g_nodeInfo.insert(std::make_pair("capacity", ""));
    g_nodeInfo.insert(std::make_pair("voltage_now", ""));
    g_nodeInfo.insert(std::make_pair("temp", ""));
    g_nodeInfo.insert(std::make_pair("health", ""));
    g_nodeInfo.insert(std::make_pair("status", ""));
    g_nodeInfo.insert(std::make_pair("present", ""));
    g_nodeInfo.insert(std::make_pair("charge_counter", ""));
    g_nodeInfo.insert(std::make_pair("technology", ""));

    auto iter = g_filenodeName.begin();
    while (iter != g_filenodeName.end()) {
        if (*iter == "battery") {
            CheckSubfolderNode(*iter);
            iter = g_filenodeName.erase(iter);
        } else {
            iter++;
        }
    }

    iter = g_filenodeName.begin();
    while (iter != g_filenodeName.end()) {
        if (*iter == "Battery") {
            CheckSubfolderNode(*iter);
            iter = g_filenodeName.erase(iter);
        } else {
            iter++;
        }
    }

    for (auto it = g_filenodeName.begin(); it != g_filenodeName.end(); ++it) {
        CheckSubfolderNode(*it);
    }
    HDF_LOGI("%{public}s exit", __func__);
}

static int32_t InitBaseSysfs(void)
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;

    dir = opendir(SYSTEM_BATTERY_PATH.c_str());
    if (dir == nullptr) {
        HDF_LOGE("%{public}s: cannot open POWER_SUPPLY_BASE_PATH", __func__);
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
            HDF_LOGD("%{public}s: init sysfs info of %{public}s", __func__, entry->d_name);
            if (index >= MAX_SYSFS_SIZE) {
                HDF_LOGE("%{public}s: too many plugged types", __func__);
                break;
            }
            g_filenodeName.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalBaseNode();
    HDF_LOGD("%{public}s: index is %{public}d", __func__, index);
    closedir(dir);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

static int32_t ReadTemperatureSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    InitBaseSysfs();
    std::string tempNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "temp") {
            tempNode = iter->second;
            break;
        }
    }
    std::string sysBattTemPath = SYSTEM_BATTERY_PATH + "/" + tempNode + "/" + "temp";
    HDF_LOGE("%{public}s: sysBattTemPath is %{public}s", __func__, sysBattTemPath.c_str());

    int fd = open(sysBattTemPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysBattTemPath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysBattTemPath.c_str());
        close(fd);
        return -1;
    }

    buf[readSize] = '\0';
    int32_t battTemperature = strtol(buf, nullptr, strlen);
    HDF_LOGE("%{public}s: read system file temperature is %{public}d", __func__, battTemperature);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battTemperature;
}

static int32_t ReadVoltageSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string voltageNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "voltage_now") {
            voltageNode = iter->second;
            break;
        }
    }
    std::string sysBattVolPath = SYSTEM_BATTERY_PATH + "/" + voltageNode + "/" + "voltage_now";
    HDF_LOGE("%{public}s: sysBattVolPath is %{public}s", __func__, sysBattVolPath.c_str());

    int fd = open(sysBattVolPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysBattVolPath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysBattVolPath.c_str());
        close(fd);
        return -1;
    }

    buf[readSize] = '\0';
    int32_t battVoltage = strtol(buf, nullptr, strlen);
    HDF_LOGE("%{public}s: read system file voltage is %{public}d", __func__, battVoltage);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battVoltage;
}

static int32_t ReadCapacitySysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string capacityNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "capacity") {
            capacityNode = iter->second;
            break;
        }
    }
    std::string sysBattCapPath = SYSTEM_BATTERY_PATH + "/" + capacityNode + "/" + "capacity";
    HDF_LOGE("%{public}s: sysBattCapPath is %{public}s", __func__, sysBattCapPath.c_str());

    int fd = open(sysBattCapPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysBattCapPath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysBattCapPath.c_str());
        close(fd);
        return -1;
    }

    buf[readSize] = '\0';
    int32_t battCapacity = strtol(buf, nullptr, strlen);
    HDF_LOGE("%{public}s: read system file capacity is %{public}d", __func__, battCapacity);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battCapacity;
}

static void Trim(char* str)
{
    HDF_LOGD("%{public}s: enter.", __func__);
    if (str == nullptr) {
        return;
    }

    str[strcspn(str, "\n")] = 0;
    HDF_LOGD("%{public}s: exit", __func__);
}

static int32_t HealthStateEnumConverter(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap healthStateEnumMap[] = {
        {"Good", PowerSupplyProvider::BATTERY_HEALTH_GOOD},
        {"Cold", PowerSupplyProvider::BATTERY_HEALTH_COLD},
        {"Warm", PowerSupplyProvider::BATTERY_HEALTH_GOOD}, // JEITA specification
        {"Cool", PowerSupplyProvider::BATTERY_HEALTH_GOOD}, // JEITA specification
        {"Hot", PowerSupplyProvider::BATTERY_HEALTH_OVERHEAT}, // JEITA specification
        {"Overheat", PowerSupplyProvider::BATTERY_HEALTH_OVERHEAT},
        {"Over voltage", PowerSupplyProvider::BATTERY_HEALTH_OVERVOLTAGE},
        {"Dead", PowerSupplyProvider::BATTERY_HEALTH_DEAD},
        {"Unknown", PowerSupplyProvider::BATTERY_HEALTH_UNKNOWN},
        {"Unspecified failure", PowerSupplyProvider::BATTERY_HEALTH_UNKNOWN},
        {NULL, PowerSupplyProvider::BATTERY_HEALTH_UNKNOWN},
    };

    for (int i = 0; healthStateEnumMap[i].str; ++i) {
        if (strcmp(str, healthStateEnumMap[i].str) == 0) {
            return healthStateEnumMap[i].enumVal;
        }
    }

    return PowerSupplyProvider::BATTERY_HEALTH_UNKNOWN;
}

static int32_t ReadHealthStateSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    char buf[128] = {0};
    int32_t readSize;
    std::string healthNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "health") {
            healthNode = iter->second;
            break;
        }
    }
    std::string sysHealthStatePath = SYSTEM_BATTERY_PATH + "/" + healthNode + "/" + "health";
    HDF_LOGE("%{public}s: sysHealthStatePath is %{public}s", __func__, sysHealthStatePath.c_str());

    int fd = open(sysHealthStatePath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysHealthStatePath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysHealthStatePath.c_str());
        close(fd);
        return -1;
    }

    Trim(buf);

    int32_t battHealthState = HealthStateEnumConverter(buf);
    HDF_LOGE("%{public}s: read system file healthstate is %{public}d", __func__, battHealthState);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battHealthState;
}

static int32_t PluggedTypeEnumConverter(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap pluggedTypeEnumMap[] = {
        {"USB", PowerSupplyProvider::PLUGGED_TYPE_USB},
        {"USB_PD_DRP", PowerSupplyProvider::PLUGGED_TYPE_USB},
        {"Wireless", PowerSupplyProvider::PLUGGED_TYPE_WIRELESS},
        {"Mains", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"UPS", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_ACA", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_C", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_CDP", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_DCP", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_HVDCP", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"USB_PD", PowerSupplyProvider::PLUGGED_TYPE_AC},
        {"Unknown", PowerSupplyProvider::PLUGGED_TYPE_BUTT},
        {NULL, PowerSupplyProvider::PLUGGED_TYPE_BUTT},
    };

    for (int i = 0; pluggedTypeEnumMap[i].str; ++i) {
        if (strcmp(str, pluggedTypeEnumMap[i].str) == 0) {
            return pluggedTypeEnumMap[i].enumVal;
        }
    }

    HDF_LOGI("%{public}s exit", __func__);
    return PowerSupplyProvider::PLUGGED_TYPE_BUTT;
}


static int32_t GetPluggedTypeName()
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[128] = {0};

    std::string onlineNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "online") {
            onlineNode = iter->second;
            break;
        }
    }
    std::string sysOnlinePath = SYSTEM_BATTERY_PATH + "/" + onlineNode + "/" + "online";
    HDF_LOGE("%{public}s: sysOnlinePath is %{public}s", __func__, sysOnlinePath.c_str());

    int fd = open(sysOnlinePath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysOnlinePath.c_str());
        return -1;
    }

    int32_t readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysOnlinePath.c_str());
        close(fd);
        return -1;
    }
    buf[readSize] = '\0';
    Trim(buf);

    int32_t online = strtol(buf, nullptr, STR_TO_LONG_LEN);
    return online;
}

static int32_t ReadPluggedTypeSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    char buf[128] = {0};
    int32_t readSize;
    int32_t online = GetPluggedTypeName();
    if (online != 1) {
        return PowerSupplyProvider::PLUGGED_TYPE_NONE;
    }

    std::string typeNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "type") {
            typeNode = iter->second;
            break;
        }
    }
    std::string sysPluggedTypePath = SYSTEM_BATTERY_PATH + "/" + typeNode + "/" + "type";
    HDF_LOGE("%{public}s: sysPluggedTypePath is %{public}s", __func__, sysPluggedTypePath.c_str());

    int fd = open(sysPluggedTypePath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysPluggedTypePath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysPluggedTypePath.c_str());
        close(fd);
        return -1;
    }
    buf[readSize] = '\0';
    Trim(buf);
    int32_t battPlugType = PluggedTypeEnumConverter(buf);
    if (battPlugType == PowerSupplyProvider::PLUGGED_TYPE_BUTT) {
        HDF_LOGW("%{public}s: not support the online type %{public}s", __func__, buf);
        battPlugType = PowerSupplyProvider::PLUGGED_TYPE_NONE;
    }

    HDF_LOGE("%{public}s: read system file pluggedtype is %{public}d", __func__, battPlugType);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battPlugType;
}

int32_t ChargeStateEnumConverter(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap chargeStateEnumMap[] = {
        {"Discharging", PowerSupplyProvider::CHARGE_STATE_NONE},
        {"Charging", PowerSupplyProvider::CHARGE_STATE_ENABLE},
        {"Full", PowerSupplyProvider::CHARGE_STATE_FULL},
        {"Not charging", PowerSupplyProvider::CHARGE_STATE_DISABLE},
        {"Unknown", PowerSupplyProvider::CHARGE_STATE_RESERVED},
        {NULL, PowerSupplyProvider::CHARGE_STATE_RESERVED},
    };

    for (int i = 0; chargeStateEnumMap[i].str; ++i) {
        if (strcmp(str, chargeStateEnumMap[i].str) == 0) {
            return chargeStateEnumMap[i].enumVal;
        }
    }

    return PowerSupplyProvider::CHARGE_STATE_RESERVED;
}

static int32_t ReadChargeStateSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    char buf[128] = {0};
    int32_t readSize;
    std::string statusNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "status") {
            statusNode = iter->second;
            break;
        }
    }
    std::string sysChargeStatePath = SYSTEM_BATTERY_PATH + "/" + statusNode + "/" + "status";
    HDF_LOGE("%{public}s: sysChargeStatePath is %{public}s", __func__, sysChargeStatePath.c_str());

    int fd = open(sysChargeStatePath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysChargeStatePath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysChargeStatePath.c_str());
        close(fd);
        return -1;
    }

    Trim(buf);
    int32_t battChargeState = ChargeStateEnumConverter(buf);
    HDF_LOGE("%{public}s: read system file chargestate is %{public}d", __func__, battChargeState);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battChargeState;
}

static int32_t ReadChargeCounterSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string counterNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "charge_counter") {
            counterNode = iter->second;
            break;
        }
    }
    std::string sysChargeCounterPath = SYSTEM_BATTERY_PATH + "/" + counterNode + "/" + "charge_counter";
    HDF_LOGE("%{public}s: sysChargeCounterPath is %{public}s", __func__, sysChargeCounterPath.c_str());

    int fd = open(sysChargeCounterPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysChargeCounterPath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysChargeCounterPath.c_str());
        close(fd);
        return -1;
    }

    buf[readSize] = '\0';
    int32_t battChargeCounter = strtol(buf, nullptr, strlen);
    HDF_LOGE("%{public}s: read system file chargestate is %{public}d", __func__, battChargeCounter);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battChargeCounter;
}

static int32_t ReadPresentSysfs()
{
    HDF_LOGD("%{public}s: enter.", __func__);
    int strlen = 10;
    char buf[128] = {0};
    int32_t readSize;
    std::string presentNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "present") {
            presentNode = iter->second;
            break;
        }
    }
    std::string sysPresentPath = SYSTEM_BATTERY_PATH + "/" + presentNode + "/" + "present";
    HDF_LOGE("%{public}s: sysPresentPath is %{public}s", __func__, sysPresentPath.c_str());

    int fd = open(sysPresentPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysPresentPath.c_str());
        return -1;
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysPresentPath.c_str());
        close(fd);
        return -1;
    }

    buf[readSize] = '\0';
    int32_t battPresent = strtol(buf, nullptr, strlen);
    HDF_LOGE("%{public}s: read system file chargestate is %{public}d", __func__, battPresent);
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battPresent;
}

static std::string ReadTechnologySysfs(std::string& battTechnology)
{
    HDF_LOGD("%{public}s: enter.", __func__);
    char buf[128] = {0};
    int32_t readSize;
    std::string technologyNode = "battery";
    for (auto iter = g_nodeInfo.begin(); iter != g_nodeInfo.end(); ++iter) {
        if (iter->first == "technology") {
            technologyNode = iter->second;
            break;
        }
    }
    std::string sysTechnologyPath = SYSTEM_BATTERY_PATH + "/" + technologyNode + "/" + "technology";
    HDF_LOGE("%{public}s: sysTechnologyPath is %{public}s", __func__, sysTechnologyPath.c_str());

    int fd = open(sysTechnologyPath.c_str(), O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, sysTechnologyPath.c_str());
        return "";
    }

    readSize = read(fd, buf, sizeof(buf) - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, sysTechnologyPath.c_str());
        close(fd);
        return "";
    }
    buf[readSize] = '\0';
    Trim(buf);

    battTechnology.assign(buf, strlen(buf));
    HDF_LOGE("%{public}s: read system file technology is %{public}s.", __func__, battTechnology.c_str());
    close(fd);

    HDF_LOGD("%{public}s: exit.", __func__);
    return battTechnology;
}

/**
 * @tc.name: HdiService001
 * @tc.desc: Test functions of ParseTemperature
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiService001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: HdiService001 start.", __func__);
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        MockFileInit();

        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/temp", "567");
    }

    int32_t temperature = 0;
    provider->ParseTemperature(&temperature);
    HDF_LOGD("%{public}s: HdiService001::temperature=%{public}d.", __func__, temperature);
    int32_t sysfsTemperature = ReadTemperatureSysfs();
    HDF_LOGD("%{public}s: HdiService001::sysfsTemperature=%{public}d.", __func__, sysfsTemperature);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(temperature == sysfsTemperature);
    } else {
        ASSERT_TRUE(temperature == 567);
    }
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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
        CreateFile("/data/local/tmp/battery/voltage_now", "4123456");
    }

    int32_t voltage = 0;
    provider->ParseVoltage(&voltage);
    HDF_LOGD("%{public}s: HdiService002::voltage=%{public}d.", __func__, voltage);
    int32_t sysfsVoltage = ReadVoltageSysfs();
    HDF_LOGD("%{public}s: HdiService002::sysfsVoltage=%{public}d.", __func__, sysfsVoltage);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(voltage == sysfsVoltage);
    } else {
        ASSERT_TRUE(voltage == 4123456);
    }
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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/capacity", "11");
    }

    int32_t capacity = -1;
    provider->ParseCapacity(&capacity);
    HDF_LOGD("%{public}s: HdiService003::capacity=%{public}d.", __func__, capacity);
    int32_t sysfsCapacity = ReadCapacitySysfs();
    HDF_LOGD("%{public}s: HdiService003::sysfsCapacity=%{public}d.", __func__, sysfsCapacity);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(capacity == sysfsCapacity);
    } else {
        ASSERT_TRUE(capacity == 11);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/health", "Good");
    }

    int32_t healthState = -1;
    provider->ParseHealthState(&healthState);
    HDF_LOGD("%{public}s: HdiService004::healthState=%{public}d.", __func__, healthState);
    int32_t sysfsHealthState = ReadHealthStateSysfs();
    HDF_LOGD("%{public}s: HdiService004::sysfsHealthState=%{public}d.", __func__, sysfsHealthState);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(healthState == sysfsHealthState);
    } else {
        ASSERT_TRUE(healthState == 1);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/online", "1");
        CreateFile("/data/local/tmp/battery/type", "Wireless");
        CreateFile("/data/local/tmp/ohos_charger/type", "Wireless");
        CreateFile("/data/local/tmp/ohos-fgu/type", "Wireless");
    }

    int32_t pluggedType = PowerSupplyProvider::PLUGGED_TYPE_NONE;
    provider->ParsePluggedType(&pluggedType);
    HDF_LOGD("%{public}s: HdiService005::pluggedType=%{public}d.", __func__, pluggedType);
    int32_t sysfsPluggedType = ReadPluggedTypeSysfs();
    HDF_LOGD("%{public}s: HdiService005::sysfsPluggedType=%{public}d.", __func__, sysfsPluggedType);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(pluggedType == sysfsPluggedType);
    } else {
        ASSERT_TRUE(pluggedType == PowerSupplyProvider::PLUGGED_TYPE_WIRELESS);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/status", "Not charging");
    }

    int32_t chargeState = PowerSupplyProvider::CHARGE_STATE_RESERVED;
    provider->ParseChargeState(&chargeState);
    HDF_LOGD("%{public}s: HdiService006::chargeState=%{public}d.", __func__, chargeState);
    int32_t sysfsChargeState = ReadChargeStateSysfs();
    HDF_LOGD("%{public}s: HdiService006::sysfsPluggedType=%{public}d.", __func__, sysfsChargeState);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(chargeState == sysfsChargeState);
    } else {
        ASSERT_TRUE(chargeState == PowerSupplyProvider::CHARGE_STATE_DISABLE);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/charge_counter", "12345");
    }

    int32_t chargeCounter = -1;
    provider->ParseChargeCounter(&chargeCounter);
    HDF_LOGD("%{public}s: HdiService007::chargeCounter=%{public}d.", __func__, chargeCounter);
    int32_t sysfsChargeCounter = ReadChargeCounterSysfs();
    HDF_LOGD("%{public}s: HdiService007::sysfsChargeCounter=%{public}d.", __func__, sysfsChargeCounter);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(chargeCounter == sysfsChargeCounter);
    } else {
        ASSERT_TRUE(chargeCounter == 12345);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/battery/present", "1");
    }

    int8_t present = -1;
    provider->ParsePresent(&present);
    HDF_LOGD("%{public}s: HdiService008::present=%{public}d.", __func__, present);
    int32_t sysfsPresent = ReadPresentSysfs();
    HDF_LOGD("%{public}s: HdiService008::sysfsPresent=%{public}d.", __func__, sysfsPresent);

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        ASSERT_TRUE(present == sysfsPresent);
    } else {
        ASSERT_TRUE(present == 1);
    }

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
    std::unique_ptr<PowerSupplyProvider> provider = std::make_unique<PowerSupplyProvider>();
    if (provider == nullptr) {
        HDF_LOGD("%{public}s: Failed to get PowerSupplyProvider", __func__);
        return;
    }

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("%{public}s: system battery file node exist", __func__);
        provider->InitPowerSupplySysfs();
    } else {
        HDF_LOGD("%{public}s: battery not exist", __func__);
        std::string path = "/data/local/tmp";
        provider->SetSysFilePath(path);
        provider->InitPowerSupplySysfs();
        CreateFile("/data/local/tmp/ohos-fgu/technology", "Li");
    }

    std::string technology = "invalid";
    provider->ParseTechnology(technology);
    HDF_LOGD("%{public}s: HdiService009::technology=%{public}s.", __func__, technology.c_str());
    std::string sysfsTechnology = "invalid";
    ReadTechnologySysfs(sysfsTechnology);
    HDF_LOGD("Read from test file sysfsTechnology=%{public}s.", sysfsTechnology.c_str());

    if ((access(SYSTEM_BATTERY_PATH.c_str(), F_OK) == 0) && (access(POWER_SUPPLY_SUB_PATH.c_str(), F_OK) == 0)) {
        HDF_LOGD("battery path exist. technology length=%{public}d.", technology.size());
        HDF_LOGD("battery path exist. sysfsTechnology length =%{public}d.", sysfsTechnology.size());
        ASSERT_TRUE(strcmp(technology.c_str(), sysfsTechnology.c_str()) == 0);
    } else {
        ASSERT_TRUE(technology == "Li");
    }

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
    void* service = nullptr;
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
    backlight->InitBacklightSysfs();
    auto ret = backlight->HandleBacklight(0);
    HDF_LOGD("%{public}s: HdiService025::ret==%{public}d", __func__, ret);
    backlight->TurnOnScreen();

    ASSERT_TRUE(ret != -1);
    HDF_LOGD("%{public}s: HdiService024 end.", __func__);
}
}

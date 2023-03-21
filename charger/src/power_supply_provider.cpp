/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "power_supply_provider.h"
#include "charger_log.h"
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <securec.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr int32_t MAX_SYSFS_SIZE = 64;
constexpr int32_t MAX_BUFF_SIZE = 128;
constexpr int32_t STR_TO_LONG_LEN = 10;
constexpr int32_t MKDIR_WAIT_TIME = 1;
constexpr int32_t NUM_ZERO = 0;
const std::string POWER_SUPPLY_BASE_PATH = "/sys/class/power_supply";
const std::string MOCK_POWER_SUPPLY_BASE_PATH = "/data/service/el0/battery";
const std::string POWER_SUPPLY_BATTERY = "Battery";
const std::string INVALID_STRING_VALUE = "invalid";
const std::string BATTERY_NODE_PATH = "battery";
} // namespace

BatterydInfo g_batteryInfo;

struct StringEnumMap {
    const char* str;
    int32_t enumVal;
};

PowerSupplyProvider::PowerSupplyProvider()
{
    path_ = POWER_SUPPLY_BASE_PATH;
    index_ = 0;
}

inline int32_t PowerSupplyProvider::ParseInt(const char* str)
{
    return static_cast<int32_t>(strtol(str, nullptr, STR_TO_LONG_LEN));
}

inline void PowerSupplyProvider::Trim(char* str)
{
    if (str == nullptr) {
        return;
    }

    str[strcspn(str, "\n")] = 0;
}

int32_t PowerSupplyProvider::ChargeStateEnumConverter(const char* str)
{
    struct StringEnumMap chargeStateEnumMap[] = {
        {"Discharging",  CHARGE_STATE_NONE    },
        {"Charging",     CHARGE_STATE_ENABLE  },
        {"Full",         CHARGE_STATE_FULL    },
        {"Not charging", CHARGE_STATE_DISABLE },
        {"Unknown",      CHARGE_STATE_RESERVED},
        {nullptr,        CHARGE_STATE_RESERVED},
    };

    for (int32_t i = 0; chargeStateEnumMap[i].str; ++i) {
        if (strcmp(str, chargeStateEnumMap[i].str) == 0) {
            return chargeStateEnumMap[i].enumVal;
        }
    }

    return CHARGE_STATE_RESERVED;
}

void PowerSupplyProvider::FormatPath(
    std::string& path, size_t size, const char* format, const char* basePath, const char* name) const
{
    char buff[PATH_MAX] = {0};
    if (strcpy_s(buff, PATH_MAX, path.c_str()) != EOK) {
        BATTERY_HILOGW(FEATURE_CHARGING, "failed to copy path of %{public}s", name);
        return;
    }

    if (snprintf_s(buff, PATH_MAX, size - 1, format, basePath, name) == -1) {
        BATTERY_HILOGW(FEATURE_CHARGING, "failed to format path of %{public}s", name);
        return;
    }
    path.assign(buff, strlen(buff));
}

void PowerSupplyProvider::FormatSysfsPaths()
{
    // Format paths for power supply types
    FormatPath(batterySysfsInfo_.capacityPath, PATH_MAX, "%s/%s/capacity", path_.c_str(),
        nodeNamePathMap_["capacity"].c_str());
    FormatPath(
        batterySysfsInfo_.temperaturePath, PATH_MAX, "%s/%s/temp", path_.c_str(), nodeNamePathMap_["temp"].c_str());
    FormatPath(
        batterySysfsInfo_.chargeStatePath, PATH_MAX, "%s/%s/status", path_.c_str(), nodeNamePathMap_["status"].c_str());
}

int32_t PowerSupplyProvider::ReadSysfsFile(const char* path, char* buf, size_t size) const
{
    int32_t fd = open(path, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH);
    if (fd < NUM_ZERO) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to open path");
        return HDF_ERR_IO;
    }

    size_t readSize = read(fd, buf, size - 1);
    buf[readSize] = '\0';
    Trim(buf);
    close(fd);

    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ReadBatterySysfsToBuff(const char* path, char* buf, size_t size) const
{
    int32_t ret = ReadSysfsFile(path, buf, size);
    if (ret != HDF_SUCCESS) {
        BATTERY_HILOGW(FEATURE_CHARGING, "read path failed, ret: %{public}d", ret);
        return ret;
    }

    return HDF_SUCCESS;
}

void PowerSupplyProvider::CreateFile(const std::string& path, const std::string& content)
{
    if (access(path.c_str(), F_OK) == 0) {
        return;
    }

    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        BATTERY_HILOGE(FEATURE_CHARGING, "cannot create file");
        return;
    }
    stream << content.c_str() << std::endl;
    stream.close();
}

void PowerSupplyProvider::InitBatteryPath()
{
    std::string sysLowercaseBatteryPath = "/sys/class/power_supply/battery";

    if (access(sysLowercaseBatteryPath.c_str(), F_OK) == 0) {
        BATTERY_HILOGI(FEATURE_CHARGING, "system battery path is exist");
        return;
    } else {
        std::string sysCapitalBatteryPath = "/sys/class/power_supply/Battery";
        if (access(sysCapitalBatteryPath.c_str(), F_OK) == 0) {
            BATTERY_HILOGI(FEATURE_CHARGING, "system Battery path is exist");
            return;
        }
        InitDefaultSysfs();
    }
}

int32_t PowerSupplyProvider::InitPowerSupplySysfs()
{
    DIR* dir;
    index_ = 0;

    dir = opendir(path_.c_str());
    if (dir == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "cannot open path_");
        return HDF_ERR_IO;
    }

    while (true) {
        struct dirent* entry = readdir(dir);
        if (entry == nullptr) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            if (index_ >= MAX_SYSFS_SIZE) {
                BATTERY_HILOGW(FEATURE_CHARGING, "too many power supply types");
                break;
            }
            nodeNames_.emplace_back(entry->d_name);
            index_++;
        }
    }
    nodeNamePathMap_.clear();
    TraversalNode();
    FormatSysfsPaths();
    BATTERY_HILOGD(FEATURE_CHARGING, "init power supply sysfs nodes, total count %{public}d", index_);
    closedir(dir);

    return HDF_SUCCESS;
}

void PowerSupplyProvider::TraversalNode()
{
    nodeNamePathMap_.insert(std::make_pair("capacity", ""));
    nodeNamePathMap_.insert(std::make_pair("temp", ""));
    nodeNamePathMap_.insert(std::make_pair("status", ""));

    auto iter = nodeNames_.begin();
    while (iter != nodeNames_.end()) {
        if (*iter == "battery") {
            CheckSubfolderNode(*iter);
            iter = nodeNames_.erase(iter);
        } else {
            iter++;
        }
    }

    iter = nodeNames_.begin();
    while (iter != nodeNames_.end()) {
        if (*iter == POWER_SUPPLY_BATTERY) {
            CheckSubfolderNode(*iter);
            iter = nodeNames_.erase(iter);
        } else {
            iter++;
        }
    }

    for (auto& nodeName : nodeNames_) {
        CheckSubfolderNode(nodeName);
    }
}

void PowerSupplyProvider::CheckSubfolderNode(const std::string& path)
{
    DIR* dir;
    std::string batteryPath = path_ + "/" + path;

    dir = opendir(batteryPath.c_str());
    if (dir == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "subfolder file is not exist.");
        return;
    }

    while (true) {
        struct dirent* entry = readdir(dir);
        if (entry == nullptr) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            continue;
        }

        if ((strcmp(entry->d_name, "type") == 0) && (nodeNamePathMap_["type"].empty()) &&
            (strcasecmp(path.c_str(), BATTERY_NODE_PATH.c_str()) != 0)) {
            nodeNamePathMap_["type"] = path;
        }

        for (auto& iter : nodeNamePathMap_) {
            if ((strcmp(entry->d_name, iter.first.c_str()) == 0) && (nodeNamePathMap_[iter.first].empty())) {
                nodeNamePathMap_[iter.first] = path;
            }
        }
    }
    closedir(dir);
}

int32_t PowerSupplyProvider::ParseCapacity(int32_t* capacity) const
{
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.capacityPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    *capacity = value;

    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseTemperature(int32_t* temperature) const
{
    char buf[MAX_BUFF_SIZE] = {0};
    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.temperaturePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    *temperature = value;

    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseChargeState(int32_t* chargeState) const
{
    char buf[MAX_BUFF_SIZE] = {0};
    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.chargeStatePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    Trim(buf);
    *chargeState = ChargeStateEnumConverter(buf);
    return HDF_SUCCESS;
}

void PowerSupplyProvider::CreateMockTechPath(std::string& mockTechPath)
{
    BATTERY_HILOGI(FEATURE_CHARGING, "create mockFilePath path");
    CreateFile(mockTechPath + "/capacity", "1000");
    CreateFile(mockTechPath + "/status", "Not charging");
    CreateFile(mockTechPath + "/temp", "345");
}

void PowerSupplyProvider::CreateMockChargerPath(std::string& mockChargerPath)
{
    BATTERY_HILOGI(FEATURE_CHARGING, "create mockFilePath path");
    CreateFile(mockChargerPath + "/type", "USB");
    CreateFile(mockChargerPath + "/constant_charge_current", "0");
    CreateFile(mockChargerPath + "/health", "Good");
    CreateFile(mockChargerPath + "/online", "1");
    CreateFile(mockChargerPath + "/status", "Charging");
    CreateFile(mockChargerPath + "/type", "USB");
}

void PowerSupplyProvider::CreateMockBatteryPath(std::string& mockBatteryPath)
{
    BATTERY_HILOGI(FEATURE_CHARGING, "create mockFilePath path");
    CreateFile(mockBatteryPath + "/capacity", "11");
    CreateFile(mockBatteryPath + "/status", "Charging");
    CreateFile(mockBatteryPath + "/temp", "222");
}

void PowerSupplyProvider::InitDefaultSysfs()
{
    std::string mockBatteryPath = MOCK_POWER_SUPPLY_BASE_PATH + "/battery";
    std::string mockChargerPath = MOCK_POWER_SUPPLY_BASE_PATH + "/ohos_charger";
    std::string mockTechPath = MOCK_POWER_SUPPLY_BASE_PATH + "/ohos-fgu";

    if (access(mockBatteryPath.c_str(), 0) == -1) {
        mkdir(mockBatteryPath.c_str(), S_IRWXU | S_IRWXG);
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(mockChargerPath.c_str(), 0) == -1) {
        mkdir(mockChargerPath.c_str(), S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(mockTechPath.c_str(), 0) == -1) {
        mkdir(mockTechPath.c_str(), S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    CreateMockTechPath(mockTechPath);
    CreateMockChargerPath(mockChargerPath);
    CreateMockBatteryPath(mockBatteryPath);
    path_ = MOCK_POWER_SUPPLY_BASE_PATH;
}
} // namespace PowerMgr
} // namespace OHOS

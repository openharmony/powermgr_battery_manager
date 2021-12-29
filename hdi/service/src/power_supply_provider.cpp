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

#include "power_supply_provider.h"

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <securec.h>
#include <unistd.h>
#include "osal/osal_mem.h"

#include "utils/hdf_log.h"

#define HDF_LOG_TAG power_supply_provider

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const int MAX_BUFF_SIZE = 128;
const int INVALID_BATT_INT_VALUE = -1;
const int STR_TO_LONG_LEN = 10;
const int UVOL_TO_MVOL = 1000;
const int MKDIR_WAIT_TIME = 1;
const std::string POWER_SUPPLY_BASE_PATH = "/sys/class/power_supply";
const std::string POWER_SUPPLY_BATTERY = "Battery";
const std::string BATTERY_KEY_CAPACITY = "POWER_SUPPLY_CAPACITY=";
const std::string BATTERY_KEY_VOLTAGE = "POWER_SUPPLY_VOLTAGE_NOW=";
const std::string BATTERY_KEY_TEMPERATURE = "POWER_SUPPLY_TEMP=";
const std::string BATTERY_KEY_HEALTH = "POWER_SUPPLY_HEALTH=";
const std::string BATTERY_KEY_CHARGE_STATUS = "POWER_SUPPLY_STATUS=";
const std::string BATTERY_KEY_PRESENT = "POWER_SUPPLY_PRESENT=";
const std::string BATTERY_KEY_TECHNOLOGY = "POWER_SUPPLY_TECHNOLOGY=";
const std::string BATTERY_KEY_CHARGE_COUNTER = "POWER_SUPPLY_CHARGE_COUNTER=";
const std::string INVALID_STRING_VALUE = "invalid";
const std::string BATTERY_NODE_PATH = "battery";
const std::string CHARGER_NODE_PATH = "bq2560x_charger";
const std::string FGU_NODE_PATH = "sc27xx-fgu";

struct StringEnumMap {
    const char* str;
    int32_t enumVal;
};

struct BatteryAssigner {
    const char* prefix;
    const size_t prefixLen;
    void (*Assigner)(const char*, struct BatterydInfo*);
};

PowerSupplyProvider::PowerSupplyProvider()
{
    HDF_LOGI("%{public}s enter", __func__);
    path_ = POWER_SUPPLY_BASE_PATH;
}

inline int32_t PowerSupplyProvider::ParseInt(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    return strtol(str, nullptr, STR_TO_LONG_LEN);
}

inline void PowerSupplyProvider::Trim(char* str) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (str == nullptr) {
        return;
    }

    str[strcspn(str, "\n")] = 0;
}

inline void PowerSupplyProvider::CapacityAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->capacity_ = ParseInt(str); // default in percent format
}

inline void PowerSupplyProvider::VoltageAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->voltage_ = ParseInt(str) / UVOL_TO_MVOL; // convert to millivolt(mV) format
}

inline void PowerSupplyProvider::TemperatureAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->temperature_ = ParseInt(str);
}

int32_t PowerSupplyProvider::HealthStateEnumConverter(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap healthStateEnumMap[] = {
        {"Good", BATTERY_HEALTH_GOOD},
        {"Cold", BATTERY_HEALTH_COLD},
        {"Warm", BATTERY_HEALTH_GOOD}, // JEITA specification
        {"Cool", BATTERY_HEALTH_GOOD}, // JEITA specification
        {"Hot", BATTERY_HEALTH_OVERHEAT}, // JEITA specification
        {"Overheat", BATTERY_HEALTH_OVERHEAT},
        {"Over voltage", BATTERY_HEALTH_OVERVOLTAGE},
        {"Dead", BATTERY_HEALTH_DEAD},
        {"Unknown", BATTERY_HEALTH_UNKNOWN},
        {"Unspecified failure", BATTERY_HEALTH_UNKNOWN},
        {NULL, BATTERY_HEALTH_UNKNOWN},
    };

    for (int i = 0; healthStateEnumMap[i].str; ++i) {
        if (strcmp(str, healthStateEnumMap[i].str) == 0) {
            return healthStateEnumMap[i].enumVal;
        }
    }

    return BATTERY_HEALTH_UNKNOWN;
}

inline void PowerSupplyProvider::HealthStateAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->healthState_ = HealthStateEnumConverter(str);
}

int32_t PowerSupplyProvider::ChargeStateEnumConverter(const char* str)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap chargeStateEnumMap[] = {
        {"Discharging", CHARGE_STATE_NONE},
        {"Charging", CHARGE_STATE_ENABLE},
        {"Full", CHARGE_STATE_FULL},
        {"Not charging", CHARGE_STATE_DISABLE},
        {"Unknown", CHARGE_STATE_RESERVED},
        {NULL, CHARGE_STATE_RESERVED},
    };

    for (int i = 0; chargeStateEnumMap[i].str; ++i) {
        if (strcmp(str, chargeStateEnumMap[i].str) == 0) {
            return chargeStateEnumMap[i].enumVal;
        }
    }

    return CHARGE_STATE_RESERVED;
}

inline void PowerSupplyProvider::ChargeStateAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->chargeState_ = ChargeStateEnumConverter(str);
}

inline void PowerSupplyProvider::PresentAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->present_ = ParseInt(str);
}

inline void PowerSupplyProvider::TechnologyAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->technology_ = str;
}

inline void PowerSupplyProvider::ChargeCounterAssigner(const char* str, struct BatterydInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    info->chargeCounter_ = ParseInt(str);
}

void PowerSupplyProvider::FormatPath(std::string& path,
    size_t size, const char* format, const char* basePath, const char* name) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buff[PATH_MAX] = {0};
    if (strcpy_s(buff, PATH_MAX, path.c_str()) != EOK) {
        HDF_LOGW("%{public}s: failed to copy path of %{public}s", __func__, name);
        return;
    }

    if (snprintf_s(buff, PATH_MAX, size - 1, format, basePath, name) == -1) {
        HDF_LOGW("%{public}s: failed to format path of %{public}s", __func__, name);
        return;
    }
    path.assign(buff, strlen(buff));
    HDF_LOGI("%{public}s: path is %{public}s", __func__, path.c_str());

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void PowerSupplyProvider::FormatSysfsPaths(struct PowerSupplySysfsInfo* info)
{
    HDF_LOGI("%{public}s enter", __func__);
    // Format paths for power supply types
    FormatPath(info->typePath, PATH_MAX, "%s/%s/type",
        path_.c_str(), nodeInfo_["type"].c_str());
    FormatPath(info->onlinePath, PATH_MAX, "%s/%s/online",
        path_.c_str(), nodeInfo_["online"].c_str());
    FormatPath(info->currentMaxPath, PATH_MAX, "%s/%s/current_max",
        path_.c_str(), nodeInfo_["current_max"].c_str());
    FormatPath(info->voltageMaxPath, PATH_MAX, "%s/%s/voltage_max",
        path_.c_str(), nodeInfo_["voltage_max"].c_str());

    FormatPath(batterySysfsInfo_.capacityPath, PATH_MAX,
        "%s/%s/capacity", path_.c_str(), nodeInfo_["capacity"].c_str());
    FormatPath(batterySysfsInfo_.voltagePath, PATH_MAX,
        "%s/%s/voltage_now", path_.c_str(), nodeInfo_["voltage_now"].c_str());
    FormatPath(batterySysfsInfo_.temperaturePath, PATH_MAX,
        "%s/%s/temp", path_.c_str(), nodeInfo_["temp"].c_str());
    FormatPath(batterySysfsInfo_.healthStatePath, PATH_MAX,
        "%s/%s/health", path_.c_str(), nodeInfo_["health"].c_str());
    FormatPath(batterySysfsInfo_.chargeStatePath, PATH_MAX,
        "%s/%s/status", path_.c_str(), nodeInfo_["status"].c_str());
    FormatPath(batterySysfsInfo_.presentPath, PATH_MAX,
        "%s/%s/present", path_.c_str(), nodeInfo_["present"].c_str());
    FormatPath(batterySysfsInfo_.chargeCounterPath, PATH_MAX,
        "%s/%s/charge_counter", path_.c_str(), nodeInfo_["charge_counter"].c_str());
    FormatPath(batterySysfsInfo_.technologyPath, PATH_MAX,
        "%s/%s/technology", path_.c_str(), nodeInfo_["technology"].c_str());

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

int32_t PowerSupplyProvider::ReadSysfsFile(const char* path, char* buf, size_t size) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t readSize;
    int fd = open(path, O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{public}s", __func__, path);
        return HDF_ERR_IO;
    }

    readSize = read(fd, buf, size - 1);
    if (readSize < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{public}s", __func__, path);
        close(fd);
        return HDF_ERR_IO;
    }

    buf[readSize] = '\0';
    Trim(buf);
    close(fd);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ReadBatterySysfsToBuff(const char* path, char* buf, size_t size) const
{
    HDF_LOGI("%{public}s enter", __func__);
    int32_t ret;

    ret = ReadSysfsFile(path, buf, size);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("%{public}s: read path %{private}s failed, ret: %{public}d", __func__, path, ret);
        return ret;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void PowerSupplyProvider::GetPluggedTypeName(char* buf, size_t size) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string onlineNode = "USB";
    int32_t ret;
    int32_t online;
    std::string onlinePath;

    auto iter = filenodeName_.begin();
    while (iter != filenodeName_.end()) {
        onlinePath = path_ + "/" + *iter + "/" + "online";
        ret = ReadSysfsFile(onlinePath.c_str(), buf, size);
        if (ret != HDF_SUCCESS) {
            HDF_LOGW("%{public}s: read online path failed in loop, ret: %{public}d", __func__, ret);
        }
        online = ParseInt(buf);
        if (online) {
            onlineNode = *iter;
            break;
        }
        iter++;
    }

    HDF_LOGI("%{public}s: online path is: %{public}s", __func__, onlinePath.c_str());
    ret = ReadSysfsFile(onlinePath.c_str(), buf, size);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("%{public}s: read online path failed, ret: %{public}d", __func__, ret);
        return;
    }

    online = ParseInt(buf);
    if (!online) {
        HDF_LOGW("%{public}s: charger is not online, so no type return.", __func__);
        return;
    }

    std::string typeNode = onlineNode;
    std::string typePath = path_ + "/" + typeNode + "/" + "type";
    HDF_LOGI("%{public}s: type path is: %{public}s", __func__, typePath.c_str());
    ret = ReadSysfsFile(typePath.c_str(), buf, size);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("%{public}s: read type path failed, ret: %{public}d", __func__, ret);
        return;
    }
    Trim(buf);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

int32_t PowerSupplyProvider::PluggedTypeEnumConverter(const char* str) const
{
    HDF_LOGI("%{public}s enter", __func__);
    struct StringEnumMap pluggedTypeEnumMap[] = {
        {"USB", PLUGGED_TYPE_USB},
        {"USB_PD_DRP", PLUGGED_TYPE_USB},
        {"Wireless", PLUGGED_TYPE_WIRELESS},
        {"Mains", PLUGGED_TYPE_AC},
        {"UPS", PLUGGED_TYPE_AC},
        {"USB_ACA", PLUGGED_TYPE_AC},
        {"USB_C", PLUGGED_TYPE_AC},
        {"USB_CDP", PLUGGED_TYPE_AC},
        {"USB_DCP", PLUGGED_TYPE_AC},
        {"USB_HVDCP", PLUGGED_TYPE_AC},
        {"USB_PD", PLUGGED_TYPE_AC},
        {"Unknown", PLUGGED_TYPE_BUTT},
        {NULL, PLUGGED_TYPE_BUTT},
    };

    for (int i = 0; pluggedTypeEnumMap[i].str; ++i) {
        if (strcmp(str, pluggedTypeEnumMap[i].str) == 0) {
            return pluggedTypeEnumMap[i].enumVal;
        }
    }

    HDF_LOGI("%{public}s exit", __func__);
    return PLUGGED_TYPE_BUTT;
}

int32_t PowerSupplyProvider::ParsePluggedMaxCurrent(int32_t* maxCurrent) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    GetPluggedTypeName(buf, sizeof(buf));
    HDF_LOGD("%{public}s buf is: %{public}s", __func__, buf);
    std::string currentMaxNode = "Battery";
    for (auto iter = nodeInfo_.begin(); iter != nodeInfo_.end(); ++iter) {
        if (iter->first == "current_max") {
            currentMaxNode = iter->second;
            break;
        }
    }
    std::string currentMaxPath = POWER_SUPPLY_BASE_PATH + "/" + currentMaxNode + "/" + "current_max";
    int32_t ret = ReadBatterySysfsToBuff(currentMaxPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: maxCurrent is %{public}d", __func__, value);
    *maxCurrent = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParsePluggedMaxVoltage(int32_t* maxVoltage) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    GetPluggedTypeName(buf, sizeof(buf));
    std::string voltageMaxNode = "Battery";
    for (auto iter = nodeInfo_.begin(); iter != nodeInfo_.end(); ++iter) {
        if (iter->first == "voltage_max") {
            voltageMaxNode = iter->second;
            break;
        }
    }
    std::string voltageMaxPath = POWER_SUPPLY_BASE_PATH + "/" + voltageMaxNode + "/" + "voltage_max";
    int32_t ret = ReadBatterySysfsToBuff(voltageMaxPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: maxCurrent is %{public}d", __func__, value);
    *maxVoltage = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void PowerSupplyProvider::UpdateInfoByReadSysFile(struct BatterydInfo* info) const
{
    HDF_LOGI("%{public}s enter", __func__);
    ParseCapacity(&info->capacity_);
    ParseVoltage(&info->voltage_);
    ParseTemperature(&info->temperature_);
    ParseHealthState(&info->healthState_);
    ParseChargeState(&info->chargeState_);
    ParseChargeCounter(&info->chargeCounter_);
    ParsePresent(&info->present_);

    info->pluggedType_ = PLUGGED_TYPE_NONE;
    ParsePluggedType(&info->pluggedType_);

    info->pluggedMaxCurrent_ = INVALID_BATT_INT_VALUE;
    ParsePluggedMaxCurrent(&info->pluggedMaxCurrent_);

    info->pluggedMaxVoltage_ = INVALID_BATT_INT_VALUE;
    ParsePluggedMaxVoltage(&info->pluggedMaxVoltage_);

    info->technology_ = INVALID_STRING_VALUE;
    ParseTechnology(info->technology_);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void PowerSupplyProvider::ParseUeventToBatterydInfo(const char* msg, struct BatterydInfo* info) const
{
    HDF_LOGI("%{public}s enter", __func__);
    static struct BatteryAssigner batteryAssigners[] = {
        {BATTERY_KEY_CAPACITY.c_str(), BATTERY_KEY_CAPACITY.length(), CapacityAssigner},
        {BATTERY_KEY_VOLTAGE.c_str(), BATTERY_KEY_VOLTAGE.length(), VoltageAssigner},
        {BATTERY_KEY_TEMPERATURE.c_str(), BATTERY_KEY_TEMPERATURE.length(), TemperatureAssigner},
        {BATTERY_KEY_HEALTH.c_str(), BATTERY_KEY_HEALTH.length(), HealthStateAssigner},
        {BATTERY_KEY_CHARGE_STATUS.c_str(), BATTERY_KEY_CHARGE_STATUS.length(), ChargeStateAssigner},
        {BATTERY_KEY_PRESENT.c_str(), BATTERY_KEY_PRESENT.length(), PresentAssigner},
        {BATTERY_KEY_TECHNOLOGY.c_str(), BATTERY_KEY_TECHNOLOGY.length(), TechnologyAssigner},
        {BATTERY_KEY_CHARGE_COUNTER.c_str(), BATTERY_KEY_CHARGE_COUNTER.length(), ChargeCounterAssigner},
        {NULL, 0, NULL} // end of the array
    };

    while (*msg) {
        for (int i = 0; batteryAssigners[i].prefix; ++i) {
            if (!strncmp(msg, batteryAssigners[i].prefix, batteryAssigners[i].prefixLen)) {
                HDF_LOGD("%{public}s: msg: %{public}s", __func__, msg);
                msg += batteryAssigners[i].prefixLen;
                batteryAssigners[i].Assigner(msg, info);
                break;
            }
        }
        while (*msg++) {} // move to next
    }

    info->pluggedType_ = PLUGGED_TYPE_NONE;
    ParsePluggedType(&info->pluggedType_);

    info->pluggedMaxCurrent_ = INVALID_BATT_INT_VALUE;
    ParsePluggedMaxCurrent(&info->pluggedMaxCurrent_);

    info->pluggedMaxVoltage_ = INVALID_BATT_INT_VALUE;
    ParsePluggedMaxVoltage(&info->pluggedMaxVoltage_);

    info->technology_ = INVALID_STRING_VALUE;
    ParseTechnology(info->technology_);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void PowerSupplyProvider::SetSysFilePath(const std::string& path)
{
    HDF_LOGI("%{public}s enter", __func__);
    path_ = path;
    HDF_LOGD("%{public}s: path is %{public}s", __func__, path.c_str());
}

std::string PowerSupplyProvider::CreateFile(std::string path, std::string content)
{
    HDF_LOGI("%{public}s enter", __func__);
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: Cannot create file %{public}s", __func__, path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

void PowerSupplyProvider::InitBatteryPath()
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string sysBatteryPath = "/sys/class/power_supply/battery";
    if (access(sysBatteryPath.c_str(), F_OK) == 0) {
        HDF_LOGI("%{public}s: system battery path is exist", __func__);
        return;
    } else {
        HDF_LOGI("%{public}s: create mock battery path", __func__);
        InitDefaultSysfs();
    }

    return;
    HDF_LOGI("%{public}s exit", __func__);
}

int32_t PowerSupplyProvider::InitPowerSupplySysfs(void)
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    index_ = 0;

    HDF_LOGD("%{public}s: path_ is %{public}s", __func__, path_.c_str());
    dir = opendir(path_.c_str());
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
            if (index_ >= MAX_SYSFS_SIZE) {
                HDF_LOGE("%{public}s: too many plugged types", __func__);
                break;
            }
            filenodeName_.emplace_back(entry->d_name);
            index_++;
        }
    }
    struct PowerSupplySysfsInfo sysfsInfo = {0};

    TraversalNode();
    FormatSysfsPaths(&sysfsInfo);
    HDF_LOGD("%{public}s: index_ is %{public}d", __func__, index_);
    closedir(dir);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void PowerSupplyProvider::TraversalNode()
{
    HDF_LOGI("%{public}s enter", __func__);
    nodeInfo_.insert(std::make_pair("type", ""));
    nodeInfo_.insert(std::make_pair("online", ""));
    nodeInfo_.insert(std::make_pair("current_max", ""));
    nodeInfo_.insert(std::make_pair("voltage_max", ""));
    nodeInfo_.insert(std::make_pair("capacity", ""));
    nodeInfo_.insert(std::make_pair("voltage_now", ""));
    nodeInfo_.insert(std::make_pair("temp", ""));
    nodeInfo_.insert(std::make_pair("health", ""));
    nodeInfo_.insert(std::make_pair("status", ""));
    nodeInfo_.insert(std::make_pair("present", ""));
    nodeInfo_.insert(std::make_pair("charge_counter", ""));
    nodeInfo_.insert(std::make_pair("technology", ""));

    auto iter = filenodeName_.begin();
    while (iter != filenodeName_.end()) {
        if (*iter == "battery") {
            CheckSubfolderNode(*iter);
            iter = filenodeName_.erase(iter);
        } else {
            iter++;
        }
    }

    iter = filenodeName_.begin();
    while (iter != filenodeName_.end()) {
        if (*iter == "Battery") {
            CheckSubfolderNode(*iter);
            iter = filenodeName_.erase(iter);
        } else {
            iter++;
        }
    }

    for (auto it = filenodeName_.begin(); it != filenodeName_.end(); ++it) {
        CheckSubfolderNode(*it);
    }
    HDF_LOGI("%{public}s exit", __func__);
}

void PowerSupplyProvider::CheckSubfolderNode(const std::string& path)
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR *dir = nullptr;
    struct dirent* entry = nullptr;
    std::string batteryPath = path_ + "/" + path;
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
        if ((strcmp(entry->d_name, "type") == 0) && (nodeInfo_["type"] == "") &&
            (strcasecmp(path.c_str(), BATTERY_NODE_PATH.c_str()) != 0)) {
            nodeInfo_["type"] = path;
        } else if ((strcmp(entry->d_name, "online") == 0) && (nodeInfo_["online"] == "")) {
            nodeInfo_["online"] = path;
        } else if ((strcmp(entry->d_name, "current_max") == 0) && (nodeInfo_["current_max"] == "")) {
            nodeInfo_["current_max"] = path;
        } else if ((strcmp(entry->d_name, "voltage_max") == 0) && (nodeInfo_["voltage_max"] == "")) {
            nodeInfo_["voltage_max"] = path;
        } else if ((strcmp(entry->d_name, "capacity") == 0) && (nodeInfo_["capacity"] == "")) {
            nodeInfo_["capacity"] = path;
        } else if ((strcmp(entry->d_name, "voltage_now") == 0) && (nodeInfo_["voltage_now"] == "")) {
            nodeInfo_["voltage_now"] = path;
        } else if ((strcmp(entry->d_name, "temp") == 0) && (nodeInfo_["temp"] == "")) {
            nodeInfo_["temp"] = "battery";
        } else if ((strcmp(entry->d_name, "health") == 0) && (nodeInfo_["health"] == "")) {
            nodeInfo_["health"] = path;
        } else if ((strcmp(entry->d_name, "status") == 0) && (nodeInfo_["status"] == "")) {
            nodeInfo_["status"] = path;
        } else if ((strcmp(entry->d_name, "present") == 0) && (nodeInfo_["present"] == "")) {
            nodeInfo_["present"] = path;
        } else if ((strcmp(entry->d_name, "charge_counter") == 0) && (nodeInfo_["charge_counter"] == "")) {
            nodeInfo_["charge_counter"] = path;
        } else if ((strcmp(entry->d_name, "technology") == 0) && (nodeInfo_["technology"] == "")) {
            nodeInfo_["technology"] = path;
        } else {
            HDF_LOGI("%{public}s: battery node other branch is excute.", __func__);
        }
    }
    closedir(dir);
    HDF_LOGI("%{public}s exit", __func__);
}

int32_t PowerSupplyProvider::ParseCapacity(int32_t* capacity) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.capacityPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: capacity is %{public}d", __func__, value);
    *capacity = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseVoltage(int32_t* voltage) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.voltagePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: voltage is %{public}d", __func__, value);
    *voltage = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseTemperature(int32_t* temperature) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.temperaturePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: temperature is %{public}d", __func__, value);
    *temperature = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseHealthState(int32_t* healthState) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.healthStatePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    Trim(buf);
    *healthState = HealthStateEnumConverter(buf);
    HDF_LOGD("%{public}s: healthState is %{public}d", __func__, *healthState);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParsePluggedType(int32_t* pluggedType) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    GetPluggedTypeName(buf, sizeof(buf));
    int32_t type = PluggedTypeEnumConverter(buf);
    if (type == PLUGGED_TYPE_BUTT) {
        HDF_LOGW("%{public}s: not support the online type %{public}s", __func__, buf);
        return HDF_ERR_NOT_SUPPORT;
    }

    HDF_LOGD("%{public}s: return online plugged type %{public}d", __func__, type);
    *pluggedType = type;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseChargeState(int32_t* chargeState) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};
    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.chargeStatePath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    Trim(buf);
    *chargeState = ChargeStateEnumConverter(buf);
    HDF_LOGD("%{public}s: chargeState is %{public}d", __func__, *chargeState);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParsePresent(int8_t* present) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};
    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.presentPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int8_t value = (int8_t)ParseInt(buf);
    HDF_LOGD("%{public}s: present is %{public}d", __func__, value);
    *present = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseChargeCounter(int32_t* chargeCounter) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.chargeCounterPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    int32_t value = ParseInt(buf);
    HDF_LOGD("%{public}s: temperature is %{public}d", __func__, value);
    *chargeCounter = value;

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t PowerSupplyProvider::ParseTechnology(std::string& technology) const
{
    HDF_LOGI("%{public}s enter", __func__);
    char buf[MAX_BUFF_SIZE] = {0};
    HDF_LOGD("%{public}s: technology path is %{public}s", __func__, batterySysfsInfo_.technologyPath.c_str());

    int32_t ret = ReadBatterySysfsToBuff(batterySysfsInfo_.technologyPath.c_str(), buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    technology.assign(buf, strlen(buf));
    HDF_LOGD("%{public}s: technology is %{public}s", __func__, technology.c_str());

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void PowerSupplyProvider::InitDefaultSysfs(void)
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string mockBatteryPath = "/data/local/tmp/battery";
    std::string mockChargerPath = "/data/local/tmp/bq2560x_charger";
    std::string mockTechPath = "/data/local/tmp/sc27xx-fgu";
    if (access(mockBatteryPath.c_str(), 0) == -1) {
        mkdir("/data/local/tmp/battery", S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(mockChargerPath.c_str(), 0) == -1) {
        mkdir("/data/local/tmp/bq2560x_charger", S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(mockTechPath.c_str(), 0) == -1) {
        mkdir("/data/local/tmp/sc27xx-fgu", S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    HDF_LOGD("%{public}s: create mock path for Hi3516DV300", __func__);
    CreateFile("/data/local/tmp/sc27xx-fgu/capacity", "1000");
    CreateFile("/data/local/tmp/sc27xx-fgu/current_avg", "1000");
    CreateFile("/data/local/tmp/sc27xx-fgu/current_now", "1000");
    CreateFile("/data/local/tmp/sc27xx-fgu/health", "Over voltage");
    CreateFile("/data/local/tmp/sc27xx-fgu/present", "0");
    CreateFile("/data/local/tmp/sc27xx-fgu/status", "Not charging");
    CreateFile("/data/local/tmp/sc27xx-fgu/type", "Unknown");
    CreateFile("/data/local/tmp/sc27xx-fgu/temp", "345");
    CreateFile("/data/local/tmp/sc27xx-fgu/technology", "Li-ion");
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB");
    CreateFile("/data/local/tmp/bq2560x_charger/constant_charge_current", "0");
    CreateFile("/data/local/tmp/bq2560x_charger/health", "Good");
    CreateFile("/data/local/tmp/bq2560x_charger/online", "1");
    CreateFile("/data/local/tmp/bq2560x_charger/status", "Charging");
    CreateFile("/data/local/tmp/bq2560x_charger/type", "USB");
    CreateFile("/data/local/tmp/battery/capacity", "11");
    CreateFile("/data/local/tmp/battery/charge_control_limit", "0");
    CreateFile("/data/local/tmp/battery/charge_counter", "4000000");
    CreateFile("/data/local/tmp/battery/charge_full", "4000000");
    CreateFile("/data/local/tmp/battery/charge_full_design", "4000000");
    CreateFile("/data/local/tmp/battery/constant_charge_current", "0");
    CreateFile("/data/local/tmp/battery/current_avg", "1000");
    CreateFile("/data/local/tmp/battery/current_now", "1000");
    CreateFile("/data/local/tmp/battery/health", "Good");
    CreateFile("/data/local/tmp/battery/input_current_limit", "0");
    CreateFile("/data/local/tmp/battery/online", "1");
    CreateFile("/data/local/tmp/battery/present", "0");
    CreateFile("/data/local/tmp/battery/status", "Full");
    CreateFile("/data/local/tmp/battery/temp", "222");
    CreateFile("/data/local/tmp/battery/voltage_avg", "4123456");
    CreateFile("/data/local/tmp/battery/voltage_now", "4123456");
    CreateFile("/data/local/tmp/battery/type", "Battery");
    path_ = "/data/local/tmp";
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

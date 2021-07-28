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

#include "batteryd_parser.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <securec.h>
#include <unistd.h>

#include "batteryd.h"
#include "utils/hdf_log.h"
#include "osal/osal_mem.h"

#define HDF_LOG_TAG batteryd_parser

#define MAX_SYSFS_SIZE 64
#define MAX_BUFF_SIZE 128
#define INVALID_BATT_INT_VALUE (-1)

#define POWER_SUPPLY_BASE_PATH "/sys/class/power_supply"
#define POWER_SUPPLY_TYPE_BATTERY "Battery"

#define BATTERY_KEY_CAPACITY "POWER_SUPPLY_CAPACITY="
#define BATTERY_KEY_VOLTAGE "POWER_SUPPLY_VOLTAGE_NOW="
#define BATTERY_KEY_TEMPERATURE "POWER_SUPPLY_TEMP="
#define BATTERY_KEY_HEALTH "POWER_SUPPLY_HEALTH="
#define BATTERY_KEY_CHARGE_STATUS "POWER_SUPPLY_STATUS="
#define BATTERY_KEY_PRESENT "POWER_SUPPLY_PRESENT="
#define BATTERY_KEY_TECHNOLOGY "POWER_SUPPLY_TECHNOLOGY="
#define BATTERY_KEY_CHARGE_COUNTER "POWER_SUPPLY_CHARGE_COUNTER="

struct PowerSupplySysfsInfo {
    char *name;
    char typePath[PATH_MAX];
    char onlinePath[PATH_MAX];
    char currentMaxPath[PATH_MAX];
    char voltageMaxPath[PATH_MAX];
};

static struct PowerSupplySysfsInfo g_powerSupplySysfsInfos[MAX_SYSFS_SIZE];

struct BatterySysfsInfo {
    char *name;
    char capacityPath[PATH_MAX];
    char voltagePath[PATH_MAX];
    char temperaturePath[PATH_MAX];
    char healthStatePath[PATH_MAX];
    char chargeStatePath[PATH_MAX];
    char presentPath[PATH_MAX];
    char technologyPath[PATH_MAX];
};

static struct BatterySysfsInfo g_batterySysfsInfo;

// Keep it same as the BatteryHealthState in battery_info.h
enum BatteryHealthState {
    BATTERY_HEALTH_UNKNOWN = 0,
    BATTERY_HEALTH_GOOD,
    BATTERY_HEALTH_OVERHEAT,
    BATTERY_HEALTH_OVERVOLTAGE,
    BATTERY_HEALTH_COLD,
    BATTERY_HEALTH_DEAD,
    BATTERY_HEALTH_RESERVED,
};

// Keep it same as the BatteryChargeState in battery_info.h
enum BatteryChargeState {
    CHARGE_STATE_NONE = 0,
    CHARGE_STATE_ENABLE,
    CHARGE_STATE_DISABLE,
    CHARGE_STATE_FULL,
    CHARGE_STATE_RESERVED,
};

// Keep it same as the BatteryPluggedType in battery_info.h
enum BatteryPluggedType {
    PLUGGED_TYPE_NONE = 0,
    PLUGGED_TYPE_AC,
    PLUGGED_TYPE_USB,
    PLUGGED_TYPE_WIRELESS,
    PLUGGED_TYPE_BUTT
};

struct StringEnumMap {
    char *str;
    int32_t enumVal;
};

struct StringEnumMap g_healthStateEnumMap[] = {
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

struct StringEnumMap g_chargeStateEnumMap[] = {
    {"Discharging", CHARGE_STATE_NONE},
    {"Charging", CHARGE_STATE_ENABLE},
    {"Full", CHARGE_STATE_FULL},
    {"Not charging", CHARGE_STATE_DISABLE},
    {"Unknown", CHARGE_STATE_RESERVED},
    {NULL, CHARGE_STATE_RESERVED},
};

struct StringEnumMap g_pluggedTypeEnumMap[] = {
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

struct BatteryAssigner {
    const char *prefix;
    const size_t prefixLen;
    void (*Assigner)(const char *, struct BatterydInfo *);
};

inline static int32_t ParseInt(const char *str)
{
    return strtol(str, NULL, 10);
}

inline static void TrimNewLine(char *str)
{
    if (str == NULL) {
        return;
    }
    str[strcspn(str, "\n")] = 0;
}

static int32_t ReadSysfsFile(const char *path, char *buf, size_t size)
{
    int32_t ret;
    int fd = open(path, O_RDONLY);
    if (fd < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to open %{private}s", __func__, path);
        return HDF_ERR_IO;
    }
    ret = read(fd, buf, size);
    if (ret < HDF_SUCCESS) {
        HDF_LOGE("%{public}s: failed to read %{private}s", __func__, path);
        close(fd);
        return HDF_ERR_IO;
    }
    close(fd);
    buf[size - 1] = '\0';
    return HDF_SUCCESS;
}

static int32_t ReadBatterySysfsToBuff(const char *path, char *buf, size_t size)
{
    int32_t ret;
    if (path == NULL) {
        HDF_LOGW("%{public}s: battery sysfs info is not exist. path=%{public}s", __func__, path);
        return HDF_ERR_INVALID_OBJECT;
    }
    ret = ReadSysfsFile(path, buf, size);
    if (ret != HDF_SUCCESS) {
        HDF_LOGW("%{public}s: read path %{private}s failed, ret: %{public}d", __func__, path, ret);
        return ret;
    }
    return HDF_SUCCESS;
}

static bool IsBatteryType(const char *typePath)
{
    char buf[MAX_BUFF_SIZE] = {0};
    int32_t ret = ReadBatterySysfsToBuff(typePath, buf, MAX_BUFF_SIZE);
    if (ret != HDF_SUCCESS) {
        return false;
    }
    TrimNewLine(buf);
    return strcmp(buf, POWER_SUPPLY_TYPE_BATTERY) == 0;
}

inline static void CapacityAssigner(const char *str, struct BatterydInfo *info)
{
    info->capacity_ = ParseInt(str); // default in percent format
}

inline static void VoltageAssigner(const char *str, struct BatterydInfo *info)
{
    info->voltage_ = ParseInt(str) / 1000; // convert to millivolt(mV) format
}

inline static void TemperatureAssigner(const char *str, struct BatterydInfo *info)
{
    info->temperature_ = ParseInt(str);
}

static int32_t HealthStateEnumConverter(const char *str)
{
    for (int i = 0; g_healthStateEnumMap[i].str; ++i) {
        if (strcmp(str, g_healthStateEnumMap[i].str) == 0) {
            return g_healthStateEnumMap[i].enumVal;
        }
    }
    return BATTERY_HEALTH_UNKNOWN;
}

inline static void HealthStateAssigner(const char *str, struct BatterydInfo *info)
{
    info->healthState_ = HealthStateEnumConverter(str);
}

static int32_t ChargeStateEnumConverter(const char *str)
{
    for (int i = 0; g_chargeStateEnumMap[i].str; ++i) {
        if (strcmp(str, g_chargeStateEnumMap[i].str) == 0) {
            return g_chargeStateEnumMap[i].enumVal;
        }
    }
    return CHARGE_STATE_RESERVED;
}

inline static void ChargeStateAssigner(const char *str, struct BatterydInfo *info)
{
    info->chargeState_ = ChargeStateEnumConverter(str);
}

inline static void PresentAssigner(const char *str, struct BatterydInfo *info)
{
    info->present_ = ParseInt(str);
}

inline static void TechnologyAssigner(const char *str, struct BatterydInfo *info)
{
    info->technology_ = str;
}

inline static void ChargeCounterAssigner(const char *str, struct BatterydInfo *info)
{
    info->chargeCounter_ = ParseInt(str);
}

static struct BatteryAssigner g_batteryAssigners[] = {
    {BATTERY_KEY_CAPACITY, sizeof(BATTERY_KEY_CAPACITY) - 1, CapacityAssigner},
    {BATTERY_KEY_VOLTAGE, sizeof(BATTERY_KEY_VOLTAGE) - 1, VoltageAssigner},
    {BATTERY_KEY_TEMPERATURE, sizeof(BATTERY_KEY_TEMPERATURE) - 1, TemperatureAssigner},
    {BATTERY_KEY_HEALTH, sizeof(BATTERY_KEY_HEALTH) - 1, HealthStateAssigner},
    {BATTERY_KEY_CHARGE_STATUS, sizeof(BATTERY_KEY_CHARGE_STATUS) - 1, ChargeStateAssigner},
    {BATTERY_KEY_PRESENT, sizeof(BATTERY_KEY_PRESENT) - 1, PresentAssigner},
    {BATTERY_KEY_TECHNOLOGY, sizeof(BATTERY_KEY_TECHNOLOGY) - 1, TechnologyAssigner},
    {BATTERY_KEY_CHARGE_COUNTER, sizeof(BATTERY_KEY_CHARGE_COUNTER) - 1, ChargeCounterAssigner},
    {NULL, 0, NULL} // end of the array
};

static void FormatPath(char *path, size_t size, const char *format, const char *basePath, const char *name)
{
    int ret = snprintf_s(path, PATH_MAX, size - 1, format, basePath, name);
    if (ret == -1) {
        HDF_LOGW("%{public}s: failed to format path of %{public}s", __func__, name);
    }
}

static void FormatSysfsPaths(struct PowerSupplySysfsInfo *info)
{
    // Format paths for plugged power supply types
    FormatPath(info->typePath, sizeof(info->typePath), "%s/%s/type", POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(info->onlinePath, sizeof(info->onlinePath), "%s/%s/online", POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(info->currentMaxPath, sizeof(info->currentMaxPath), "%s/%s/current_max", POWER_SUPPLY_BASE_PATH,
        info->name);
    FormatPath(info->voltageMaxPath, sizeof(info->voltageMaxPath), "%s/%s/voltage_max", POWER_SUPPLY_BASE_PATH,
        info->name);
    if (!IsBatteryType(info->typePath)) {
        return;
    }
    // Format paths for battery only
    g_batterySysfsInfo.name = info->name;
    FormatPath(g_batterySysfsInfo.capacityPath, sizeof(g_batterySysfsInfo.capacityPath), "%s/%s/capacity",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.voltagePath, sizeof(g_batterySysfsInfo.voltagePath), "%s/%s/voltage_now",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.temperaturePath, sizeof(g_batterySysfsInfo.temperaturePath), "%s/%s/temp",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.healthStatePath, sizeof(g_batterySysfsInfo.healthStatePath), "%s/%s/health",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.chargeStatePath, sizeof(g_batterySysfsInfo.chargeStatePath), "%s/%s/status",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.presentPath, sizeof(g_batterySysfsInfo.presentPath), "%s/%s/present",
        POWER_SUPPLY_BASE_PATH, info->name);
    FormatPath(g_batterySysfsInfo.technologyPath, sizeof(g_batterySysfsInfo.technologyPath), "%s/%s/technology",
        POWER_SUPPLY_BASE_PATH, info->name);
}

int32_t InitBatterydSysfs(void)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    int32_t index = 0;

    dir = opendir(POWER_SUPPLY_BASE_PATH);
    if (dir == NULL) {
        HDF_LOGE("%{public}s: cannot open POWER_SUPPLY_BASE_PATH", __func__);
        return HDF_ERR_IO;
    }

    while (true) {
        entry = readdir(dir);
        if (entry == NULL) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            struct PowerSupplySysfsInfo sysfsInfo = {0};
            sysfsInfo.name = entry->d_name;
            HDF_LOGD("%{public}s: init sysfs info of %{public}s", __func__, sysfsInfo.name);
            if (index >= MAX_SYSFS_SIZE) {
                HDF_LOGE("%{public}s: too many plugged types", __func__);
                break;
            }
            FormatSysfsPaths(&sysfsInfo);
            g_powerSupplySysfsInfos[index] = sysfsInfo;
            index++;
        }
    }
    closedir(dir);
    return HDF_SUCCESS;
}

int32_t ParseCapacity(int32_t *capacity)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.capacityPath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    value = ParseInt(buf);
    HDF_LOGD("%{public}s: capacity is %{public}d", __func__, value);
    *capacity = value;
    return HDF_SUCCESS;
}

int32_t ParseVoltage(int32_t *voltage)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.voltagePath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    value = ParseInt(buf);
    HDF_LOGD("%{public}s: voltage is %{public}d", __func__, value);
    *voltage = value;
    return HDF_SUCCESS;
}

int32_t ParseTemperature(int32_t *temperature)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.temperaturePath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    value = ParseInt(buf);
    HDF_LOGD("%{public}s: temperature is %{public}d", __func__, value);
    *temperature = value;
    return HDF_SUCCESS;
}

int32_t ParseHealthState(int32_t *healthState)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.healthStatePath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    TrimNewLine(buf);
    value = HealthStateEnumConverter(buf);
    HDF_LOGD("%{public}s: healthState is %{public}d", __func__, value);
    *healthState = value;
    return HDF_SUCCESS;
}

void GetPluggedTypeName(char *buf, size_t size)
{
    int32_t ret;
    int32_t online;
    for (int i = 0; i < MAX_SYSFS_SIZE && g_powerSupplySysfsInfos[i].name; ++i) {
        if (IsBatteryType(g_powerSupplySysfsInfos[i].typePath)) {
            // ignore the battery type
            continue;
        }
        ret = ReadSysfsFile(g_powerSupplySysfsInfos[i].onlinePath, buf, size);
        if (ret != HDF_SUCCESS) {
            HDF_LOGW("%{public}s: read online path failed, ret: %{public}d", __func__, ret);
            continue;
        }
        online = ParseInt(buf);
        if (!online) {
            continue;
        }
        HDF_LOGD("%{public}s: %{public}s is online", __func__, g_powerSupplySysfsInfos[i].name);
        ret = ReadSysfsFile(g_powerSupplySysfsInfos[i].typePath, buf, size);
        if (ret != HDF_SUCCESS) {
            HDF_LOGW("%{public}s: read type path failed, ret: %{public}d", __func__, ret);
            continue;
        }
        TrimNewLine(buf);
        break;
    }
}

static int32_t PluggedTypeEnumConverter(const char *str)
{
    for (int i = 0; g_pluggedTypeEnumMap[i].str; ++i) {
        if (strcmp(str, g_pluggedTypeEnumMap[i].str) == 0) {
            return g_pluggedTypeEnumMap[i].enumVal;
        }
    }
    return PLUGGED_TYPE_BUTT;
}

int32_t ParsePluggedType(int32_t *pluggedType)
{
    int32_t type;
    char buf[MAX_BUFF_SIZE] = {0};
    GetPluggedTypeName(buf, sizeof(buf));
    type = PluggedTypeEnumConverter(buf);
    if (type == PLUGGED_TYPE_BUTT) {
        HDF_LOGW("%{public}s: not support the online type %{public}s", __func__, buf);
        return HDF_ERR_NOT_SUPPORT;
    }
    HDF_LOGD("%{public}s: return online plugged type %{public}d", __func__, type);
    *pluggedType = type;
    return HDF_SUCCESS;
}

int32_t ParseChargeState(int32_t *chargeState)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.chargeStatePath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    TrimNewLine(buf);
    value = ChargeStateEnumConverter(buf);
    HDF_LOGD("%{public}s: chargeState is %{public}d", __func__, value);
    *chargeState = value;
    return HDF_SUCCESS;
}

int32_t ParsePresent(int8_t *present)
{
    int32_t ret;
    int8_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.presentPath, buf, sizeof(buf));
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    value = (int8_t)ParseInt(buf);
    HDF_LOGD("%{public}s: present is %{public}d", __func__, value);
    *present = value;
    return HDF_SUCCESS;
}

int32_t ParseTechnology(char *buf, size_t size)
{
    int32_t ret;
    ret = ReadBatterySysfsToBuff(g_batterySysfsInfo.technologyPath, buf, size);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    HDF_LOGD("%{public}s: technology is %{public}s", __func__, buf);
    return HDF_SUCCESS;
}

int32_t ParsePluggedMaxCurrent(int32_t *maxCurrent)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    GetPluggedTypeName(buf, sizeof(buf));
    for (int i = 0; i < MAX_SYSFS_SIZE && g_powerSupplySysfsInfos[i].name; ++i) {
        if (strcmp(buf, g_powerSupplySysfsInfos[i].name) != 0) {
            continue;
        }
        ret = ReadBatterySysfsToBuff(g_powerSupplySysfsInfos[i].currentMaxPath, buf, sizeof(buf));
        if (ret != HDF_SUCCESS) {
            return ret;
        }
        value = ParseInt(buf);
        HDF_LOGD("%{public}s: maxCurrent is %{public}d", __func__, value);
        *maxCurrent = value;
        return HDF_SUCCESS;
    }
    return HDF_ERR_NOT_SUPPORT;
}

int32_t ParsePluggedMaxVoltage(int32_t *maxVoltage)
{
    int32_t ret;
    int32_t value;
    char buf[MAX_BUFF_SIZE] = {0};
    GetPluggedTypeName(buf, sizeof(buf));
    for (int i = 0; i < MAX_SYSFS_SIZE && g_powerSupplySysfsInfos[i].name; ++i) {
        if (strcmp(buf, g_powerSupplySysfsInfos[i].name) != 0) {
            continue;
        }
        ret = ReadBatterySysfsToBuff(g_powerSupplySysfsInfos[i].voltageMaxPath, buf, sizeof(buf));
        if (ret != HDF_SUCCESS) {
            return ret;
        }
        value = ParseInt(buf);
        HDF_LOGD("%{public}s: maxCurrent is %{public}d", __func__, value);
        *maxVoltage = value;
        return HDF_SUCCESS;
    }
    return HDF_ERR_NOT_SUPPORT;
}

void ParseUeventToBatterydInfo(const char *msg, struct BatterydInfo *info)
{
    info->technology_ = "";
    while (*msg) {
        for (int i = 0; g_batteryAssigners[i].prefix; ++i) {
            if (!strncmp(msg, g_batteryAssigners[i].prefix, g_batteryAssigners[i].prefixLen)) {
                HDF_LOGD("%{public}s: msg: %{public}s", __func__, msg);
                msg += g_batteryAssigners[i].prefixLen;
                g_batteryAssigners[i].Assigner(msg, info);
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
}
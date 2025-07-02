/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "battery_dump.h"

#include <ctime>
#include <iosfwd>
#include <cstdio>
#include "battery_info.h"
#include "battery_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr uint32_t MS_NS = 1000000;
constexpr int32_t CAPACITY_DUMP_PARAM_SIZE = 2;
constexpr int32_t CAPACITY_LIMIT_MIN = 0;
constexpr int32_t CAPACITY_LIMIT_MAX = 100;
constexpr int32_t UEVENT_DUMP_PARAM_SIZE = 2;
}

void BatteryDump::DumpBatteryHelp(int32_t fd)
{
    dprintf(fd, "Usage:\n");
    dprintf(fd, "      -h: dump help\n");
    dprintf(fd, "      -i: dump battery info\n");
#ifndef BATTERY_USER_VERSION
    dprintf(fd, "      -u: unplug battery charging state\n");
    dprintf(fd, "      -r: reset battery state\n");
    dprintf(fd, "      --capacity <capacity>: set battery capacity, the capacity range [0, 100]\n");
    dprintf(fd, "      --uevent <uevent>: set battery uevent\n");
#endif
}

void BatteryDump::DumpCurrentTime(int32_t fd)
{
    timespec curTime = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &curTime);
    struct tm *timeinfo = localtime(&(curTime.tv_sec));
    if (timeinfo == nullptr) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "timeinfo cannot be null");
        return;
    }
    // Add 1900 to the year, add 1 to the month.
    dprintf(fd, "Current time: %04d-%02d-%02d %02d:%02d:%02d.%03d\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
            timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
            int32_t { (curTime.tv_nsec / MS_NS) });
}

void BatteryDump::DumpBatteryInfo(sptr<BatteryService> &service, int32_t fd)
{
    int32_t capacity = 0;
    service->GetCapacity(capacity);
    dprintf(fd, "capacity: %d \n", capacity);
    uint32_t batteryLevel = static_cast<uint32_t>(BatteryCapacityLevel::LEVEL_NONE);
    service->GetCapacityLevel(batteryLevel);
    dprintf(fd, "batteryLevel: %u \n", batteryLevel);
    uint32_t chargingStatus = static_cast<uint32_t>(BatteryChargeState::CHARGE_STATE_NONE);
    service->GetChargingStatus(chargingStatus);
    dprintf(fd, "chargingStatus: %u \n", chargingStatus);
    uint32_t healthState = static_cast<uint32_t>(BatteryHealthState::HEALTH_STATE_UNKNOWN);
    service->GetHealthStatus(healthState);
    dprintf(fd, "healthState: %u \n", healthState);
    uint32_t pluggedType = static_cast<uint32_t>(BatteryPluggedType::PLUGGED_TYPE_NONE);
    service->GetPluggedType(pluggedType);
    dprintf(fd, "pluggedType: %u \n", pluggedType);
    int32_t voltage = INVALID_BATT_INT_VALUE;
    service->GetVoltage(voltage);
    dprintf(fd, "voltage: %d \n", voltage);
    bool present = false;
    service->GetPresent(present);
    dprintf(fd, "present: %d \n", present);
    std::string technology;
    service->GetTechnology(technology);
    dprintf(fd, "technology: %s \n", technology.c_str());
    int32_t nowCurrent = INVALID_BATT_INT_VALUE;
    service->GetNowCurrent(nowCurrent);
    dprintf(fd, "nowCurrent: %d \n", nowCurrent);
    int32_t currentAverage = INVALID_BATT_INT_VALUE;
    service->GetCurrentAverage(currentAverage);
    dprintf(fd, "currentAverage: %d \n", currentAverage);
    int32_t totalEnergy = INVALID_BATT_INT_VALUE;
    service->GetTotalEnergy(totalEnergy);
    dprintf(fd, "totalEnergy: %d \n", totalEnergy);
    int32_t remainEnergy = INVALID_BATT_INT_VALUE;
    service->GetRemainEnergy(remainEnergy);
    dprintf(fd, "remainingEnergy: %d \n", remainEnergy);
    int64_t remainingChargeTime = INVALID_REMAINING_CHARGE_TIME_VALUE;
    service->GetRemainingChargeTime(remainingChargeTime);
    dprintf(fd, "remainingChargeTime: %ld \n", remainingChargeTime);
    int32_t temperature = INVALID_BATT_INT_VALUE;
    service->GetBatteryTemperature(temperature);
    dprintf(fd, "temperature: %d \n", temperature);
    ChargeType chargeType = service->GetChargeType();
    dprintf(fd, "chargeType: %u \n", chargeType);
}

bool BatteryDump::GetBatteryInfo(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args)
{
    if ((args.empty()) || (args[0].compare(u"-i") != 0)) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "args cannot be empty or invalid");
        return false;
    }
    DumpCurrentTime(fd);
    DumpBatteryInfo(service, fd);
    return true;
}

bool BatteryDump::MockUnplugged(int32_t fd, sptr<BatteryService>& service, const std::vector<std::u16string>& args)
{
    if ((args.empty()) || (args[0].compare(u"-u") != 0)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "args cannot be empty or invalid");
        return false;
    }
#ifndef BATTERY_USER_VERSION
    service->MockUnplugged();
    dprintf(fd, "unplugged battery charging state \n");
#else
    dprintf(fd, "[Failed] User version is not support \n");
#endif
    return true;
}

bool BatteryDump::Reset(int32_t fd, sptr<BatteryService>& service, const std::vector<std::u16string>& args)
{
    if ((args.empty()) || (args[0].compare(u"-r") != 0)) {
        BATTERY_HILOGW(FEATURE_CHARGING, "args cannot be empty or invalid");
        return false;
    }
#ifndef BATTERY_USER_VERSION
    service->Reset();
    dprintf(fd, "reset battery state \n");
#else
    dprintf(fd, "[Failed] User version is not support \n");
#endif
    return true;
}

bool BatteryDump::MockCapacity(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args)
{
    if ((args.empty()) || args.size() != CAPACITY_DUMP_PARAM_SIZE || (args[0].compare(u"--capacity") != 0)) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "args cannot be empty or invalid");
        return false;
    }
#ifndef BATTERY_USER_VERSION
    int32_t capacity = 0;
    std::string capacityStr = Str16ToStr8(args[1]);
    if (!StrToInt(capacityStr, capacity)) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "capacity convert failed");
        return false;
    }
    if (capacity < CAPACITY_LIMIT_MIN || capacity > CAPACITY_LIMIT_MAX) {
        dprintf(fd, "capacity out of range\n");
        return true;
    }
    service->MockCapacity(capacity);
    dprintf(fd, "battery capacity %d \n", capacity);
#else
    dprintf(fd, "[Failed] User version is not support \n");
#endif
    return true;
}

bool BatteryDump::MockUevent(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args)
{
    if ((args.empty()) || args.size() != UEVENT_DUMP_PARAM_SIZE || (args[0].compare(u"--uevent") != 0)) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "args cannot be empty or invalid");
        return false;
    }
#ifndef BATTERY_USER_VERSION
    std::string uevent = Str16ToStr8(args[1]);
    service->MockUevent(uevent);
    dprintf(fd, "battery uevent %s \n", uevent.c_str());
#else
    dprintf(fd, "[Failed] User version is not support \n");
#endif
    return true;
}
}  // namespace PowerMgr
}  // namespace OHOS

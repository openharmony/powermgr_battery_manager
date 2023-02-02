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
}

bool BatteryDump::DumpBatteryHelp(int32_t fd, const std::vector<std::u16string> &args)
{
    if ((args.empty()) || (args[0].compare(u"-h") != 0)) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "args cannot be empty or invalid");
        return false;
    }
    DumpHelp(fd);
    return true;
}

void BatteryDump::DumpHelp(int32_t fd)
{
    dprintf(fd, "Usage:\n");
    dprintf(fd, "      -h: dump help\n");
    dprintf(fd, "      -i: dump battery info\n");
    dprintf(fd, "      -d: show low power diaolog\n");
    dprintf(fd, "      -u: unplug battery charging state\n");
    dprintf(fd, "      -r: reset battery charging state\n");
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

bool BatteryDump::GetBatteryInfo(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args)
{
    if ((args.empty()) || (args[0].compare(u"-i") != 0)) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "args cannot be empty or invalid");
        return false;
    }
    DumpCurrentTime(fd);
    int32_t capacity = service->GetCapacity();
    dprintf(fd, "capacity: %u \n", capacity);
    BatteryCapacityLevel batteryLevel = service->GetCapacityLevel();
    dprintf(fd, "batteryLevel: %u \n", batteryLevel);
    BatteryChargeState chargingStatus = service->GetChargingStatus();
    dprintf(fd, "chargingStatus: %u \n", chargingStatus);
    BatteryHealthState healthState = service->GetHealthStatus();
    dprintf(fd, "healthState: %u \n", healthState);
    BatteryPluggedType pluggedType = service->GetPluggedType();
    dprintf(fd, "pluggedType: %u \n", pluggedType);
    int32_t voltage = service->GetVoltage();
    dprintf(fd, "voltage: %d \n", voltage);
    bool present = service->GetPresent();
    dprintf(fd, "present: %d \n", present);
    std::string technology = service->GetTechnology();
    dprintf(fd, "technology: %s \n", technology.c_str());
    int32_t nowCurrent = service->GetNowCurrent();
    dprintf(fd, "nowCurrent: %d \n", nowCurrent);
    int32_t currentAverage = service->GetCurrentAverage();
    dprintf(fd, "currentAverage: %d \n", currentAverage);
    int32_t totalEnergy = service->GetTotalEnergy();
    dprintf(fd, "totalEnergy: %d \n", totalEnergy);
    int32_t remainEnergy = service->GetRemainEnergy();
    dprintf(fd, "remainingEnergy: %d \n", remainEnergy);
    int64_t remainingChargeTime = service->GetRemainingChargeTime();
    dprintf(fd, "remainingChargeTime: %ld \n", remainingChargeTime);
    int32_t temperature = service->GetBatteryTemperature();
    dprintf(fd, "temperature: %d \n", temperature);
    return true;
}

bool BatteryDump::MockUnplugged(int32_t fd, sptr<BatteryService>& service, const std::vector<std::u16string>& args)
{
    if ((args.empty()) || (args[0].compare(u"-u") != 0)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "args cannot be empty or invalid");
        return false;
    }
    service->MockUnplugged(true);
    dprintf(fd, "unplugged battery charging state \n");
    return true;
}

bool BatteryDump::ResetPlugged(int32_t fd, sptr<BatteryService>& service, const std::vector<std::u16string>& args)
{
    if ((args.empty()) || (args[0].compare(u"-r") != 0)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "args cannot be empty or invalid");
        return false;
    }
    service->MockUnplugged(false);
    dprintf(fd, "reset battery charging state \n");
    return true;
}
}  // namespace PowerMgr
}  // namespace OHOS

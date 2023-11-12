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

#ifndef POWER_SUPPLY_PROVIDER_H
#define POWER_SUPPLY_PROVIDER_H

#include <cstdio>
#include <cstring>
#include <climits>
#include <map>
#include <vector>
#include "batteryd_api.h"
#include "v2_0/ibattery_interface.h"

namespace OHOS {
namespace PowerMgr {
class PowerSupplyProvider {
public:
    // Keep it same as the BatteryChargeState in battery_info.h
    enum BatteryChargeState {
        CHARGE_STATE_NONE = 0,
        CHARGE_STATE_ENABLE,
        CHARGE_STATE_DISABLE,
        CHARGE_STATE_FULL,
        CHARGE_STATE_RESERVED,
    };

    PowerSupplyProvider();
    virtual ~PowerSupplyProvider() = default;

    int32_t InitPowerSupplySysfs();
    void InitDefaultSysfs();
    int32_t ParseCapacity(int32_t* capacity) const;
    int32_t ParseTemperature(int32_t* temperature) const;
    int32_t ParseChargeState(int32_t* chargeState) const;
    void InitBatteryPath();

private:
    struct BatterySysfsInfo {
        char* name;
        std::string capacityPath;
        std::string temperaturePath;
        std::string chargeStatePath;
    } batterySysfsInfo_;

    static inline int32_t ParseInt(const char* str);
    static inline void Trim(char* str);
    static int32_t ChargeStateEnumConverter(const char* str);

    void TraversalNode();
    void CheckSubfolderNode(const std::string& path);
    void FormatPath(std::string& path, size_t size, const char* format, const char* basePath, const char* name) const;
    void FormatSysfsPaths();
    int32_t ReadSysfsFile(const char* path, char* buf, size_t size) const;
    int32_t ReadBatterySysfsToBuff(const char* path, char* buf, size_t size) const;
    void CreateFile(const std::string& path, const std::string& content);
    void CreateMockTechPath(std::string& mockTechPath);
    void CreateMockChargerPath(std::string& mockChargerPath);
    void CreateMockBatteryPath(std::string& mockBatteryPath);
    std::vector<std::string> nodeNames_;
    std::map<std::string, std::string> nodeNamePathMap_;
    std::string path_;
    int32_t index_;
};
}  // namespace PowerMgr
}  // namespace OHOS

#endif // POWER_SUPPLY_PROVIDER_H

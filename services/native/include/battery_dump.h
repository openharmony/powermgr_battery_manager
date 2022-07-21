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

#ifndef BATTERY_DUMP_H
#define BATTERY_DUMP_H

#include <cstdint>
#include <string>
#include <vector>
#include "refbase.h"

#include "singleton.h"
#include "battery_service.h"

namespace OHOS {
namespace PowerMgr {
class BatteryDump : public Singleton<BatteryDump> {
public:
    BatteryDump() = default;
    virtual ~BatteryDump() = default;
    bool DumpBatteryHelp(int32_t fd, const std::vector<std::u16string> &args);
    void DumpHelp(int32_t fd);
    bool GetBatteryInfo(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args);
    bool MockUnplugged(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args);
    bool ResetPlugged(int32_t fd, sptr<BatteryService> &service, const std::vector<std::u16string> &args);

private:
    void DumpCurrentTime(int32_t fd);
};
}  // namespace Sensors
}  // namespace OHOS
#endif  // BATTERY_DUMP_H

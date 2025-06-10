/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef BATTERTMGR_UTILS_BATTERY_HOOKMGR_H
#define BATTERTMGR_UTILS_BATTERY_HOOKMGR_H

#include <cstdint>
#include <hookmgr.h>
#include <modulemgr.h>
#include <string>
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
enum class BatteryHookStage : int32_t {
    BATTERY_UEVENT_CHECK = 0,
    BATTERY_PUBLISH_EVENT,
    BATTERY_HOOK_STAGE_MAX = 1000,
};

typedef struct UeventCheckInfo {
    std::string UeventName;
    bool checkResult;
} UEVENT_CHECK_INFO;

struct PublishEventContext {
    BatteryPluggedType pluggedType;
    BatteryPluggedType lastPluggedType;
    bool wirelessChargerEnable;
};

HOOK_MGR* GetBatteryHookMgr();
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERTMGR_UTILS_BATTERY_HOOKMGR_H
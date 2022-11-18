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

#ifndef BATTERY_LOG_H
#define BATTERY_LOG_H

#include "hilog/log.h"

namespace OHOS {
namespace PowerMgr {
#define FILENAME         (__builtin_strrchr("/" __FILE__, '/') + 1)
#define FORMAT(fmt, ...) "[%{public}s:%{public}d] %{public}s# " fmt, FILENAME, __LINE__, __FUNCTION__, ##__VA_ARGS__

#ifdef BATTERY_HILOGF
#undef BATTERY_HILOGF
#endif

#ifdef BATTERY_HILOGE
#undef BATTERY_HILOGE
#endif

#ifdef BATTERY_HILOGW
#undef BATTERY_HILOGW
#endif

#ifdef BATTERY_HILOGI
#undef BATTERY_HILOGI
#endif

#ifdef BATTERY_HILOGD
#undef BATTERY_HILOGD
#endif

namespace {
// Battery manager reserved domain id range
constexpr unsigned int BATTERY_DOMAIN_ID_START = 0xD002920;
constexpr unsigned int BATTERY_DOMAIN_ID_END = BATTERY_DOMAIN_ID_START + 32;
constexpr unsigned int TEST_DOMAIN_ID = 0xD000F00;
}

enum BatteryManagerLogLabel {
    // Component labels, you can add if needed
    COMP_APP = 0,
    COMP_FWK = 1,
    COMP_SVC = 2,
    COMP_HDI = 3,
    COMP_DRV = 4,
    // Feature labels, use to mark major features
    FEATURE_CHARGING,
    FEATURE_BATT_INFO,
    FEATURE_BATT_LIGHT,
    // Test label
    LABEL_TEST,
    // The end of labels, max to the domain id range length 32
    LABEL_END,
};

enum BatteryManagerLogDomain {
    DOMAIN_APP = BATTERY_DOMAIN_ID_START + COMP_APP, // 0xD002920
    DOMAIN_FRAMEWORK, // 0xD002921
    DOMAIN_SERVICE, // 0xD002922
    DOMAIN_HDI, // 0xD002923
    DOMAIN_DRIVER, // 0xD002924
    DOMAIN_FEATURE_CHARGING,
    DOMAIN_FEATURE_BATT_INFO,
    DOMAIN_FEATURE_BATT_LIGHT,
    DOMAIN_TEST = TEST_DOMAIN_ID, // 0xD000F00
    DOMAIN_END = BATTERY_DOMAIN_ID_END, // Max to 0xD002940, keep the sequence and length same as BatteryManagerLogLabel
};

// Keep the sequence and length same as BatteryManagerLogDomain
static constexpr OHOS::HiviewDFX::HiLogLabel BATTERY_LABEL[LABEL_END] = {
    {LOG_CORE, DOMAIN_APP,               "BatteryApp"},
    {LOG_CORE, DOMAIN_FRAMEWORK,         "BatteryFwk"},
    {LOG_CORE, DOMAIN_SERVICE,           "BatterySvc"},
    {LOG_CORE, DOMAIN_HDI,               "BatteryHdi"},
    {LOG_CORE, DOMAIN_DRIVER,            "BatteryDrv"},
    {LOG_CORE, DOMAIN_FEATURE_CHARGING,  "BatteryCharging"},
    {LOG_CORE, DOMAIN_FEATURE_BATT_INFO, "BatteryInfo"},
    {LOG_CORE, DOMAIN_FEATURE_BATT_LIGHT, "BatteryLight"},
    {LOG_CORE, DOMAIN_TEST,              "BatteryTest"},
};

#define BATTERY_HILOGF(domain, ...) (void)OHOS::HiviewDFX::HiLog::Fatal(BATTERY_LABEL[domain], FORMAT(__VA_ARGS__))
#define BATTERY_HILOGE(domain, ...) (void)OHOS::HiviewDFX::HiLog::Error(BATTERY_LABEL[domain], FORMAT(__VA_ARGS__))
#define BATTERY_HILOGW(domain, ...) (void)OHOS::HiviewDFX::HiLog::Warn(BATTERY_LABEL[domain], FORMAT(__VA_ARGS__))
#define BATTERY_HILOGI(domain, ...) (void)OHOS::HiviewDFX::HiLog::Info(BATTERY_LABEL[domain], FORMAT(__VA_ARGS__))
#define BATTERY_HILOGD(domain, ...) (void)OHOS::HiviewDFX::HiLog::Debug(BATTERY_LABEL[domain], FORMAT(__VA_ARGS__))
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_LOG_H

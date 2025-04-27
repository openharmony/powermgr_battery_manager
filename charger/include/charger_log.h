/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "beget_ext.h"
#include <string>

#define CHARGER_LOG_FILE  "charger.log"
#define FEATURE_CHARGING  "charger: "


#define BATTERY_HILOGE(label, fmt, ...)                                                                            \
    do {                                                                                                           \
        InitLogPrint(INIT_LOG_PATH CHARGER_LOG_FILE, INIT_ERROR, label, fmt,             \
            (STARTUP_FILE_NAME), (__LINE__), (__FUNCTION__), ##__VA_ARGS__);                                       \
    } while (0)
#define BATTERY_HILOGW(label, fmt, ...)                                                                            \
    do {                                                                                                           \
        InitLogPrint(INIT_LOG_PATH CHARGER_LOG_FILE, INIT_WARN, label, fmt,              \
            (STARTUP_FILE_NAME), (__LINE__), (__FUNCTION__), ##__VA_ARGS__);                                       \
    } while (0)
#define BATTERY_HILOGI(label, fmt, ...)                                                                            \
    do {                                                                                                           \
        InitLogPrint(INIT_LOG_PATH CHARGER_LOG_FILE, INIT_INFO, label, fmt,              \
            (STARTUP_FILE_NAME), (__LINE__), (__FUNCTION__), ##__VA_ARGS__);                                       \
    } while (0)
#define BATTERY_HILOGD(label, fmt, ...)                                                                            \
    do {                                                                                                           \
        InitLogPrint(INIT_LOG_PATH CHARGER_LOG_FILE, INIT_DEBUG, label, fmt,             \
            (STARTUP_FILE_NAME), (__LINE__), (__FUNCTION__), ##__VA_ARGS__);                                       \
    } while (0)

#endif // BATTERY_LOG_H

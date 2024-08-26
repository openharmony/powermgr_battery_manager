/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHBATTERY_INFO_HEADER
#define OHBATTERY_INFO_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static const char* COMMON_EVENT_KEY_CAPACITY = "soc";
static const char* COMMON_EVENT_KEY_CHARGE_STATE = "chargeState";
static const char* COMMON_EVENT_KEY_PLUGGED_TYPE = "pluggedType";

typedef enum {
    /**
     * Power source is unplugged.
     */
    PLUGGED_TYPE_NONE,

    /**
     * Power source is an AC charger.
     */
    PLUGGED_TYPE_AC,

    /**
     * Power source is a USB DC charger.
     */
    PLUGGED_TYPE_USB,

    /**
     * Power source is wireless charger.
     */
    PLUGGED_TYPE_WIRELESS,

    /**
     * The bottom of the enum.
     */
    PLUGGED_TYPE_BUTT
} BatteryInfo_BatteryPluggedType;

/** todo
 * Return the capacity of the battery, in percent.
 */
int32_t OH_BatteryInfo_GetCapacity();

/** todo
 * Return the charger type plugged, such as PLUGGED_TYPE_NONE,
 * PLUGGED_TYPE_AC, PLUGGED_TYPE_USB,....
 */
BatteryInfo_BatteryPluggedType OH_BatteryInfo_GetPluggedType();

#ifdef __cplusplus
}
/** @} */
#endif /* __cplusplus */

#endif /* BATTERY_INFO_NDK_TYPES_HEADER */
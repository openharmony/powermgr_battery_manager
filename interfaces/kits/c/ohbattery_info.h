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

/**
 * @addtogroup OH_BatteryInfo
 * @{
 *
 * @brief Provides the definition of the C interface for the BatteryInfo module.
 *
 * @syscap SystemCapability.PowerManager.BatteryManager.Core
 * @since 13
 * @version 1.0
 */
/**
 * @file ohbattery_info.h
 *
 * @brief Declares the APIs to get informations about the current battery capacity and the power source type,
 *        defines strings that identify corresponding common events.
 *
 * @library libohbattery_info.so
 * @kit BasicServicesKit
 * @syscap SystemCapability.PowerManager.BatteryManager.Core
 * @since 13
 * @version 1.0
 */
#ifndef OHBATTERY_INFO_HEADER
#define OHBATTERY_INFO_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief A string that identifies the common event sent after battery capacity changes.
 * @since 13
 * @version 1.0
 */
static const char* COMMON_EVENT_KEY_CAPACITY = "soc";
/**
 * @brief A string that identifies the common event sent after charge state changes.
 * @since 13
 * @version 1.0
 */
static const char* COMMON_EVENT_KEY_CHARGE_STATE = "chargeState";
/**
 * @brief A string that identifies the common event sent after plugged type changes.
 * @since 13
 * @version 1.0
 */
static const char* COMMON_EVENT_KEY_PLUGGED_TYPE = "pluggedType";

/**
 * @brief Defines plugged types.
 *
 * @since 13
 * @version 1.0
 */
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

/**
 * @brief This API returns the current battery capacity.
 *
 * @return Returns number betweem 0 and 100.
 * @syscap SystemCapability.PowerManager.BatteryManager.Core
 * @since 13
 */
int32_t OH_BatteryInfo_GetCapacity();

/**
 * @brief This API returns the current plugged type.
 *
 * @return {@link BatteryInfo_BatteryPluggedType#PLUGGED_TYPE_NONE} if the power source is unplugged.
 *         {@link PLUGGED_TYPE_AC} if the power source is an AC charger.
 *         {@link PLUGGED_TYPE_USB} if the power source is an USB DC charger.
 *         {@link PLUGGED_TYPE_WIRELESS} if the power source is wireless charger.
 *         {@link PLUGGED_TYPE_BUTT} if the type is unknown.
 * @syscap SystemCapability.PowerManager.BatteryManager.Core
 * @since 13
 */
BatteryInfo_BatteryPluggedType OH_BatteryInfo_GetPluggedType();
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* OHBATTERY_INFO_HEADER */
/** @} */

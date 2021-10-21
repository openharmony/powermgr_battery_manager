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

#ifndef BATTERYD_API_H
#define BATTERYD_API_H

#include <stdlib.h>
#include <string>

enum BatterydCmd {
    CMD_BIND_BATTERY_SUBSCRIBER = 0,
    CMD_UNBIND_BATTERY_SUBSCRIBER,
    CMD_NOTIFY_SUBSCRIBER,
    CMD_GET_CAPACITY,
    CMD_GET_VOLTAGE,
    CMD_GET_TEMPERATURE,
    CMD_GET_HEALTH_STATE,
    CMD_GET_PLUGGED_TYPE,
    CMD_GET_CHARGE_STATE,
    CMD_GET_PRESENT,
    CMD_GET_TECHNOLOGY,
    CMD_CHANGE_PATH
};

enum BatterydPluggedType {
    PLUGGED_TYPE_NONE = 0,
    PLUGGED_TYPE_AC,
    PLUGGED_TYPE_USB,
    PLUGGED_TYPE_WIRELESS,
    PLUGGED_TYPE_BUTT
};

// Keep it same as the inner kit battery_info.h
struct BatterydInfo {
    int32_t capacity_;
    int32_t voltage_;
    int32_t temperature_;
    int32_t healthState_;
    int32_t pluggedType_;
    int32_t pluggedMaxCurrent_;
    int32_t pluggedMaxVoltage_;
    int32_t chargeState_;
    int32_t chargeCounter_;
    int8_t present_;
    std::string technology_;
};

#endif // BATTERYD_API_H

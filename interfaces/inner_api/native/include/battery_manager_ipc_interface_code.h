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

#ifndef BATTERY_MANAGER_IPC_INTERFACE_DODE_H
#define BATTERY_MANAGER_IPC_INTERFACE_DODE_H

/* SAID: 3302 */
namespace OHOS {
namespace PowerMgr {
enum class BatterySrvInterfaceCode {
    BATT_GET_CAPACITY = 0,
    BATT_GET_CHARGING_STATUS,
    BATT_GET_HEALTH_STATUS,
    BATT_GET_PLUG_TYPE,
    BATT_GET_VOLTAGE,
    BATT_GET_PRESENT,
    BATT_GET_TEMPERATURE,
    BATT_GET_TECHNOLOGY,
    BATT_GET_BATTERY_LEVEL,
    BATT_GET_REMAINING_CHARGE_TIME,
    BATT_GET_BATTERY_TOTAL_ENERGY,
    BATT_GET_BATTERY_CURRENT_AVERAGE,
    BATT_GET_BATTERY_CURRENT_NOW,
    BATT_GET_BATTERY_REMAIN_ENERGY,
    SET_BATTERY_CONFIG,
    GET_BATTERY_CONFIG,
    SUPPORT_BATTERY_CONFIG
};
} // space PowerMgr
} // namespace OHOS

#endif // BATTERY_MANAGER_IPC_INTERFACE_DODE_H

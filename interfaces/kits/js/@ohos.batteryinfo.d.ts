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

/**
 * Obtains battery information of a device.
 *
 * <p>Battery information includes the remaining battery power,
 * voltage, temperature, model, and charger type.
 *
 * @SysCap SystemCapability.PowerMgr.BatteryManager
 * @devices phone, tablet
 * @since 6
 */
declare namespace batteryInfo {
    /**
     * Battery state of charge (SoC) of the current device.
     * @devices phone, tablet
     */
    const batterySOC: number;

    /**
     * Battery charging status of the current device.
     * @devices phone, tablet
     */
    const chargingStatus: BatteryChargeState;

    /**
     * Battery health state of the current device.
     * @devices phone, tablet
     */
    const healthStatus: BatteryHealthState;

    /**
     * Charger type of the current device.
     * @devices phone, tablet
     */
    const pluggedType: BatteryPluggedType;

    /**
     * Battery voltage of the current device.
     * @devices phone, tablet
     */
    const voltage: number;

    /**
     * Battery technology of the current device.
     * @devices phone, tablet
     */
    const technology: string;

    /**
     * Battery temperature of the current device.
     * @devices phone, tablet
     */
    const batteryTemperature: number;

    /**
     * Indicates the charger type of a device.
     *
     * @SysCap SystemCapability.PowerMgr.BatteryManager
     * @devices phone, tablet
     * @since 6
     */
    export enum BatteryPluggedType {
        /**
         * Unknown type
         * @devices phone, tablet
         */
        NONE,
        /**
         * AC charger
         * @devices phone, tablet
         */
        AC,
        /**
         * USB charger
         * @devices phone, tablet
         */
        USB,
        /**
         * Wireless charger
         * @devices phone, tablet
         */
        WIRELESS
    }

    /**
     * Indicates the battery charging status of a device.
     *
     * @SysCap SystemCapability.PowerMgr.BatteryManager
     * @devices phone, tablet
     * @since 6
     */
    export enum BatteryChargeState {
        /**
         * Unknown state.
         * @devices phone, tablet
         */
        NONE,
        /**
         * The battery is being charged.
         * @devices phone, tablet
         */
        ENABLE,
        /**
         * The battery is not being charged.
         * @devices phone, tablet
         */
        DISABLE,
        /**
         * The battery is fully charged.
         * @devices phone, tablet
         */
        FULL
    }

    /**
     * Indicates the battery health status of a device.
     *
     * @SysCap SystemCapability.PowerMgr.BatteryManager
     * @devices phone, tablet
     * @since 6
     */
    export enum BatteryHealthState {
        /**
         * Unknown state.
         * @devices phone, tablet
         */
        UNKNOWN,
        /**
         * The battery is in healthy state.
         * @devices phone, tablet
         */
        GOOD,
        /**
         * The battery is overheated.
         * @devices phone, tablet
         */
        OVERHEAT,
        /**
         * The battery voltage is over high.
         * @devices phone, tablet
         */
        OVERVOLTAGE,
        /**
         * The battery temperature is low.
         * @devices phone, tablet
         */
        COLD,
        /**
         * The battery is dead.
         * @devices phone, tablet
         */
        DEAD
    }
}
export default batteryInfo;


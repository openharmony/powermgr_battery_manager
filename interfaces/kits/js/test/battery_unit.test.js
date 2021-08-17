/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import app from '@system.app'
import Context from '@ohos.napi_context'
import batteryInfo from '@ohos.batteryInfo';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('appInfoTest', function () {
    console.log("*************Battery Unit Test Begin*************");
    it('battery_soc_test', 0, function () {
        var batterySoc = batteryInfo.batterySOC;
        console.info('batterySoc = ' + batterySoc);
        expect(batterySoc >= 0 && batterySoc <= 100).assertEqual('1')
    })
    it('charging_status_test', 0, function () {
        var chargingStatus = batteryInfo.chargingStatus;
        console.info('chargingStatus = ' + chargingStatus);
        expect(chargingStatus >= 0 && chargingStatus <= 3).assertEqual('1')
    })
    it('health_status_test', 0, function () {
        var healthStatus = batteryInfo.healthStatus;
        console.info('healthStatus = ' + healthStatus);
        expect(healthStatus >= 0 && healthStatus <= 5).assertEqual('1')
    })
    it('plugged_type_test', 0, function () {
        var pluggedType = batteryInfo.pluggedType;
        console.info('pluggedType = ' + pluggedType);
        expect(pluggedType >= 0 && pluggedType <= 3).assertEqual('1')
    })
    it('voltage_test', 0, function () {
        var voltage = batteryInfo.voltage;
        console.info('voltage = ' + voltage);
        expect(voltage >= 0).assertEqual('1')
    })
    it('technology_test', 0, function () {
        var technology = batteryInfo.technology;
        console.info('technology = ' + technology);
        expect(0).assertEqual('0')
    })
    it('battery_temperature_test', 0, function () {
        var batteryTemperature = batteryInfo.batteryTemperature;
        console.info('batteryTemperature = ' + batteryTemperature);
        expect(batteryTemperature <= 700).assertEqual('1')
    })

    it('is_battery_present_test', 0, function () {
        var isBatteryPresent = batteryInfo.isBatteryPresent;
        console.info('isBatteryPresent = ' + isBatteryPresent);
        expect(0).assertEqual('0')
    })

    it('enum_health_state_test_unknown', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.UNKNOWN;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 0).assertEqual('1')
    })
    it('enum_health_state_test_good', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.GOOD;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 1).assertEqual('1')
    })
    it('enum_health_state_test_overheat', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.OVERHEAT;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 2).assertEqual('1')
    })
    it('enum_health_state_test_overvoltage', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.OVERVOLTAGE;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 3).assertEqual('1')
    })
    it('enum_health_state_test_cold', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.COLD;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 4).assertEqual('1')
    })
    it('enum_health_state_test_dead', 0, function () {
        var batteryHealthState = batteryInfo.BatteryHealthState.DEAD;
        console.info('batteryHealthState = ' + batteryHealthState);
        expect(batteryHealthState == 5).assertEqual('1')
    })

    it('enum_charge_state_test_none', 0, function () {
        var batteryChargeState = batteryInfo.BatteryChargeState.NONE;
        console.info('batteryChargeState = ' + batteryChargeState);
        expect(batteryChargeState == 0).assertEqual('1')
    })
    it('enum_charge_state_test_enable', 0, function () {
        var batteryChargeState = batteryInfo.BatteryChargeState.ENABLE;
        console.info('batteryChargeState = ' + batteryChargeState);
        expect(batteryChargeState == 1).assertEqual('1')
    })
    it('enum_charge_state_test_disable', 0, function () {
        var batteryChargeState = batteryInfo.BatteryChargeState.DISABLE;
        console.info('batteryChargeState = ' + batteryChargeState);
        expect(batteryChargeState == 2).assertEqual('1')
    })
    it('enum_charge_state_test_full', 0, function () {
        var batteryChargeState = batteryInfo.BatteryChargeState.FULL;
        console.info('batteryChargeState = ' + batteryChargeState);
        expect(batteryChargeState == 3).assertEqual('1')
    })

    it('enum_plugged_type_test_none', 0, function () {
        var batteryPluggedType = batteryInfo.BatteryPluggedType.NONE;
        console.info('batteryPluggedType = ' + batteryPluggedType);
        expect(batteryPluggedType == 0).assertEqual('1')
    })
    it('enum_plugged_type_test_ac', 0, function () {
        var batteryPluggedType = batteryInfo.BatteryPluggedType.AC;
        console.info('batteryPluggedType = ' + batteryPluggedType);
        expect(batteryPluggedType == 1).assertEqual('1')
    })
    it('enum_plugged_type_test_usb', 0, function () {
        var batteryPluggedType = batteryInfo.BatteryPluggedType.USB;
        console.info('batteryPluggedType = ' + batteryPluggedType);
        expect(batteryPluggedType == 2).assertEqual('1')
    })
    it('enum_plugged_type_test_wireless', 0, function () {
        var batteryPluggedType = batteryInfo.BatteryPluggedType.WIRELESS;
        console.info('batteryPluggedType = ' + batteryPluggedType);
        expect(batteryPluggedType == 3).assertEqual('1')
    })
})

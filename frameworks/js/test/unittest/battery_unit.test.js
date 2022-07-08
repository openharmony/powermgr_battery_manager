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

import batteryInfo from '@ohos.batteryinfo';

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('appInfoTest', function () {
    console.log("*************Battery Unit Test Begin*************");

    it('battery_soc_test', 0, function () {
      let batterySoc = batteryInfo.batterySOC;
      console.info('batterySoc = ' + batterySoc);
      expect(batterySoc >= -1 && batterySoc <= 100).assertTrue()
    })

    it('charging_status_test', 0, function () {
      let chargingStatus = batteryInfo.chargingStatus;
      console.info('chargingStatus = ' + chargingStatus);
      expect(chargingStatus >= 0 && chargingStatus <= 4).assertTrue()
    })

    it('health_status_test', 0, function () {
      let healthStatus = batteryInfo.healthStatus;
      console.info('healthStatus = ' + healthStatus);
      expect(healthStatus >= 0 && healthStatus <= 6).assertTrue()
    })

    it('plugged_type_test', 0, function () {
      let pluggedType = batteryInfo.pluggedType;
      console.info('pluggedType = ' + pluggedType);
      expect(pluggedType >= 0 && pluggedType <= 4).assertTrue()
    })

    it('voltage_test', 0, function () {
      let voltage = batteryInfo.voltage;
      console.info('voltage = ' + voltage);
      expect(voltage >= -1).assertTrue()
    })

    it('technology_test', 0, function () {
      let technology = batteryInfo.technology;
      console.info('technology = ' + technology);
      expect(technology !== null).assertTrue()
    })

    it('battery_temperature_test', 0, function () {
      let batteryTemperature = batteryInfo.batteryTemperature;
      console.info('batteryTemperature = ' + batteryTemperature);
      expect(batteryTemperature <= 700).assertTrue()
    })

    it('enum_health_state_test_unknown', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.UNKNOWN;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 0).assertTrue()
    })

    it('enum_health_state_test_good', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.GOOD;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 1).assertTrue()
    })

    it('enum_health_state_test_overheat', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.OVERHEAT;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 2).assertTrue()
    })

    it('enum_health_state_test_overvoltage', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.OVERVOLTAGE;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 3).assertTrue()
    })

    it('enum_health_state_test_cold', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.COLD;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 4).assertTrue()
    })

    it('enum_health_state_test_dead', 0, function () {
      let batteryHealthState = batteryInfo.BatteryHealthState.DEAD;
      console.info('batteryHealthState = ' + batteryHealthState);
      expect(batteryHealthState == 5).assertTrue()
    })

    it('enum_charge_state_test_none', 0, function () {
      let batteryChargeState = batteryInfo.BatteryChargeState.NONE;
      console.info('batteryChargeState = ' + batteryChargeState);
      expect(batteryChargeState == 0).assertTrue()
    })

    it('enum_charge_state_test_enable', 0, function () {
      let batteryChargeState = batteryInfo.BatteryChargeState.ENABLE;
      console.info('batteryChargeState = ' + batteryChargeState);
      expect(batteryChargeState == 1).assertTrue()
    })

    it('enum_charge_state_test_disable', 0, function () {
      let batteryChargeState = batteryInfo.BatteryChargeState.DISABLE;
      console.info('batteryChargeState = ' + batteryChargeState);
      expect(batteryChargeState == 2).assertTrue()
    })

    it('enum_charge_state_test_full', 0, function () {
      let batteryChargeState = batteryInfo.BatteryChargeState.FULL;
      console.info('batteryChargeState = ' + batteryChargeState);
      expect(batteryChargeState == 3).assertTrue()
    })

    it('enum_plugged_type_test_none', 0, function () {
      let batteryPluggedType = batteryInfo.BatteryPluggedType.NONE;
      console.info('batteryPluggedType = ' + batteryPluggedType);
      expect(batteryPluggedType == 0).assertTrue()
    })

    it('enum_plugged_type_test_ac', 0, function () {
      let batteryPluggedType = batteryInfo.BatteryPluggedType.AC;
      console.info('batteryPluggedType = ' + batteryPluggedType);
      expect(batteryPluggedType == 1).assertTrue()
    })

    it('enum_plugged_type_test_usb', 0, function () {
      let batteryPluggedType = batteryInfo.BatteryPluggedType.USB;
      console.info('batteryPluggedType = ' + batteryPluggedType);
      expect(batteryPluggedType == 2).assertTrue()
    })

    it('enum_plugged_type_test_wireless', 0, function () {
      let batteryPluggedType = batteryInfo.BatteryPluggedType.WIRELESS;
      console.info('batteryPluggedType = ' + batteryPluggedType);
      expect(batteryPluggedType == 3).assertTrue()
    })

    it('is_battery_present_test', 0, function () {
      let isBatteryPresent = batteryInfo.isBatteryPresent;
      console.info('isBatteryPresent = ' + isBatteryPresent);
      if (isBatteryPresent) {
        expect(isBatteryPresent).assertTrue();
      } else {
        expect(isBatteryPresent).assertFalse();
      }
    })

    it('nowCurrent_test', 0, function () {
      let nowCurrent = batteryInfo.nowCurrent;
      console.info('nowCurrent = ' + nowCurrent);
      expect(nowCurrent >= 0).assertTrue();
    })

    it('remainingEnergy_test', 0, function () {
      let remainingEnergy = batteryInfo.remainingEnergy;
      console.info('remEnergy = ' + remainingEnergy);
      expect(remainingEnergy >= 0).assertTrue();
    })

    it('totalEnergy_test', 0, function () {
      let totalEnergy = batteryInfo.totalEnergy;
      console.info('totalenergy = ' + totalEnergy);
      expect(totalEnergy >= 0).assertTrue();
    })

    it('remainingChargeTime_test', 0, function () {
      let remainingChargeTime = batteryInfo.estimatedRemainingChargeTime;
      console.info('remainingChargeTime = ' + remainingChargeTime);
      expect(remainingChargeTime >= 0).assertTrue();
    })

    it('batterylevel_test', 0, function () {
      let batteryLevel = batteryInfo.batteryCapacityLevel;
      console.info('batteryLevel = ' + batteryLevel);
      expect(batteryLevel >= batteryInfo.BatteryCapacityLevel.LEVEL_NONE &&
        batteryLevel <= batteryInfo.BatteryCapacityLevel.LEVEL_CRITICAL).assertTrue();
    })

    it('enum_level_state_test_none', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_NONE;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 0).assertTrue();
    })

    it('enum_level_state_test_full', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_FULL;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 1).assertTrue();
    })

    it('enum_level_state_test_high', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_HIGH;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 2).assertTrue();
    })

    it('enum_level_state_test_normal', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_NORMAL;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 3).assertTrue();
    })

    it('enum_level_state_test_low', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_LOW;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 4).assertTrue();
    })

    it('enum_level_state_test_emergency', 0, function () {
      let batteryLevelState = batteryInfo.BatteryCapacityLevel.LEVEL_CRITICAL;
      console.info('batteryLevelState = ' + batteryLevelState);
      expect(batteryLevelState == 5).assertTrue();
    })
    
    it('CommonEventBatteryChangedCode_EXTRA_SOC_test', 0, function () {
      console.info('CommonEventBatteryChangedCode_EXTRA_SOC = '
          + batteryInfo.CommonEventBatteryChangedCode.EXTRA_SOC);
      expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_SOC == 0).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_VOLTAGE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_VOLTAGE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_VOLTAGE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_VOLTAGE == 1).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_TEMPERATURE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_TEMPERATURE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_TEMPERATURE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_TEMPERATURE == 2).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_HEALTH_STATE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_HEALTH_STATE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_HEALTH_STATE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_HEALTH_STATE == 3).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_PLUGGED_TYPE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_PLUGGED_TYPE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_PLUGGED_TYPE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_PLUGGED_TYPE == 4).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_MAX_CURRENT_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_MAX_CURRENT = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_MAX_CURRENT);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_MAX_CURRENT == 5).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_MAX_VOLTAGE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_MAX_VOLTAGE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_MAX_VOLTAGE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_MAX_VOLTAGE == 6).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_CHARGE_STATE_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_CHARGE_STATE = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_CHARGE_STATE);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_CHARGE_STATE == 7).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_CHARGE_COUNTER_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_CHARGE_COUNTER = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_CHARGE_COUNTER);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_CHARGE_COUNTER == 8).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_PRESENT_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_PRESENT = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_PRESENT);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_PRESENT == 9).assertTrue();
    })

    it('CommonEventBatteryChangedCode_EXTRA_TECHNOLOGY_test', 0, function () {
        console.info('CommonEventBatteryChangedCode_EXTRA_TECHNOLOGY = '
            + batteryInfo.CommonEventBatteryChangedCode.EXTRA_TECHNOLOGY);
        expect(batteryInfo.CommonEventBatteryChangedCode.EXTRA_TECHNOLOGY == 10).assertTrue();
    })
  })

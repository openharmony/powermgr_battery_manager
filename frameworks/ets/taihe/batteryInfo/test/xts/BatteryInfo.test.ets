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

import { describe, it, expect, TestType, Size, Level } from "../hypium/index";
import { BusinessError } from '@ohos.base'
import hilog from '@ohos.hilog'
import Utils from './Util.test';
import batteryInfo from '@ohos.batteryInfo'

let domain: number = 0x0000;
let tag: string = 'testTag';
const ERROR_CODE = 202;

export default function BatteryInfoTest() {

  describe("BatteryInfoTest", (): void => {
    hilog.info(domain, tag, '%{public}s', 'describe start');
    it("BatteryInfoTest0100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0100 start');
        try {
          let soc = batteryInfo.batterySOC();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(soc));
          hilog.info(domain, tag, '%{public}s', typeof soc);
          expect(new Number(soc)).assertLargerOrEqual(new Number(0));
          expect(new Number(soc)).assertLessOrEqual(new Number(100));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0200 start');
        try {
          let chargingStatus = batteryInfo.chargingStatus();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargingStatus));
          hilog.info(domain, tag, '%{public}s', typeof chargingStatus);
          expect(new Number(chargingStatus)).assertLargerOrEqual(new Number(batteryInfo.BatteryChargeState.NONE));
          expect(new Number(chargingStatus)).assertLessOrEqual(new Number(batteryInfo.BatteryChargeState.FULL));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0300 start');
        try {
          let healthStatus = batteryInfo.healthStatus();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(healthStatus));
          hilog.info(domain, tag, '%{public}s', typeof healthStatus);
          expect(new Number(healthStatus)).assertLargerOrEqual(new Number(batteryInfo.BatteryHealthState.UNKNOWN));
          expect(new Number(healthStatus)).assertLessOrEqual(new Number(batteryInfo.BatteryHealthState.DEAD));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0400 start');
        try {
          let pluggedType = batteryInfo.pluggedType();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(pluggedType));
          hilog.info(domain, tag, '%{public}s', typeof pluggedType);
          expect(new Number(pluggedType)).assertLargerOrEqual(new Number(batteryInfo.BatteryPluggedType.NONE));
          expect(new Number(pluggedType)).assertLessOrEqual(new Number(batteryInfo.BatteryPluggedType.WIRELESS));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0500 start');
        try {
          let voltage = batteryInfo.voltage();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(voltage));
          hilog.info(domain, tag, '%{public}s', typeof voltage);
          expect(new Number(voltage)).assertLargerOrEqual(new Number(-1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0600 start');
        try {
          let technology = batteryInfo.technology();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(technology));
          hilog.info(domain, tag, '%{public}s', typeof technology);
          expect(typeof technology).assertEqual('string');
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0700 start');
        try {
          let batteryTemperature = batteryInfo.batteryTemperature();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryTemperature));
          hilog.info(domain, tag, '%{public}s', typeof batteryTemperature);
          expect(new Number(batteryTemperature)).assertLessOrEqual(new Number(700));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0800", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0800 start');
        try {
          let isBatteryPresent = batteryInfo.isBatteryPresent();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(isBatteryPresent));
          hilog.info(domain, tag, '%{public}s', typeof isBatteryPresent);
          expect(typeof isBatteryPresent).assertEqual('boolean');
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest0900", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest0900 start');
        try {
          let batteryLevel = batteryInfo.batteryCapacityLevel();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryLevel));
          hilog.info(domain, tag, '%{public}s', typeof batteryLevel);
          expect(new Number(batteryLevel)).assertLargerOrEqual(new Number(batteryInfo.BatteryCapacityLevel.LEVEL_FULL));
          expect(new Number(batteryLevel)).assertLessOrEqual(new Number(batteryInfo.BatteryCapacityLevel.LEVEL_SHUTDOWN));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1000", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1000 start');
        try {
          let estimatedRemainingChargeTimeInfo = batteryInfo.estimatedRemainingChargeTime();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(estimatedRemainingChargeTimeInfo));
          hilog.info(domain, tag, '%{public}s', typeof estimatedRemainingChargeTimeInfo);
          expect(new Number(estimatedRemainingChargeTimeInfo)).assertEqual(new Number(-1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1100 start');
        try {
          let totalEnergyInfo = batteryInfo.totalEnergy();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(totalEnergyInfo));
          hilog.info(domain, tag, '%{public}s', typeof totalEnergyInfo);
          expect(new Number(totalEnergyInfo)).assertEqual(new Number(-1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1200 start');
        try {
          let nowCurrent = batteryInfo.nowCurrent();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(nowCurrent));
          hilog.info(domain, tag, '%{public}s', typeof nowCurrent);
          expect(new Number(nowCurrent)).assertLargerOrEqual(new Number(-10000));
          expect(new Number(nowCurrent)).assertLessOrEqual(new Number(10000));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1300 start');
        try {
          let remainingEnergyInfo = batteryInfo.remainingEnergy();
          hilog.info(domain, tag, '%{public}s', JSON.stringify(remainingEnergyInfo));
          hilog.info(domain, tag, '%{public}s', typeof remainingEnergyInfo);
          expect(new Number(remainingEnergyInfo)).assertEqual(new Number(-1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1400 start');
        try {
          let batteryPluggedTypeNone = batteryInfo.BatteryPluggedType.NONE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryPluggedTypeNone));
          hilog.info(domain, tag, '%{public}s', typeof batteryPluggedTypeNone);
          expect(new Number(batteryPluggedTypeNone)).assertEqual(new Number(0));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1500 start');
        try {
          let batteryPluggedTypeAc = batteryInfo.BatteryPluggedType.AC;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryPluggedTypeAc));
          hilog.info(domain, tag, '%{public}s', typeof batteryPluggedTypeAc);
          expect(new Number(batteryPluggedTypeAc)).assertEqual(new Number(1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1600 start');
        try {
          let batteryPluggedTypeUsb = batteryInfo.BatteryPluggedType.USB;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryPluggedTypeUsb));
          hilog.info(domain, tag, '%{public}s', typeof batteryPluggedTypeUsb);
          expect(new Number(batteryPluggedTypeUsb)).assertEqual(new Number(2));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1700 start');
        try {
          let batteryPluggedTypeWireless = batteryInfo.BatteryPluggedType.WIRELESS;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryPluggedTypeWireless));
          hilog.info(domain, tag, '%{public}s', typeof batteryPluggedTypeWireless);
          expect(new Number(batteryPluggedTypeWireless)).assertEqual(new Number(3));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1800", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1800 start');
        try {
          let batteryChargeStateNone = batteryInfo.BatteryChargeState.NONE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryChargeStateNone));
          hilog.info(domain, tag, '%{public}s', typeof batteryChargeStateNone);
          expect(new Number(batteryChargeStateNone)).assertEqual(new Number(0));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest1900", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest1900 start');
        try {
          let batteryChargeStateEnable = batteryInfo.BatteryChargeState.ENABLE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryChargeStateEnable));
          hilog.info(domain, tag, '%{public}s', typeof batteryChargeStateEnable);
          expect(new Number(batteryChargeStateEnable)).assertEqual(new Number(1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2000", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2000 start');
        try {
          let batteryChargeStateDisable = batteryInfo.BatteryChargeState.DISABLE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryChargeStateDisable));
          hilog.info(domain, tag, '%{public}s', typeof batteryChargeStateDisable);
          expect(new Number(batteryChargeStateDisable)).assertEqual(new Number(2));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2100 start');
        try {
          let batteryChargeStateFull = batteryInfo.BatteryChargeState.FULL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryChargeStateFull));
          hilog.info(domain, tag, '%{public}s', typeof batteryChargeStateFull);
          expect(new Number(batteryChargeStateFull)).assertEqual(new Number(3));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2200 start');
        try {
          let batteryHealthStateUnknown = batteryInfo.BatteryHealthState.UNKNOWN;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateUnknown));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateUnknown);
          expect(new Number(batteryHealthStateUnknown)).assertEqual(new Number(0));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2300 start');
        try {
          let batteryHealthStateGood = batteryInfo.BatteryHealthState.GOOD;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateGood));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateGood);
          expect(new Number(batteryHealthStateGood)).assertEqual(new Number(1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2400 start');
        try {
          let batteryHealthStateOverheat = batteryInfo.BatteryHealthState.OVERHEAT;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateOverheat));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateOverheat);
          expect(new Number(batteryHealthStateOverheat)).assertEqual(new Number(2));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2500 start');
        try {
          let batteryHealthStateOvervoltage = batteryInfo.BatteryHealthState.OVERVOLTAGE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateOvervoltage));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateOvervoltage);
          expect(new Number(batteryHealthStateOvervoltage)).assertEqual(new Number(3));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2600 start');
        try {
          let batteryHealthStateCold = batteryInfo.BatteryHealthState.COLD;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateCold));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateCold);
          expect(new Number(batteryHealthStateCold)).assertEqual(new Number(4));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2700 start');
        try {
          let batteryHealthStateDead = batteryInfo.BatteryHealthState.DEAD;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryHealthStateDead));
          hilog.info(domain, tag, '%{public}s', typeof batteryHealthStateDead);
          expect(new Number(batteryHealthStateDead)).assertEqual(new Number(5));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2800", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2800 start');
        try {
          let batteryCapacityLevelFull = batteryInfo.BatteryCapacityLevel.LEVEL_FULL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelFull));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelFull);
          expect(new Number(batteryCapacityLevelFull)).assertEqual(new Number(0));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest2900", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest2900 start');
        try {
          let batteryCapacityLevelHigh = batteryInfo.BatteryCapacityLevel.LEVEL_HIGH;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelHigh));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelHigh);
          expect(new Number(batteryCapacityLevelHigh)).assertEqual(new Number(1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3000", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3000 start');
        try {
          let batteryCapacityLevelNormal = batteryInfo.BatteryCapacityLevel.LEVEL_NORMAL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelNormal));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelNormal);
          expect(new Number(batteryCapacityLevelNormal)).assertEqual(new Number(2));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3100 start');
        try {
          let batteryCapacityLevelLow = batteryInfo.BatteryCapacityLevel.LEVEL_LOW;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelLow));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelLow);
          expect(new Number(batteryCapacityLevelLow)).assertEqual(new Number(3));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3200 start');
        try {
          let batteryCapacityLevelWarning = batteryInfo.BatteryCapacityLevel.LEVEL_WARNING;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelWarning));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelWarning);
          expect(new Number(batteryCapacityLevelWarning)).assertEqual(new Number(4));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3300 start');
        try {
          let batteryCapacityLevelCritical = batteryInfo.BatteryCapacityLevel.LEVEL_CRITICAL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelCritical));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelCritical);
          expect(new Number(batteryCapacityLevelCritical)).assertEqual(new Number(5));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3400 start');
        try {
          let batteryCapacityLevelShutdown = batteryInfo.BatteryCapacityLevel.LEVEL_SHUTDOWN;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(batteryCapacityLevelShutdown));
          hilog.info(domain, tag, '%{public}s', typeof batteryCapacityLevelShutdown);
          expect(new Number(batteryCapacityLevelShutdown)).assertEqual(new Number(6));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3500 start');
        try {
          let commonEventBatteryChangedKeySoc = batteryInfo.CommonEventBatteryChangedKey.EXTRA_SOC;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeySoc));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeySoc);
          expect(new String(commonEventBatteryChangedKeySoc)).assertEqual(new String('soc'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3600 start');
        try {
          let commonEventBatteryChangedKeyChargeState = batteryInfo.CommonEventBatteryChangedKey.EXTRA_CHARGE_STATE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyChargeState));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyChargeState);
          expect(new String(commonEventBatteryChangedKeyChargeState)).assertEqual(new String('chargeState'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3700 start');
        try {
          let commonEventBatteryChangedKeyHealthState = batteryInfo.CommonEventBatteryChangedKey.EXTRA_HEALTH_STATE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyHealthState));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyHealthState);
          expect(new String(commonEventBatteryChangedKeyHealthState)).assertEqual(new String('healthState'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3800", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3800 start');
        try {
          let commonEventBatteryChangedKeyPluggedType = batteryInfo.CommonEventBatteryChangedKey.EXTRA_PLUGGED_TYPE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyPluggedType));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyPluggedType);
          expect(new String(commonEventBatteryChangedKeyPluggedType)).assertEqual(new String('pluggedType'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest3900", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest3900 start');
        try {
          let commonEventBatteryChangedKeyVoltage = batteryInfo.CommonEventBatteryChangedKey.EXTRA_VOLTAGE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyVoltage));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyVoltage);
          expect(new String(commonEventBatteryChangedKeyVoltage)).assertEqual(new String('voltage'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest4000", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4000 start');
        try {
          let commonEventBatteryChangedKeyTechnology = batteryInfo.CommonEventBatteryChangedKey.EXTRA_TECHNOLOGY;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyTechnology));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyTechnology);
          expect(new String(commonEventBatteryChangedKeyTechnology)).assertEqual(new String('technology'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest4100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4100 start');
        try {
          let commonEventBatteryChangedKeyTemperature = batteryInfo.CommonEventBatteryChangedKey.EXTRA_TEMPERATURE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyTemperature));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyTemperature);
          expect(new String(commonEventBatteryChangedKeyTemperature)).assertEqual(new String('temperature'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest4200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4200 start');
        try {
          let commonEventBatteryChangedKeyPresent = batteryInfo.CommonEventBatteryChangedKey.EXTRA_PRESENT;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyPresent));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyPresent);
          expect(new String(commonEventBatteryChangedKeyPresent)).assertEqual(new String('present'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest4300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4300 start');
        try {
          let commonEventBatteryChangedKeyCapacityLevel = batteryInfo.CommonEventBatteryChangedKey.EXTRA_CAPACITY_LEVEL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(commonEventBatteryChangedKeyCapacityLevel));
          hilog.info(domain, tag, '%{public}s', typeof commonEventBatteryChangedKeyCapacityLevel);
          expect(new String(commonEventBatteryChangedKeyCapacityLevel)).assertEqual(new String('capacityLevel'));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("BatteryInfoTest4400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4400 start');
        try {
          let sceneName = 'batteryInfo';
          let sceneValue = '1';
          let result = batteryInfo.setBatteryConfig(sceneName, sceneValue);
          hilog.info(domain, tag, '%{public}s', JSON.stringify(result));
          hilog.info(domain, tag, '%{public}s', typeof result);
          expect(typeof result).assertEqual('number');
          done();
        } catch (e : BusinessError) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          expect(e.code).assertEqual(ERROR_CODE);
          done();
        }
      })
    it("BatteryInfoTest4500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4500 start');
        try {
          let sceneName = 'batteryInfo';
          let result = batteryInfo.getBatteryConfig(sceneName);
          hilog.info(domain, tag, '%{public}s', JSON.stringify(result));
          hilog.info(domain, tag, '%{public}s', typeof result);
          expect(typeof result).assertEqual('string');
          done();
        } catch (e : BusinessError) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          expect(e.code).assertEqual(ERROR_CODE);
          done();
        }
      })
    it("BatteryInfoTest4600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest4600 start');
        try {
          let sceneName = 'batteryInfo';
          let result = batteryInfo.isBatteryConfigSupported(sceneName);
          hilog.info(domain, tag, '%{public}s', JSON.stringify(result));
          hilog.info(domain, tag, '%{public}s', typeof result);
          expect(typeof result).assertEqual('boolean');
          done();
        } catch (e : BusinessError) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          expect(e.code).assertEqual(ERROR_CODE);
          done();
        }
      })
  })
  hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest end');
}

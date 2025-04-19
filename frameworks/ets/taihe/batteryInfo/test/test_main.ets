/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

import { batteryInfo } from "./@ohos.batteryInfo";

function main() {
    try {
        let num = batteryInfo.setBatteryConfig("wireless_tx", "1");
        console.info('setBatteryConfig done, return val:', num);
        let str = batteryInfo.getBatteryConfig("wireless_tx");
        console.info('getBatteryConfig done, return val:', str);
        let result = batteryInfo.isBatteryConfigSupported("wireless_tx");
        console.info('isBatteryConfigSupported done, return val:', result);
        console.info('-----------------------------------');

        console.info('batterySOC() done' + batteryInfo.batterySOC());
        console.info('chargingStatus() done' + batteryInfo.chargingStatus());
        console.info('healthStatus() done' + batteryInfo.healthStatus());
        console.info('pluggedType() done' + batteryInfo.pluggedType());
        console.info('voltage() done' + batteryInfo.voltage());
        console.info('technology() done' + batteryInfo.technology());
        console.info('batteryTemperature() done' + batteryInfo.batteryTemperature());
        console.info('isBatteryPresent() done' + batteryInfo.isBatteryPresent());
        console.info('batteryCapacityLevel() done' + batteryInfo.batteryCapacityLevel());
        console.info('estimatedRemainingChargeTime() done' + batteryInfo.estimatedRemainingChargeTime());
        console.info('totalEnergy() done' + batteryInfo.totalEnergy());
        console.info('nowCurrent() done' + batteryInfo.nowCurrent());
        console.info('remainingEnergy() done' + batteryInfo.remainingEnergy());
        console.info('-----------------------------------');

        console.info('BatteryPluggedType() done:'+ batteryInfo.BatteryPluggedType.NONE);
        console.info('BatteryPluggedType() done:'+ batteryInfo.BatteryPluggedType.AC);
        console.info('BatteryPluggedType() done:'+ batteryInfo.BatteryPluggedType.USB);
        console.info('BatteryPluggedType() done:'+ batteryInfo.BatteryPluggedType.WIRELESS);
        console.info('-----------------------------------');

        console.info('BatteryChargeState() done:'+ batteryInfo.BatteryChargeState.NONE);
        console.info('BatteryChargeState() done:'+ batteryInfo.BatteryChargeState.ENABLE);
        console.info('BatteryChargeState() done:'+ batteryInfo.BatteryChargeState.DISABLE);
        console.info('BatteryChargeState() done:'+ batteryInfo.BatteryChargeState.FULL);
        console.info('-----------------------------------');

        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.UNKNOWN);
        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.GOOD);
        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.OVERHEAT);
        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.OVERVOLTAGE);
        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.COLD);
        console.info('BatteryHealthState() done:'+ batteryInfo.BatteryHealthState.DEAD);
        console.info('-----------------------------------');

        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_FULL);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_HIGH);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_NORMAL);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_LOW);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_WARNING);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_CRITICAL);
        console.info('BatteryCapacityLevel() done:'+ batteryInfo.BatteryCapacityLevel.LEVEL_SHUTDOWN);
        console.info('-----------------------------------');

        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_SOC);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_CHARGE_STATE);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_HEALTH_STATE);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_PLUGGED_TYPE);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_VOLTAGE);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_TECHNOLOGY);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_TEMPERATURE);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_PRESENT);
        console.info('CommonEventBatteryChangedKey() done:'+ batteryInfo.CommonEventBatteryChangedKey.EXTRA_CAPACITY_LEVEL);
        console.info('-----------------------------------');   
    }
    catch (err) {
        console.error("testBatteryInfo err: " + err)
    }
  
}

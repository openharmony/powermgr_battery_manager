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

import battery from '@system.battery';
import batteryInfo from '@ohos.batteryInfo';

import {describe, it, expect} from 'deccjsunit/index';

function successFunc(data, tag) {
    console.log(tag + ": level: " + data.level + ", charging: " + data.charging);
    let soc = (batteryInfo.batterySOC * 0.01);
    expect(fabs(soc - data.level) <= 1e-9).assertTrue();
    if (batteryInfo.chargingStatus === batteryInfo.BatteryChargeState.ENABLE ||
        batteryInfo.chargingStatus === batteryInfo.BatteryChargeState.FULL) {
        expect(data.charging).assertTrue();
    } else {
        expect(data.charging).assertFalse();
    }
}

function failFunc(data, code, tag) {
    console.log(tag + ": data: " + data + ", code: " + code);
    expect().assertFail();
}

function completeFunc(tag) {
    console.log(tag + ": The device information is obtained successfully.");
}

describe('appInfoTest', function () {
    console.log("*************System Battery Unit Test Begin*************");

    /**
     * @tc.number system_battery_js_0100
     * @tc.name get_status_success_test
     * @tc.desc Battery acquisition kit
     */
    const successTest = "get_status_success_test";
    it(successTest, 0, function () {
        battery.getStatus({
            success: (data) => {
                successFunc(data, successTest);
            },
            fail: (data, code) => {
                failFunc(data, code, successTest);
            },
            complete: () => {
                completeFunc(successTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0200
     * @tc.name get_status_success_null_test
     * @tc.desc Battery acquisition kit
     */
    const successNullTest = "get_status_success_null_test";
    it(successNullTest, 0, function () {
        battery.getStatus({
            success: null,
            fail: (data, code) => {
                failFunc(data, code, successNullTest);
            },
            complete: () => {
                completeFunc(successNullTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0300
     * @tc.name get_status_success_empty_test
     * @tc.desc Battery acquisition kit
     */
    const successEmptyTest = "get_status_success_null_test";
    it(successEmptyTest, 0, function () {
        battery.getStatus({
            fail: (data, code) => {
                failFunc(data, code, successEmptyTest);
            },
            complete: () => {
                completeFunc(successEmptyTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0400
     * @tc.name get_status_fail_null_test
     * @tc.desc Battery acquisition kit
     */
    let failNullTest = "get_status_fail_null_test";
    it(failNullTest, 0, function () {
        battery.getStatus({
            success: (data) => {
                successFunc(data, failNullTest);
            },
            fail: null,
            complete: () => {
                completeFunc(failNullTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0500
     * @tc.name get_status_fail_empty_test
     * @tc.desc Battery acquisition kit
     */
    let failEmptyTest = "get_status_fail_empty_test";
    it(failEmptyTest, 0, function () {
        battery.getStatus({
            success: () => {
                successFunc(data, failEmptyTest);
            },
            complete: () => {
                completeFunc(failEmptyTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0600
     * @tc.name get_status_complete_null_test
     * @tc.desc Battery acquisition kit
     */
    let completeNullTest = "get_status_complete_null_test";
    it(completeNullTest, 0, function () {
        battery.getStatus({
            success: (data) => {
                successFunc(data, completeNullTest);
            },
            fail: (data, code) => {
                failFunc(data, code, completeNullTest);
            },
            complete: null
        });
    });

    /**
     * @tc.number system_battery_js_0700
     * @tc.name get_status_complete_empty_test
     * @tc.desc Battery acquisition kit
     */
    let completeEmptyTest = "get_status_complete_empty_test";
    it(completeEmptyTest, 0, function () {
        battery.getStatus({
            success: (data) => {
                successFunc(data, completeEmptyTest);
            },
            fail: (data, code) => {
                failFunc(data, code, completeEmptyTest);
            }
        });
    });

    /**
     * @tc.number system_battery_js_0800
     * @tc.name get_status_all_null
     * @tc.desc Battery acquisition kit
     */
    it('get_status_all_null', 0, function () {
        let allNull = false;
        battery.getStatus({
            success: null,
            fail: null,
            complete: null,
        });
        expect(!allNull).assertTrue();
    });

    /**
     * @tc.number system_battery_js_0800
     * @tc.name get_status_all_empty
     * @tc.desc Battery acquisition kit
     */
    it('get_status_all_empty', 0, function () {
        let allNull = false;
        battery.getStatus();
        expect(!allNull).assertTrue();
    });
});

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
import hilog from '@ohos.hilog'
import Utils from './Util.test';
import charger from '@ohos.charger'

let domain: number = 0x0000;
let tag: string = 'testTag';

export default function ChargerTest() {

  describe("ChargerTest", (): void => {
    hilog.info(domain, tag, '%{public}s', 'describe start');
    it("ChargerTest0100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0100 start');
        try {
          let chargeTypeNone = charger.ChargeType.NONE;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeNone));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeNone);
          expect(new Number(chargeTypeNone)).assertEqual(new Number(0));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0200 start');
        try {
          let chargeTypeWiredNormal = charger.ChargeType.WIRED_NORMAL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWiredNormal));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWiredNormal);
          expect(new Number(chargeTypeWiredNormal)).assertEqual(new Number(1));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0300 start');
        try {
          let chargeTypeWiredQuick = charger.ChargeType.WIRED_QUICK;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWiredQuick));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWiredQuick);
          expect(new Number(chargeTypeWiredQuick)).assertEqual(new Number(2));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0400 start');
        try {
          let chargeTypeWiredSuperQuick = charger.ChargeType.WIRED_SUPER_QUICK;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWiredSuperQuick));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWiredSuperQuick);
          expect(new Number(chargeTypeWiredSuperQuick)).assertEqual(new Number(3));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0500 start');
        try {
          let chargeTypeWirelessNormal = charger.ChargeType.WIRELESS_NORMAL;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWirelessNormal));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWirelessNormal);
          expect(new Number(chargeTypeWirelessNormal)).assertEqual(new Number(4));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0600 start');
        try {
          let chargeTypeWirelessQuick = charger.ChargeType.WIRELESS_QUICK;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWirelessQuick));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWirelessQuick);
          expect(new Number(chargeTypeWirelessQuick)).assertEqual(new Number(5));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
    it("ChargerTest0700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void): Promise<void> => {
        hilog.info(domain, tag, '%{public}s', 'ChargerTest0700 start');
        try {
          let chargeTypeWirelessSuperQuick = charger.ChargeType.WIRELESS_SUPER_QUICK;
          hilog.info(domain, tag, '%{public}s', JSON.stringify(chargeTypeWirelessSuperQuick));
          hilog.info(domain, tag, '%{public}s', typeof chargeTypeWirelessSuperQuick);
          expect(new Number(chargeTypeWirelessSuperQuick)).assertEqual(new Number(6));
          done();
        } catch (e) {
          hilog.error(domain, tag, '%{public}s', JSON.stringify(e));
          done();
        }
      })
  })
  hilog.info(domain, tag, '%{public}s', 'ChargerTest end');
}

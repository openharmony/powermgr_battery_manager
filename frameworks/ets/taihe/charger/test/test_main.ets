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

import { charger } from "./@ohos.charger";

enum ChargeType : i32 {
  NONE,
  WIRED_NORMAL,
  WIRED_QUICK,
  WIRED_SUPER_QUICK,
  WIRELESS_NORMAL,
  WIRELESS_QUICK,
  WIRELESS_SUPER_QUICK
}

function main() {
    console.info('charger.ChargeType.NONE test done, val:', charger.ChargeType.NONE);
    console.info('charger.ChargeType.WIRED_NORMAL test done, val:', charger.ChargeType.WIRED_NORMAL);
    console.info('charger.ChargeType.WIRED_QUICK test done, val:', charger.ChargeType.WIRED_QUICK);
    console.info('charger.ChargeType.WIRED_SUPER_QUICK test done, val:', charger.ChargeType.WIRED_SUPER_QUICK);
    console.info('charger.ChargeType.WIRELESS_NORMAL test done, val:', charger.ChargeType.WIRELESS_NORMAL);
    console.info('charger.ChargeType.WIRELESS_QUICK test done, val:', charger.ChargeType.WIRELESS_QUICK);
    console.info('charger.ChargeType.WIRELESS_SUPER_QUICK test done, val:', charger.ChargeType.WIRELESS_SUPER_QUICK);
}
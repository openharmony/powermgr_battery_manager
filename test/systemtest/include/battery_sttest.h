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

#ifndef BATTERY_STTEST_H
#define BATTERY_STTEST_H

#include <gtest/gtest.h>
#include "battery_led.h"

class BatterySttest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

struct BatteryLedUnitTest {};

void UpdateLedColorTest(const int32_t &chargestate, const int32_t &capacity,
    OHOS::HDI::Battery::V1_0::BatteryLed &batteryled);

template <typename Tag, typename PrivateFun, PrivateFun privateFun>
class UpdateLedColorImplement {
    friend void UpdateLedColorTest(const int32_t &chargestate, const int32_t &capacity,
        OHOS::HDI::Battery::V1_0::BatteryLed &batteryled)
    {
        (batteryled.*privateFun)(chargestate, capacity);
    }
};

template class UpdateLedColorImplement <
    BatteryLedUnitTest,
    decltype(&OHOS::HDI::Battery::V1_0::BatteryLed::UpdateLedColor),
    &OHOS::HDI::Battery::V1_0::BatteryLed::UpdateLedColor
>;
#endif // BATTERY_STTEST_H

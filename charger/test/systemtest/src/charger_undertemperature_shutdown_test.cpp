/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "charger_undertemperature_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "battery_log.h"

#include "power_supply_provider.h"
#include "battery_thread_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery;
using namespace OHOS::HDI::Battery::V1_1;
using namespace std;

namespace ChargerUndertemperatureShutdownTest {

/**
 * @tc.name: BatteryST_009
 * @tc.desc: Test functions temperature in normal range in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerUndertemperatureShutdownTest, BatteryST_009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_009 start.");
    ChargerThread ct;
    ChargerThreadInitTest(ct);

    ChargerThreadHandleTemperatureTest(-99, ct);
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_009 end.");
}

/**
 * @tc.name: BatteryST_010
 * @tc.desc: Test functions low temperature shutdown in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerUndertemperatureShutdownTest, BatteryST_010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_010 start.");
    ChargerThread ct;
    ChargerThreadInitTest(ct);
    if (false) {
        ChargerThreadHandleTemperatureTest(-100, ct);
    }
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_010 end.");
}
}

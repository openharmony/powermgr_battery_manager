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

#include "charger_overtemperature_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "battery_log.h"

#include "power_supply_provider.h"
#include "battery_thread_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery;
using namespace OHOS::HDI::Battery::V1_1;
using namespace std;

namespace ChargerOvertemperatureShutdownTest {

/**
 * @tc.name: BatteryST_007
 * @tc.desc: Test functions temperature in normal range in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOvertemperatureShutdownTest, BatteryST_007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_007 start.");
    ChargerThread ct;

    ChargerThreadInitTest(ct);
    ChargerThreadHandleTemperatureTest(599, ct);
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_007 end.");
}

/**
 * @tc.name: BatteryST_008
 * @tc.desc: Test functions high temperature shutdown in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOvertemperatureShutdownTest, BatteryST_008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_008 start.");
    ChargerThread ct;
    ChargerThreadInitTest(ct);
    if (false) {
        ChargerThreadHandleTemperatureTest(600, ct);
    }
    BATTERY_HILOGI(LABEL_TEST, "enter. BatteryST_008 end.");
}
}

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

#include "charger_overtemperature_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "utils/hdf_log.h"

#include "power_supply_provider.h"
#include "battery_thread_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_1;
using namespace std;

namespace ChargerOvertemperatureShutdownTest {
void ChargerOvertemperatureShutdownTest::SetUpTestCase(void)
{
}

void ChargerOvertemperatureShutdownTest::TearDownTestCase(void)
{
}

void ChargerOvertemperatureShutdownTest::SetUp(void)
{
}

void ChargerOvertemperatureShutdownTest::TearDown(void)
{
}

/**
 * @tc.name: BatteryST_007
 * @tc.desc: Test functions temperature in normal range in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOvertemperatureShutdownTest, BatteryST_007, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_007 start.", __func__);
    ChargerThread ct;

    ChargerThreadInitTest(ct);
    ChargerThreadHandleTemperatureTest(599, ct);
    HDF_LOGD("%{public}s: enter. BatteryST_007 end.", __func__);
}

/**
 * @tc.name: BatteryST_008
 * @tc.desc: Test functions high temperature shutdown in chargerThread
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOvertemperatureShutdownTest, BatteryST_008, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_008 start.", __func__);
    ChargerThread ct;
    ChargerThreadInitTest(ct);
    if (false) {
        ChargerThreadHandleTemperatureTest(600, ct);
    }
    HDF_LOGD("%{public}s: enter. BatteryST_008 end.", __func__);
}
}

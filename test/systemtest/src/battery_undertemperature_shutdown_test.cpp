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

#include "battery_undertemperature_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "utils/hdf_log.h"

#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_thread_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_0;
using namespace std;

namespace BatteryUndertemperatureShutdownTest {
void BatteryUndertemperatureShutdownTest::SetUpTestCase(void)
{
}

void BatteryUndertemperatureShutdownTest::TearDownTestCase(void)
{
}

void BatteryUndertemperatureShutdownTest::SetUp(void)
{
}

void BatteryUndertemperatureShutdownTest::TearDown(void)
{
}

/**
 * @tc.name: BatteryST_005
 * @tc.desc: Test functions temperature in normal range in BatteryHostServiceStub
 * @tc.type: FUNC
 */
HWTEST_F (BatteryUndertemperatureShutdownTest, BatteryST_005, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_005 start.", __func__);
    BatteryHostServiceStub stub;
    stub.Init();

    HandleTemperatureTest(-99, stub);
    HDF_LOGD("%{public}s: enter. BatteryST_005 end.", __func__);
}

/**
 * @tc.name: BatteryST_006
 * @tc.desc: Test functions lower temperature shutdown in BatteryHostServiceStub
 * @tc.type: FUNC
 */
HWTEST_F (BatteryUndertemperatureShutdownTest, BatteryST_006, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_006 start.", __func__);
    BatteryHostServiceStub stub;
    stub.Init();

    HandleTemperatureTest(-100, stub);
    HDF_LOGD("%{public}s: enter. BatteryST_006 end.", __func__);
}
}

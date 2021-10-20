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

#include <csignal>
#include <iostream>
#include "utils/hdf_log.h"

#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_thread_test.h"
#include "hdi_service_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_0;
using namespace std;

namespace HdiServiceTest {
void HdiServiceTest::SetUpTestCase(void)
{
}

void HdiServiceTest::TearDownTestCase(void)
{
}

void HdiServiceTest::SetUp(void)
{
}

void HdiServiceTest::TearDown(void)
{
}

/**
 * @tc.name: BatteryST_003
 * @tc.desc: Test functions temperature in normal range in BatteryHostServiceStub
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, BatteryST_003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_003 start.", __func__);
    BatteryHostServiceStub stub;
    stub.Init();

    HandleTemperatureTest(599, stub);
    HDF_LOGD("%{public}s: enter. BatteryST_003 end.", __func__);
}

/**
 * @tc.name: BatteryST_004
 * @tc.desc: Test functions high temperature shutdown in BatteryHostServiceStub
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, BatteryST_004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. BatteryST_004 start.", __func__);
    BatteryHostServiceStub stub;
    stub.Init();

    HandleTemperatureTest(600, stub);
    HDF_LOGD("%{public}s: enter. BatteryST_004 end.", __func__);
}
}

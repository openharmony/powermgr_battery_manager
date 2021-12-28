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

#include "capacity_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "utils/hdf_log.h"

#include "power_supply_provider.h"
#include "battery_host_service_stub.h"
#include "battery_thread_test.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_0;
using namespace std;

namespace CapacityShutdownTest {
void CapacityShutdownTest::SetUpTestCase(void)
{
}

void CapacityShutdownTest::TearDownTestCase(void)
{
}

void CapacityShutdownTest::SetUp(void)
{
}

void CapacityShutdownTest::TearDown(void)
{
}

/**
 * @tc.name: HdiServiceShutdown001
 * @tc.desc: capacity shutdown test
 * @tc.type: FUNC
 */
HWTEST_F (CapacityShutdownTest, HdiServiceShutdown001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown001 start.", __func__);
    const int32_t CHARGE_STATE_NONE = 0;

    ChargerThread ct;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(CHARGE_STATE_NONE, ct);
    int32_t state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_NONE);

    HandleCapacityTest(4, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: capacity is 4, not shutdown.", __func__);

    const int32_t CHARGE_STATE_ENABLE = 1;
    SetChargeStateTest(CHARGE_STATE_ENABLE, ct);
    state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_ENABLE);
    HandleCapacityTest(4, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: capacity is 4, not shutdown.", __func__);

    SetChargeStateTest(CHARGE_STATE_ENABLE, ct);
    state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_ENABLE);
    HandleCapacityTest(3, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: capacity is 3, but charge state is enable, not shutdown.", __func__);

    SetChargeStateTest(CHARGE_STATE_NONE, ct);
    state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_NONE);
    HandleCapacityTest(3, ct); // ShutDown

    HDF_LOGD("%{public}s: enter. HdiServiceShutdown001 end.", __func__);
}
}

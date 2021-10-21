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
 * @tc.name: HdiServiceShutdown001
 * @tc.desc: Test functions capacity in normal range and chage state is not CHARGE_STATE_ENABLE in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiServiceShutdown001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown001 start.", __func__);
    const int32_t CHARGE_STATE_NONE = 0;

    ChargerThread ct;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(CHARGE_STATE_NONE, ct);
    int32_t state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_NONE);

    HandleCapacityTest(4, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown001 end.", __func__);
}

/**
 * @tc.name: HdiServiceShutdown002
 * @tc.desc: Test functions capacity in normal range and chage state is CHARGE_STATE_ENABLE in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiServiceShutdown002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown002 start.", __func__);
    const int32_t CHARGE_STATE_ENABLE = 1;

    ChargerThread ct;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(CHARGE_STATE_ENABLE, ct);
    int32_t state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_ENABLE);

    HandleCapacityTest(4, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown002 end.", __func__);
}

/**
 * @tc.name: HdiServiceShutdown003
 * @tc.desc: Test functions capacity lower but charge state is CHARGE_STATE_ENABLE in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiServiceShutdown003, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown003 start.", __func__);
    const int32_t CHARGE_STATE_ENABLE = 1;

    ChargerThread ct;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(CHARGE_STATE_ENABLE, ct);
    int32_t state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_ENABLE);

    HandleCapacityTest(3, ct); // Do not ShutDown
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown003 end.", __func__);
}

/**
 * @tc.name: HdiServiceShutdown004
 * @tc.desc: Test functions capacity reason shutdown in ChargerThread
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, HdiServiceShutdown004, TestSize.Level1)
{
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown004 start.", __func__);
    const int32_t CHARGE_STATE_NONE = 0;

    ChargerThread ct;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(CHARGE_STATE_NONE, ct);
    int32_t state = GetChargeStateTest(ct);
    ASSERT_TRUE(state == CHARGE_STATE_NONE);

    HandleCapacityTest(3, ct); // ShutDown
    HDF_LOGD("%{public}s: enter. HdiServiceShutdown004 end.", __func__);
}
}

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

#include "charger_offline_shutdown_test.h"

#include <csignal>
#include <iostream>
#include "utils/hdf_log.h"

#include "power_supply_provider.h"
#include "battery_thread_test.h"
#include "charger_thread.h"

using namespace testing::ext;
using namespace OHOS::HDI::Battery::V1_1;
using namespace std;

namespace ChargerOfflineShutdownTest {
void ChargerOfflineShutdownTest::SetUpTestCase(void)
{
}

void ChargerOfflineShutdownTest::TearDownTestCase(void)
{
}

void ChargerOfflineShutdownTest::SetUp(void)
{
}

void ChargerOfflineShutdownTest::TearDown(void)
{
}

namespace {
    struct ChargerThreadUnitTest {};
}

void HandleChargingStateTest(ChargerThread& cthread);
std::unique_ptr<BatteryBacklight> GetBacklightTest(ChargerThread& cthread);

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class HandleChargingStateImplement {
    friend void HandleChargingStateTest(ChargerThread& cthread)
    {
        (cthread.*privateFun)();
    }
};

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class GetBacklightImplement {
    friend std::unique_ptr<BatteryBacklight> GetBacklightTest(ChargerThread& cthread)
    {
        return std::move(cthread.*privateFun);
    }
};

template class HandleChargingStateImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::HandleChargingState),
    &ChargerThread::HandleChargingState
>;

template class GetBacklightImplement <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::backlight_),
    &ChargerThread::backlight_
>;

/**
 * @tc.name: BatteryST_011
 * @tc.desc: Test functions shutdown during charing and offline less than 2 seconds
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOfflineShutdownTest, BatteryST_011, TestSize.Level1)
{
    HDF_LOGD("%{public}s: BatteryST_011 start.", __func__);
    ChargerThread ct;
    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_NONE;

    ChargerThreadInitTest(ct);
    SetChargeStateTest(chargestate, ct);
    HandleChargingStateTest(ct);
    bool screenOn = GetBacklightTest(ct)->GetScreenState();
    HDF_LOGD("%{public}s: screenOn=%{public}d", __func__, screenOn);
    ASSERT_TRUE(screenOn);

    usleep(1900000);
    HandleChargingStateTest(ct); // do not shutdown
    HDF_LOGD("%{public}s: BatteryST_011 end.", __func__);
}

/**
 * @tc.name: BatteryST_012
 * @tc.desc: Test functions shutdown during charing and offline 2 seconds
 * @tc.type: FUNC
 */
HWTEST_F (ChargerOfflineShutdownTest, BatteryST_012, TestSize.Level1)
{
    HDF_LOGD("%{public}s: BatteryST_012 start.", __func__);
    ChargerThread ct;
    int32_t chargestate = PowerSupplyProvider::CHARGE_STATE_ENABLE;
    ChargerThreadInitTest(ct);
    SetChargeStateTest(chargestate, ct);

    chargestate = PowerSupplyProvider::CHARGE_STATE_NONE;
    SetChargeStateTest(chargestate, ct);
    HandleChargingStateTest(ct);
    bool screenOn = GetBacklightTest(ct)->GetScreenState();
    HDF_LOGD("%{public}s: screenOn=%{public}d", __func__, screenOn);
    ASSERT_TRUE(screenOn);

    sleep(2);
    if (false) {
        HandleChargingStateTest(ct); // shutdown
    }
    HDF_LOGD("%{public}s: BatteryST_012 end.", __func__);
}
}

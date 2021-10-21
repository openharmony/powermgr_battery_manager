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
#include "charger_thread.h"

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

namespace {
    struct ChargerThreadUnitTest {};
}

std::unique_ptr<BatteryBacklight> GetBacklightTest(ChargerThread &cthread);

template <typename Tag, typename PrivateFun, PrivateFun privateFun>
class RebootTest {
    friend std::unique_ptr<BatteryBacklight> GetBacklightTest(ChargerThread &cthread)
    {
        return std::move(cthread.*privateFun);
    }
};

template class RebootTest <
    ChargerThreadUnitTest,
    decltype(&ChargerThread::backlight_),
    &ChargerThread::backlight_
>;

/**
 * @tc.name: BatteryST_001
 * @tc.desc: Test functions reboot in ChargerThread, unplugged not until reboot
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, BatteryST_001, TestSize.Level1)
{
    HDF_LOGD("%{public}s: BatteryST_001 start.", __func__);
    std::string reason = "rebootTest";
    BatteryThreadTest btTest;
    ChargerThread ct;

    ChargerThreadInitTest(ct);
    btTest.SetKeyState(116, 2, 0, ct);
    btTest.HandlePowerKey(116, 1999, ct); // do not reboot
    bool screenOn = GetBacklightTest(ct)->GetScreenState();
    HDF_LOGD("%{public}s: screenOn=%{public}d", __func__, screenOn);

    ASSERT_TRUE(screenOn);
    HDF_LOGD("%{public}s: BatteryST_001 end.", __func__);
}

/**
 * @tc.name: BatteryST_002
 * @tc.desc: Test functions reboot in ChargerThread, unplugged until reboot
 * @tc.type: FUNC
 */
HWTEST_F (HdiServiceTest, BatteryST_002, TestSize.Level1)
{
    HDF_LOGD("%{public}s: BatteryST_002 start.", __func__);
    std::string reason = "rebootTest";
    BatteryThreadTest btTest;
    ChargerThread ct;

    ChargerThreadInitTest(ct);
    btTest.SetKeyState(116, 2, 0, ct);
    btTest.HandlePowerKey(116, 2000, ct); // reboot
    HDF_LOGD("%{public}s: BatteryST_0022 end.", __func__);
}
}

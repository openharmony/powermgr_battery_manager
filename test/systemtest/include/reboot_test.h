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

#ifndef REBOOT_TEST_H
#define REBOOT_TEST_H

#include <gtest/gtest.h>
#include "power_supply_provider.h"
#include "battery_thread_test.h"
#include "charger_thread.h"

namespace RebootTest {
class RebootTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
namespace {
    struct ChargerThreadUnitTest {};
}

std::unique_ptr<OHOS::HDI::Battery::V1_1::BatteryBacklight>
    GetBacklightTest(OHOS::HDI::Battery::V1_1::ChargerThread& cthread);

template<typename Tag, typename PrivateFun, PrivateFun privateFun>
class Reboot {
    friend std::unique_ptr<OHOS::HDI::Battery::V1_1::BatteryBacklight> GetBacklightTest(
        OHOS::HDI::Battery::V1_1::ChargerThread& cthread)
    {
        return std::move(cthread.*privateFun);
    }
};

template class Reboot <
    ChargerThreadUnitTest,
    decltype(&OHOS::HDI::Battery::V1_1::ChargerThread::backlight_),
    &OHOS::HDI::Battery::V1_1::ChargerThread::backlight_
>;
}
#endif // REBOOT_TEST_H

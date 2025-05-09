/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "battery_info_test.h"

#include <string>

#include "battery_info.h"
#include "battery_log.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
std::shared_ptr<BatteryInfo> g_infoTest = nullptr;
} // namespace

void BatteryInfoTest::SetUpTestCase()
{
    g_infoTest = std::make_shared<BatteryInfo>();
}

void BatteryInfoTest::TearDownTestCase()
{
    g_infoTest = nullptr;
}

/**
 * @tc.name: BatteryInfo001
 * @tc.desc: BatteryInfo GetRemainEnergy function test
 * @tc.type: FUNC
 */
HWTEST_F(BatteryInfoTest, BatteryInfo001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo001 function start!");
    const int32_t remainEnergy = 100;
    g_infoTest->SetRemainEnergy(remainEnergy);
    EXPECT_EQ(g_infoTest->GetRemainEnergy(), remainEnergy);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo001 function end!");
}

/**
 * @tc.name: BatteryInfo002
 * @tc.desc: BatteryInfo GetChargeType and SetChargeType function test
 * @tc.type: FUNC
 */
HWTEST_F(BatteryInfoTest, BatteryInfo002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo002 function start!");
    const ChargeType none = ChargeType::WIRED_NORMAL;
    g_infoTest->SetChargeType(none);
    EXPECT_EQ(g_infoTest->GetChargeType(), none);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo002 function end!");
}
} // namespace PowerMgr
} // namespace OHOS
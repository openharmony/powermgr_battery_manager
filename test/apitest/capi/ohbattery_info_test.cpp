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

#include <gtest/test.h>

#include "battery_log.h"
#include "battery_srv_client.h"
#include "ohbattery_info.h"

namespace OHOS {
namespace PowerMgr {

class OhBatteryInfoTest : public testing::Test {
public:
    void SetUp() override {};
    void TearDown() override {};
    static void SetUpTestCase() override {};
    static void TearDownTestCase() override {};
}

HWTEST_F(OhBatteryInfoTest, OH_BatteryInfo_GetCapacity_001, TestSize.Level1)
{
    BATTERY_HILOGI("OH_BatteryInfo_GetCapacity_001 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t resultFromClient = batterySrvClient.GetCapacity();
    int32_t resultFromCApi = OH_BatteryInfo_GetCapacity();
    EXPECT_EQ(resultFromClient, resultFromCApi);
}

HWTEST_F(OhBatteryInfoTest, OH_BatteryInfo_GetPluggedType_001, TestSize.Level1)
{
    BATTERY_HILOGI("OH_BatteryInfo_GetPluggedType_001 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryPluggedType resultFromClient = batterySrvClient.GetPluggedType();
    BatteryInfo_BatteryPluggedType resultFromCApi = OH_BatteryInfo_GetPluggedType();
    EXPECT_EQ(static_cast<uint32_t>(resultFromClient), static_cast<uint32_t>(resultFromCApi));
}
}
}
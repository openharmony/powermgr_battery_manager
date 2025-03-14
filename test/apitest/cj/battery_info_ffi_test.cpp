/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "battery_log.h"
#include "battery_srv_client.h"
#include "battery_info_ffi.h"
#include "battery_info_ffi_test.h"

namespace OHOS {
namespace PowerMgr {
using namespace testing::ext;

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test001 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t resultFromClient = batterySrvClient.GetCapacity();
    int32_t resultFromCjApi = FfiBatteryInfoBatterySOC();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test001 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test002 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryChargeState resultFromClient = batterySrvClient.GetChargingStatus();
    int32_t resultFromCjApi = FfiBatteryInfoGetChargingState();
    EXPECT_EQ(static_cast<int32_t>(resultFromClient), resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test002 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test003 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryHealthState resultFromClient = batterySrvClient.GetHealthStatus();
    int32_t resultFromCjApi = FfiBatteryInfoGetHealthState();
    EXPECT_EQ(static_cast<int32_t>(resultFromClient), resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test003 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test004 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryPluggedType resultFromClient = batterySrvClient.GetPluggedType();
    int32_t resultFromCjApi = FfiBatteryInfoGetPluggedType();
    EXPECT_EQ(static_cast<int32_t>(resultFromClient), resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test004 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test005 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t resultFromClient = batterySrvClient.GetVoltage();
    int32_t resultFromCjApi = FfiBatteryInfoGetVoltage();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test005 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test006 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t resultFromClient = batterySrvClient.GetNowCurrent();
    int32_t resultFromCjApi = FfiBatteryInfoGetBatteryNowCurrent();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test006 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test007 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    std::string resultFromClient = batterySrvClient.GetTechnology();
    std::string resultFromCjApi = FfiBatteryInfoGetTechnology();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test007 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test008 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    int32_t resultFromClient = batterySrvClient.GetBatteryTemperature();
    int32_t resultFromCjApi = FfiBatteryInfoGetBatteryTemperature();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test008 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test009 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    bool resultFromClient = batterySrvClient.GetPresent();
    bool resultFromCjApi = FfiBatteryInfoGetBatteryPresent();
    EXPECT_EQ(resultFromClient, resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test009 end");
}

HWTEST_F(BatteryInfoFFITest, BatteryInfo_FFI_Test010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test010 start");
    BatterySrvClient& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryCapacityLevel resultFromClient = batterySrvClient.GetCapacityLevel();
    int32_t resultFromCjApi = FfiBatteryInfoGetCapacityLevel();
    EXPECT_EQ(static_cast<int32_t>(resultFromClient), resultFromCjApi);
    BATTERY_HILOGI(LABEL_TEST, "BatteryInfo_FFI_Test010 end");
}
}
}
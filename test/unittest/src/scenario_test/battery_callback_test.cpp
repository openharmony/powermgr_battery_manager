/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "battery_callback_test.h"

#ifdef GTEST
#define private   public
#define protected public
#endif
#include "battery_service.h"
#include "battery_callback.h"
#include "battery_log.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
sptr<BatteryService> g_service;
}

int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V2_0::BatteryInfo& event)
{
    return ERR_OK;
}

void BatteryCallbackTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
}

void BatteryCallbackTest::TearDownTestCase()
{
    g_service = nullptr;
}

/**
 * @tc.name: BatteryCallback001
 * @tc.desc: Update BatteryInfo, the eventCb_ is valid
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryCallbackTest, BatteryCallback001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryCallback001 function start!");
    sptr<HDI::Battery::V2_0::IBatteryInterface> iBatteryInterface;
    iBatteryInterface = HDI::Battery::V2_0::IBatteryInterface::Get();
    sptr<HDI::Battery::V2_0::IBatteryCallback> callback = new BatteryCallback();
    EXPECT_EQ(iBatteryInterface->Register(callback), HDF_SUCCESS);

    BatteryCallback::BatteryEventCallback eventCb = std::bind(&HandleBatteryCallbackEvent, std::placeholders::_1);
    EXPECT_EQ(BatteryCallback::RegisterBatteryEvent(eventCb), HDF_SUCCESS);
    HDI::Battery::V2_0::BatteryInfo event;
    iBatteryInterface->GetBatteryInfo(event);
    EXPECT_NE(callback->Update(event), HDF_FAILURE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryCallback001 function end!");
}

/**
 * @tc.name: BatteryCallback002
 * @tc.desc: Update BatteryInfo, the eventCb_ is invalid
 * @tc.type: FUNC
 */
HWTEST_F(BatteryCallbackTest, BatteryCallback002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryCallback002 function start!");
    BatteryCallback::BatteryEventCallback eventCb = nullptr;
    EXPECT_EQ(BatteryCallback::RegisterBatteryEvent(eventCb), HDF_SUCCESS);
    HDI::Battery::V2_0::BatteryInfo event;
    sptr<HDI::Battery::V2_0::IBatteryCallback> callback = new BatteryCallback();
    EXPECT_EQ(callback->Update(event), HDF_FAILURE);
    BATTERY_HILOGI(LABEL_TEST, "BatteryCallback002 function end!");
}
} // namespace PowerMgr
} // namespace OHOS

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "battery_callback.h"
#include "battery_service.h"
#include "power_common.h"
#include <memory>

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
sptr<BatteryService> g_service;
}

int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V1_1::BatteryInfo& event)
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
    sptr<HDI::Battery::V1_1::IBatteryInterface> iBatteryInterface;
    iBatteryInterface = HDI::Battery::V1_1::IBatteryInterface::Get();
    sptr<HDI::Battery::V1_1::IBatteryCallback> callback = new BatteryCallback();
    EXPECT_EQ(iBatteryInterface->Register(callback), HDF_SUCCESS);

    BatteryCallback::BatteryEventCallback eventCb = std::bind(&HandleBatteryCallbackEvent, std::placeholders::_1);
    EXPECT_EQ(BatteryCallback::RegisterBatteryEvent(eventCb), HDF_SUCCESS);
    HDI::Battery::V1_1::BatteryInfo event;
    iBatteryInterface->GetBatteryInfo(event);
    EXPECT_NE(callback->Update(event), HDF_FAILURE);
}
} // namespace PowerMgr
} // namespace OHOS

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "battery_client_death_recipient_test.h"

#include "battery_log.h"
#include "battery_srv_client.h"
#include "test_utils.h"
#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
/**
 * @tc.name: BatteryClientDeathRecipient001
 * @tc.desc: test OnRemoteDied function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryClientDeathRecipientTest, BatteryClient001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient001 start.");
    auto& batterySrvClient = BatterySrvClient::GetInstance();
    EXPECT_EQ(batterySrvClient.Connect(), ERR_OK);

    wptr<IRemoteObject> remoteObj = nullptr;
    std::shared_ptr<IRemoteObject::DeathRecipient> deathRecipient =
        std::make_shared<BatterySrvClient::BatterySrvDeathRecipient>();
    EXPECT_NE(deathRecipient, nullptr);
    deathRecipient->OnRemoteDied(remoteObj);
    EXPECT_NE(batterySrvClient.proxy_, nullptr);
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient001 end.");
}
} // namespace
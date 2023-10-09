/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "battery_log.h"
#include "battery_srv_client.h"
#include "test_utils.h"

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
HWTEST_F(BatteryClientDeathRecipientTest, BatteryClientDeathRecipient001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient001 start.");
    auto& batterySrvClient = BatterySrvClient::GetInstance();
    EXPECT_NE(batterySrvClient.Connect(), nullptr);

    wptr<IRemoteObject> remoteObj = nullptr;
    std::shared_ptr<IRemoteObject::DeathRecipient> deathRecipient =
        std::make_shared<BatterySrvClient::BatterySrvDeathRecipient>(batterySrvClient);
    EXPECT_NE(deathRecipient, nullptr);
    deathRecipient->OnRemoteDied(remoteObj);
    EXPECT_NE(batterySrvClient.proxy_, nullptr);
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient001 end.");
}

/**
 * @tc.name: BatteryClientDeathRecipient002
 * @tc.desc: test OnRemoteDied function
 * @tc.type: FUNC
 */
HWTEST_F(BatteryClientDeathRecipientTest, BatteryClientDeathRecipient002, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient002 start.");
    auto& batterySrvClient = BatterySrvClient::GetInstance();
    EXPECT_NE(batterySrvClient.Connect(), nullptr);

    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == nullptr) {
        BATTERY_HILOGD(LABEL_TEST, "Failed to get Registry");
        return;
    }
    wptr<IRemoteObject> remoteObj = sysMgr->CheckSystemAbility(POWER_MANAGER_BATT_SERVICE_ID);
    if (remoteObj == nullptr) {
        BATTERY_HILOGD(LABEL_TEST, "GetSystemAbility failed");
        return;
    }

    {
        std::shared_ptr<IRemoteObject::DeathRecipient> deathRecipient =
            std::make_shared<BatterySrvClient::BatterySrvDeathRecipient>(batterySrvClient);
        EXPECT_NE(deathRecipient, nullptr);
        deathRecipient->OnRemoteDied(remoteObj);
        deathRecipient = nullptr;
    }

    ASSERT_EQ(batterySrvClient.proxy_, nullptr);
    BATTERY_HILOGD(LABEL_TEST, "BatteryClientDeathRecipient002 end.");
}
} // namespace

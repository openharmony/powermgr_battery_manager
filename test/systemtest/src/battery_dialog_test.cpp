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

#include "battery_dialog_test.h"
#include "battery_service.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::PowerMgr;
using namespace OHOS::HDI::Battery::V1_1;
using namespace std;

static sptr<BatteryService> g_service;

namespace BatteryDialogTest {
void BatteryDialogTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
}

void BatteryDialogTest::TearDownTestCase()
{
    g_service->OnStop();
    DelayedSpSingleton<BatteryService>::DestroyInstance();
}

void BatteryDialogTest::SetUp()
{
}

void BatteryDialogTest::TearDown()
{
}

/**
 * @tc.name: BatteryDailogST_001
 * @tc.desc: test pull up BatteryDialog by service
 * @tc.type: FUNC
 * @tc.require: issueI5I9BF
 */
HWTEST_F (BatteryDialogTest, BatteryDialogST_001, TestSize.Level1)
{
    EXPECT_TRUE(g_service->ShowBatteryDialog());
}

/**
 * @tc.name: BatteryDailogST_002
 * @tc.desc: test kill BatteryDialog by service
 * @tc.type: FUNC
 * @tc.require: issueI5I9BF
 */
HWTEST_F (BatteryDialogTest, BatteryDialogST_002, TestSize.Level1)
{
    EXPECT_TRUE(g_service->DestoryBatteryDialog());
}
}

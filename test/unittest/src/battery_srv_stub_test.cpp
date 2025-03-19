/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "battery_srv_stub_test.h"

#include "ipc_types.h"
#include "parcel.h"

#include "battery_log.h"
#include "battery_manager_ipc_interface_code.h"
#include "battery_service.h"
#include "battery_srv_proxy.h"
#include "ibattery_srv.h"
#include "power_mgr_errors.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
sptr<BatteryService> g_service;
MessageParcel g_reply;
MessageOption g_option;
MessageParcel g_data;
} // namespace

void BatterySrvStubTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->OnStart();
}

void BatterySrvStubTest::TearDownTestCase()
{
    g_service = nullptr;
}

namespace {
/**
 * @tc.name: BatterySrvStub001
 * @tc.desc: Test functions OnRemoteRequest no WriteInterfaceToken
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub001 function start!");
    uint32_t code = 0;
    MessageParcel data;
    int32_t ret = g_service->OnRemoteRequest(code, data, g_reply, g_option);
    EXPECT_EQ(ret, E_GET_POWER_SERVICE_FAILED);
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub001 function end!");
}

/**
 * @tc.name: BatterySrvStub002
 * @tc.desc: Test BatterySrvInterfaceCode BATT_GET_CAPACITY to BATT_GET_BATTERY_REMAIN_ENERGY code
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub002 function start!");
    uint32_t begin = static_cast<uint32_t>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_CAPACITY);
    uint32_t end = static_cast<uint32_t>(PowerMgr::BatterySrvInterfaceCode::BATT_GET_BATTERY_REMAIN_ENERGY);
    for (uint32_t code = begin; code <= end; ++code) {
        g_data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor());
        int32_t ret = g_service->OnRemoteRequest(code, g_data, g_reply, g_option);
        EXPECT_EQ(ret, ERR_OK) << "ret: " << ret << " code: " << code;
    }

    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub002 function end!");
}

/**
 * @tc.name: BatterySrvStub003
 * @tc.desc: Test ResetProxy Invalid code
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub003 function start!");
    uint32_t code = -100;
    g_data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor());
    int32_t ret = g_service->OnRemoteRequest(code, g_data, g_reply, g_option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR) << "ret: " << ret << " code: " << code;
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub003 function end!");
}

/**
 * @tc.name: BatterySrvStub004
 * @tc.desc: Test BatterySrvInterfaceCode SET_BATTERY_CONFIG
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub004 function start!");
    g_data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor());
    u16string sceneName = Str8ToStr16("BatterySrvStub004");
    u16string value = Str8ToStr16("0");
    g_data.WriteString16(sceneName);
    g_data.WriteString16(value);
    uint32_t code = static_cast<uint32_t>(PowerMgr::BatterySrvInterfaceCode::SET_BATTERY_CONFIG);
    int32_t ret = g_service->OnRemoteRequest(code, g_data, g_reply, g_option);
    EXPECT_EQ(ret, ERR_OK) << "ret: " << ret << " code: " << code;
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub004 function end!");
}

/**
 * @tc.name: BatterySrvStub005
 * @tc.desc: Test BatterySrvInterfaceCode GET_BATTERY_CONFIG
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub005 function start!");
    g_data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor());
    u16string sceneName = Str8ToStr16("BatterySrvStub005");
    g_data.WriteString16(sceneName);
    uint32_t code = static_cast<uint32_t>(PowerMgr::BatterySrvInterfaceCode::GET_BATTERY_CONFIG);
    int32_t ret = g_service->OnRemoteRequest(code, g_data, g_reply, g_option);
    EXPECT_EQ(ret, ERR_OK) << "ret: " << ret << " code: " << code;
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub005 function end!");
}

/**
 * @tc.name: BatterySrvStub006
 * @tc.desc: Test BatterySrvInterfaceCode SUPPORT_BATTERY_CONFIG
 * @tc.type: FUNC
 * @tc.require: issueI6KRS8
 */
static HWTEST_F(BatterySrvStubTest, BatterySrvStub006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub006 function start!");
    g_data.WriteInterfaceToken(BatterySrvProxy::GetDescriptor());
    u16string sceneName = Str8ToStr16("BatterySrvStub006");
    g_data.WriteString16(sceneName);
    uint32_t code = static_cast<uint32_t>(PowerMgr::BatterySrvInterfaceCode::SUPPORT_BATTERY_CONFIG);
    int32_t ret = g_service->OnRemoteRequest(code, g_data, g_reply, g_option);
    EXPECT_EQ(ret, ERR_OK) << "ret: " << ret << " code: " << code;
    BATTERY_HILOGI(LABEL_TEST, "BatterySrvStub006 function end!");
}
} // namespace

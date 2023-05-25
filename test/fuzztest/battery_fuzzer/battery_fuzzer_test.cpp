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

/* This files contains faultlog fuzzer test modules. */

#include "battery_fuzzer_test.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>

#include "battery_info.h"
#include "battery_service.h"
#include "battery_srv_client.h"
#include "message_parcel.h"
#include "securec.h"

using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

namespace {
auto& g_batterySrvClient = BatterySrvClient::GetInstance();
const int32_t REWIND_READ_DATA = 0;
} // namespace

static void TestGetCapacity([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetCapacity();
}

static void TestGetVoltage([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetVoltage();
}

static void TestGetChargingStatus([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetChargingStatus();
}

static void TestGetHealthStatus([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetHealthStatus();
}

static void TestGetPluggedType([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetPluggedType();
}

static void TestGetPresent([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetPresent();
}

static void TestGetTechnology([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetTechnology();
}

static void TestGetTemperature([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetBatteryTemperature();
}

static void TestGetNowCurrent([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetNowCurrent();
}

static void TestGetRemainEnergy([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetRemainEnergy();
}

static void TestGetTotalEnergy([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetTotalEnergy();
}

static void TestGetCapacityLevel([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetCapacityLevel();
}

static void TestGetRemainingChargeTime([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterySrvClient.GetRemainingChargeTime();
}

static void TestBatteryServiceStub(const uint8_t* data, size_t size)
{
    uint32_t code;
    if (size < sizeof(code)) {
        return;
    }
    if (memcpy_s(&code, sizeof(code), data, sizeof(code)) != EOK) {
        return;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(BatteryService::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(REWIND_READ_DATA);
    MessageParcel reply;
    MessageOption option;
    DelayedSpSingleton<BatteryService>::GetInstance()->OnRemoteRequest(code, datas, reply, option);
}

static std::vector<std::function<void(const uint8_t*, size_t)>> fuzzFunc = {
    &TestGetCapacity,
    &TestGetVoltage,
    &TestGetChargingStatus,
    &TestGetHealthStatus,
    &TestGetPluggedType,
    &TestGetPresent,
    &TestGetTechnology,
    &TestGetTemperature,
    &TestGetNowCurrent,
    &TestGetRemainEnergy,
    &TestGetTotalEnergy,
    &TestGetCapacityLevel,
    &TestGetRemainingChargeTime
};

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int32_t> randomNum(0, fuzzFunc.size() - 1);
    int32_t number = randomNum(engine);
    fuzzFunc[number](data, size);
    TestBatteryServiceStub(data, size);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

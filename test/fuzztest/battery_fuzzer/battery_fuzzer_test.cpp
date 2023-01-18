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

/* This files contains faultlog fuzzer test modules. */

#include "battery_fuzzer_test.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <algorithm>
#include "battery_info.h"
#include "battery_srv_client.h"

using namespace OHOS::PowerMgr;
using namespace std;

namespace {
auto& g_batterySrvClient = BatterySrvClient::GetInstance();
}

static void TestGetCapacity(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetCapacity();
}

static void TestGetVoltage(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetVoltage();
}

static void TestGetChargingStatus(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetChargingStatus();
}

static void TestGetHealthStatus(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetHealthStatus();
}

static void TestGetPluggedType(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetPluggedType();
}

static void TestGetPresent(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetPresent();
}

static void TestGetTechnology(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetTechnology();
}

static void TestGetTemperature(const uint8_t* data)
{
    if (data == nullptr) {
        return;
    }

    g_batterySrvClient.GetBatteryTemperature();
}

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    int32_t idSize = 8;
    if (static_cast<int32_t>(size) > idSize) {
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int32_t> randomNum(static_cast<int32_t>(ApiNumber::NUM_ZERO),
            static_cast<int32_t>(ApiNumber::NUM_END) - 1);
        int32_t number = randomNum(engine);

        switch (static_cast<ApiNumber>(number)) {
            case ApiNumber::NUM_ZERO:
                TestGetCapacity(data);
                break;
            case ApiNumber::NUM_ONE:
                TestGetVoltage(data);
                break;
            case ApiNumber::NUM_TWO:
                TestGetChargingStatus(data);
                break;
            case ApiNumber::NUM_THREE:
                TestGetHealthStatus(data);
                break;
            case ApiNumber::NUM_FOUR:
                TestGetPluggedType(data);
                break;
            case ApiNumber::NUM_FIVE:
                TestGetPresent(data);
                break;
            case ApiNumber::NUM_SIX:
                TestGetTechnology(data);
                break;
            case ApiNumber::NUM_SEVEN:
                TestGetTemperature(data);
                break;
            case ApiNumber::NUM_EIGHT:
                g_batterySrvClient.GetNowCurrent();
                break;
            case ApiNumber::NUM_NINE:
                g_batterySrvClient.GetRemainEnergy();
                break;
            case ApiNumber::NUM_TEN:
                g_batterySrvClient.GetTotalEnergy();
                break;
            case ApiNumber::NUM_ELEVEN:
                g_batterySrvClient.GetCapacityLevel();
                break;
            case ApiNumber::NUM_TWELVE:
                g_batterySrvClient.GetRemainingChargeTime();
                break;
            default:
                break;
        }
    }
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

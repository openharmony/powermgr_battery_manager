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

/* This files contains faultlog fuzzer test modules. */

#define FUZZ_PROJECT_NAME "batteryservice_fuzzer"

#include "battery_fuzzer_test.h"
#include "ibattery_srv.h"
#include "battery_service.h"

using namespace OHOS::PowerMgr;
using namespace OHOS;

namespace {
void TestBatteryService(const uint8_t* data, size_t size)
{
    sptr<BatteryService> g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->ChangePath("");
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-ls";
    args.push_back(arg);
    g_service->Dump(fd, args);
    g_service->MockUnplugged();
    g_service->IsMockUnplugged();
    int32_t capacity = 100;
    g_service->MockCapacity(capacity);
    g_service->IsMockCapacity();
    g_service->MockUevent("");
    g_service->Reset();
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    TestBatteryService(data, size);
    return 0;
}

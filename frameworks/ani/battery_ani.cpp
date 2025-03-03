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

#include <ani.h>
#include <array>
#include <iostream>
#include "battery_srv_client.h"
#include "battery_log.h"

using namespace OHOS::PowerMgr;

thread_local static BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();
static ani_int GetBatterySOC([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
{
    int32_t capacity = g_battClient.GetCapacity();
    BATTERY_HILOGD(FEATURE_BATT_INFO, "capacity= %{public}d", capacity);
    return capacity;
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    BATTERY_HILOGI(FEATURE_BATT_INFO, "ANI_Constructor");
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "GetEnv failed");
        return ANI_ERROR;
    }

    static const char *className = "L@ohos.batteryInfo/batteryInfo;";
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "Not found %{public}s", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"getBatterySOC", ":I", reinterpret_cast<void *>(GetBatterySOC)},
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        BATTERY_HILOGE(FEATURE_BATT_INFO, "Cannot bind native methods to %{public}s", className);
        return ANI_ERROR;
    };
    BATTERY_HILOGI(FEATURE_BATT_INFO, "ANI_Constructor finish");

    *result = ANI_VERSION_1;
    return ANI_OK;
}
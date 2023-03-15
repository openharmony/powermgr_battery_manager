/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <cstdio>
#include <string>
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "napi/native_common.h"
#include "napi/native_api.h"
#include "battery_info.h"
#include "battery_log.h"

using namespace OHOS::PowerMgr;

static napi_value EnumChargeTypeClassConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void* data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static napi_value CreateEnumChargeType(napi_env env, napi_value exports)
{
    napi_value none = nullptr;
    napi_value wired_normal = nullptr;
    napi_value wired_quick = nullptr;
    napi_value wired_super_quick = nullptr;
    napi_value wireless_normal = nullptr;
    napi_value wireless_quick = nullptr;
    napi_value wireless_super_quick = nullptr;

    napi_create_int32(env, (int32_t)ChargeType::NONE, &none);
    napi_create_int32(env, (int32_t)ChargeType::WIRED_NORMAL, &wired_normal);
    napi_create_int32(env, (int32_t)ChargeType::WIRED_QUICK, &wired_quick);
    napi_create_int32(env, (int32_t)ChargeType::WIRED_SUPER_QUICK, &wired_super_quick);
    napi_create_int32(env, (int32_t)ChargeType::WIRELESS_NORMAL, &wireless_normal);
    napi_create_int32(env, (int32_t)ChargeType::WIRELESS_QUICK, &wireless_quick);
    napi_create_int32(env, (int32_t)ChargeType::WIRELESS_SUPER_QUICK, &wireless_super_quick);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NONE", none),
        DECLARE_NAPI_STATIC_PROPERTY("WIRED_NORMAL", wired_normal),
        DECLARE_NAPI_STATIC_PROPERTY("WIRED_QUICK", wired_quick),
        DECLARE_NAPI_STATIC_PROPERTY("WIRED_SUPER_QUICK", wired_super_quick),
        DECLARE_NAPI_STATIC_PROPERTY("WIRELESS_NORMAL", wireless_normal),
        DECLARE_NAPI_STATIC_PROPERTY("WIRELESS_QUICK", wireless_quick),
        DECLARE_NAPI_STATIC_PROPERTY("WIRELESS_SUPER_QUICK", wireless_super_quick),
    };

    napi_value result = nullptr;
    napi_define_class(env, "ChargeType", NAPI_AUTO_LENGTH, EnumChargeTypeClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "ChargeType", result);

    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value ChargeTypeInit(napi_env env, napi_value exports)
{
    BATTERY_HILOGD(COMP_FWK, "Enter");
    CreateEnumChargeType(env, exports);

    BATTERY_HILOGD(COMP_FWK, "Success");
    return exports;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "charger",
    .nm_register_func = ChargeTypeInit,
    .nm_modname = "charger",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}
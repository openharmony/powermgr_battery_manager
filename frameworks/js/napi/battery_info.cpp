/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include "battery_info.h"
#include "battery_srv_client.h"
#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::PowerMgr;

static BatterySrvClient &g_battClient = BatterySrvClient::GetInstance();

static napi_value BatterySOC(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t capacity = g_battClient.GetCapacity();

    NAPI_CALL(env, napi_create_int32(env, capacity, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "capacity %{public}d", capacity);
    return napiValue;
}

static napi_value GetChargingState(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t chargeState = (int32_t)g_battClient.GetChargingStatus();

    NAPI_CALL(env, napi_create_int32(env, chargeState, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "chargeState %{public}d", chargeState);

    return napiValue;
}

static napi_value GetHealthState(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t healthStatus = (int32_t)g_battClient.GetHealthStatus();

    NAPI_CALL(env, napi_create_int32(env, healthStatus, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "healthStatus %{public}d", healthStatus);

    return napiValue;
}

static napi_value GetPluggedType(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t pluggedType = (int32_t)g_battClient.GetPluggedType();

    NAPI_CALL(env, napi_create_int32(env, pluggedType, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "pluggedType %{public}d", pluggedType);

    return napiValue;
}

static napi_value GetVoltage(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t voltage = g_battClient.GetVoltage();

    NAPI_CALL(env, napi_create_int32(env, voltage, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "%{public}d", voltage);

    return napiValue;
}

static napi_value GetTechnology(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    auto technology = g_battClient.GetTechnology();
    const char *technologyStr = technology.c_str();

    NAPI_CALL(env, napi_create_string_utf8(env, technologyStr, strlen(technologyStr), &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "technology %{public}s", technologyStr);
    return napiValue;
}

static napi_value GetBatteryTemperature(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    int32_t temperature = g_battClient.GetBatteryTemperature();

    NAPI_CALL(env, napi_create_int32(env, temperature, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "%{public}d", temperature);

    return napiValue;
}

static napi_value GetBatteryPresent(napi_env env, napi_callback_info info)
{
    napi_value napiValue = nullptr;
    bool present = g_battClient.GetPresent();

    NAPI_CALL(env, napi_get_boolean(env, present, &napiValue));

    POWER_HILOGD(MODULE_JS_NAPI, "%{public}d", present);

    return napiValue;
}

static napi_value EnumHealthClassConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static napi_value CreateEnumHealthState(napi_env env, napi_value exports)
{
    napi_value unknown = nullptr;
    napi_value good = nullptr;
    napi_value overheat = nullptr;
    napi_value overvoltage = nullptr;
    napi_value cold = nullptr;
    napi_value dead = nullptr;

    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_UNKNOWN, &unknown);
    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_GOOD, &good);
    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_OVERHEAT, &overheat);
    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_OVERVOLTAGE, &overvoltage);
    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_COLD, &cold);
    napi_create_int32(env, (int32_t)BatteryHealthState::HEALTH_STATE_DEAD, &dead);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("UNKNOWN", unknown),
        DECLARE_NAPI_STATIC_PROPERTY("GOOD", good),
        DECLARE_NAPI_STATIC_PROPERTY("OVERHEAT", overheat),
        DECLARE_NAPI_STATIC_PROPERTY("OVERVOLTAGE", overvoltage),
        DECLARE_NAPI_STATIC_PROPERTY("COLD", cold),
        DECLARE_NAPI_STATIC_PROPERTY("DEAD", dead),
    };
    napi_value result = nullptr;
    napi_define_class(env, "BatteryHealthState", NAPI_AUTO_LENGTH, EnumHealthClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "BatteryHealthState", result);

    return exports;
}

static napi_value EnumChargeClassConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static napi_value CreateEnumChargeState(napi_env env, napi_value exports)
{
    napi_value none = nullptr;
    napi_value enable = nullptr;
    napi_value disable = nullptr;
    napi_value full = nullptr;

    napi_create_int32(env, (int32_t)BatteryChargeState::CHARGE_STATE_NONE, &none);
    napi_create_int32(env, (int32_t)BatteryChargeState::CHARGE_STATE_ENABLE, &enable);
    napi_create_int32(env, (int32_t)BatteryChargeState::CHARGE_STATE_DISABLE, &disable);
    napi_create_int32(env, (int32_t)BatteryChargeState::CHARGE_STATE_FULL, &full);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NONE", none),
        DECLARE_NAPI_STATIC_PROPERTY("ENABLE", enable),
        DECLARE_NAPI_STATIC_PROPERTY("DISABLE", disable),
        DECLARE_NAPI_STATIC_PROPERTY("FULL", full),
    };

    napi_value result = nullptr;
    napi_define_class(env, "BatteryChargeState", NAPI_AUTO_LENGTH, EnumChargeClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "BatteryChargeState", result);

    return exports;
}

static napi_value EnumPluggedClassConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static napi_value CreateEnumPluggedType(napi_env env, napi_value exports)
{
    napi_value none = nullptr;
    napi_value ac = nullptr;
    napi_value usb = nullptr;
    napi_value wireless = nullptr;

    napi_create_int32(env, (int32_t)BatteryPluggedType::PLUGGED_TYPE_NONE, &none);
    napi_create_int32(env, (int32_t)BatteryPluggedType::PLUGGED_TYPE_AC, &ac);
    napi_create_int32(env, (int32_t)BatteryPluggedType::PLUGGED_TYPE_USB, &usb);
    napi_create_int32(env, (int32_t)BatteryPluggedType::PLUGGED_TYPE_WIRELESS, &wireless);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NONE", none),
        DECLARE_NAPI_STATIC_PROPERTY("AC", ac),
        DECLARE_NAPI_STATIC_PROPERTY("USB", usb),
        DECLARE_NAPI_STATIC_PROPERTY("WIRELESS", wireless),
    };

    napi_value result = nullptr;
    napi_define_class(env, "BatteryPluggedType", NAPI_AUTO_LENGTH, EnumPluggedClassConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);

    napi_set_named_property(env, exports, "BatteryPluggedType", result);

    return exports;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value BatteryInit(napi_env env, napi_value exports)
{
    POWER_HILOGD(MODULE_JS_NAPI, "enter");

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_GETTER("batterySOC", BatterySOC),
        DECLARE_NAPI_GETTER("chargingStatus", GetChargingState),
        DECLARE_NAPI_GETTER("healthStatus", GetHealthState),
        DECLARE_NAPI_GETTER("pluggedType", GetPluggedType),
        DECLARE_NAPI_GETTER("voltage", GetVoltage),
        DECLARE_NAPI_GETTER("technology", GetTechnology),
        DECLARE_NAPI_GETTER("batteryTemperature", GetBatteryTemperature),
        DECLARE_NAPI_GETTER("isBatteryPresent", GetBatteryPresent),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    CreateEnumPluggedType(env, exports);
    CreateEnumChargeState(env, exports);
    CreateEnumHealthState(env, exports);

    POWER_HILOGD(MODULE_JS_NAPI, "return");

    return exports;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "batteryInfo",
    .nm_register_func = BatteryInit,
    .nm_modname = "batteryInfo",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * Module registration
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&g_module);
}

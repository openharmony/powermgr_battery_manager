/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "system_battery.h"

#include <string>

#include "battery_log.h"
#include "battery_srv_client.h"
#include "power_common.h"

using namespace OHOS::PowerMgr;
using namespace OHOS::HDI::Battery;

namespace {
const uint32_t MAX_ARGC = 1;
const uint32_t ARGC_ONE = 0;
const double LEVEL_RANGES = 0.01;

const int32_t GET_ERROR_CODE = -1;
const int32_t CALLBACK_ERROR_CODE = -2;
const int32_t RESPONSE_ERROR_CODE = -3;
const int32_t OPTIONS_ERROR_CODE = -4;

const std::string RESPONSE_CHARGING = "charging";
const std::string RESPONSE_LEVEL = "level";
const std::string FUNC_SUCEESS_NAME = "success";
const std::string FUNC_FAIL_NAME = "fail";
const std::string FUNC_COMPLETE_NAME = "complete";
}

void SystemBattery::GetBatteryStats(napi_env env, napi_value options)
{
    RETURN_IF(!CheckValueType(env, options, napi_object));

    batteryInfo_.GetBatteryInfo(error_);
    if (batteryInfo_.DataValid()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js success method");
        SuccessCallback(env, options);
    }
    if (error_.IsError()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js fail method");
        FailCallback(env, options);
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js complete method");
    CompleteCallback(env, options);
}

napi_value SystemBattery::CreateResponse(napi_env env)
{
    napi_value level = nullptr;
    napi_value charging = nullptr;
    NAPI_CALL(env, napi_create_double(env, batteryInfo_.GetLevel(), &level));
    NAPI_CALL(env, napi_create_uint32(env, batteryInfo_.IsCharging(), &charging));

    napi_value response = nullptr;
    NAPI_CALL(env, napi_create_object(env, &response));
    NAPI_CALL(env, napi_set_named_property(env, response, RESPONSE_CHARGING.c_str(), charging));
    NAPI_CALL(env, napi_set_named_property(env, response, RESPONSE_LEVEL.c_str(), level));
    return response;
}

bool SystemBattery::CheckValueType(napi_env env, napi_value value, napi_valuetype checkType)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != checkType) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Check input parameter error");
        return false;
    }
    return true;
}

napi_value SystemBattery::GetOptionsFunc(napi_env env, napi_value options, const std::string& name)
{
    napi_value property = nullptr;
    napi_status status = napi_get_named_property(env, options, name.c_str(), &property);
    if (status != napi_ok) {
        std::string msg = "Failed to obtain " + name;
        error_.SetErrorMsg(OPTIONS_ERROR_CODE, msg);
        return nullptr;
    }
    if (!CheckValueType(env, property, napi_function)) {
        std::string msg = "The " + name + " argument is not a function";
        error_.SetErrorMsg(OPTIONS_ERROR_CODE, msg);
        return nullptr;
    }
    return property;
}

void SystemBattery::SuccessCallback(napi_env env, napi_value options)
{
    napi_value success = GetOptionsFunc(env, options, FUNC_SUCEESS_NAME);
    RETURN_IF(success == nullptr);
    napi_value argv = nullptr;

    argv = CreateResponse(env);
    if (argv == nullptr) {
        std::string msg = "Failed to create BatteryResponse";
        error_.SetErrorMsg(RESPONSE_ERROR_CODE, msg);
        return;
    }

    napi_value callResult = 0;
    const size_t argc = 1;
    napi_status status = napi_call_function(env, nullptr, success, argc, &argv, &callResult);
    if (status != napi_ok) {
        std::string msg = "Failed to execute the callback function SUCCESS";
        error_.SetErrorMsg(CALLBACK_ERROR_CODE, msg);
        return;
    }
    BATTERY_HILOGI(FEATURE_BATT_INFO, "Callbacks to incoming parameters, level: %{public}f, isCharging: %{public}d",
        batteryInfo_.GetLevel(), batteryInfo_.IsCharging());
}

void SystemBattery::FailCallback(napi_env env, napi_value options)
{
    napi_value fail = GetOptionsFunc(env, options, FUNC_FAIL_NAME);
    RETURN_IF(fail == nullptr);

    napi_value data = nullptr;
    napi_value code = nullptr;
    std::string msg = error_.GetMsg();
    napi_status strStatus = napi_create_string_utf8(env, msg.c_str(), msg.size(), &data);
    napi_status intStatus = napi_create_int32(env, error_.GetCode(), &code);
    if (strStatus != napi_ok || intStatus != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Napi failed to create a parameter, code: %{public}d, msg: %{public}s",
            error_.GetCode(), msg.c_str());
        return;
    }

    napi_value argv[] = { data, code };
    napi_value callResult = 0;
    const size_t argc = 2;
    napi_status status = napi_call_function(env, nullptr, fail, argc, argv, &callResult);
    if (status != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Call fail function is failed");
    }
}

void SystemBattery::CompleteCallback(napi_env env, napi_value options)
{
    napi_value complete = GetOptionsFunc(env, options, FUNC_COMPLETE_NAME);
    RETURN_IF(complete == nullptr);
    napi_value callResult = 0;
    const size_t argc = 0;
    napi_status status = napi_call_function(env, nullptr, complete, argc, nullptr, &callResult);
    if (status != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Call complete function is failed");
    }
}

void SystemBattery::Error::SetErrorMsg(int32_t code, std::string& msg)
{
    code_ = code;
    msg_ = msg;
    BATTERY_HILOGW(FEATURE_BATT_INFO, "Error message, code: %{public}d, msg: %{public}s",
        code_, msg_.c_str());
}

void SystemBattery::BatteryInfo::GetBatteryInfo(Error& error)
{
    BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();
    capacity_ = g_battClient.GetCapacity();
    chargingState_ = g_battClient.GetChargingStatus();
    BATTERY_HILOGI(FEATURE_BATT_INFO, "Get battery info, capacity: %{public}d, charging: %{public}d",
        capacity_, static_cast<int32_t>(chargingState_));

    if (!DataValid()) {
        std::string msg = "The current charging status and quantity information cannot be obtained";
        error.SetErrorMsg(GET_ERROR_CODE, msg);
    }
}

bool SystemBattery::BatteryInfo::DataValid()
{
    return (capacity_ != INVALID_BATT_INT_VALUE) && (chargingState_ != BatteryChargeState::CHARGE_STATE_BUTT);
}

double SystemBattery::BatteryInfo::GetLevel()
{
    // Current battery level, which ranges from 0.00 to 1.00.
    return (capacity_ * LEVEL_RANGES);
}

uint32_t SystemBattery::BatteryInfo::IsCharging()
{
    return static_cast<uint32_t>(chargingState_ == BatteryChargeState::CHARGE_STATE_ENABLE);
}

static napi_value GetStatus(napi_env env, napi_callback_info info)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the GetBatteryStats method");
    size_t argc = MAX_ARGC;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));

    SystemBattery sysBatt;
    if (argc != MAX_ARGC) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Lack of parameter, argc: %{public}zu", argc);
        return nullptr;
    }

    sysBatt.GetBatteryStats(env, argv[ARGC_ONE]);
    return nullptr;
}

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value SystemBatteryInit(napi_env env, napi_value exports)
{
    BATTERY_HILOGI(COMP_FWK, "SystemBattery init");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getStatus", GetStatus)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    BATTERY_HILOGI(COMP_FWK, "SystemBattery init end");
    return exports;
}
EXTERN_C_END

/*
 * Module definition
 */
static napi_module g_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = "battery",
    .nm_register_func = SystemBatteryInit,
    .nm_modname = "battery",
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

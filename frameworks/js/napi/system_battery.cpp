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
#include <cstddef>
#include <memory>
#include "new"
#include "napi/native_common.h"

#include "battery_log.h"
#include "battery_srv_client.h"
#include "power_common.h"

using namespace OHOS::PowerMgr;

namespace {
const uint32_t MAX_ARGC = 1;
const uint32_t ARGC_ONE = 0;
const double LEVEL_RANGES = 0.01;

const std::string RESPONSE_CHARGING = "charging";
const std::string RESPONSE_LEVEL = "level";
const std::string FUNC_SUCEESS_NAME = "success";
const std::string FUNC_FAIL_NAME = "fail";
const std::string FUNC_COMPLETE_NAME = "complete";

const uint32_t COMMON_ERROR_COED = 200;
const std::string GET_BATTERY_ERROR_MSG = "Battery level is not available";
}

void SystemBattery::GetBatteryStats(napi_env env)
{
    if (!batteryInfo_.GetBatteryInfo()) {
        error_.SetErrorMsg(COMMON_ERROR_COED, GET_BATTERY_ERROR_MSG);
    }
    if (!error_.IsError()) {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js success method");
        SuccessCallback(env);
    } else {
        BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js fail method");
        FailCallback(env);
    }
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the js complete method");
    CompleteCallback(env);
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
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Failed to obtain %{public}s", name.c_str());
        return nullptr;
    }
    if (!CheckValueType(env, property, napi_function)) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "The %{public}s argument is not a function", name.c_str());
        return nullptr;
    }
    return property;
}

bool SystemBattery::CreateCallbackRef(napi_env env, napi_value options)
{
    RETURN_IF_WITH_RET(!CheckValueType(env, options, napi_object), false);

    napi_value succCallBack = GetOptionsFunc(env, options, FUNC_SUCEESS_NAME);
    if (succCallBack != nullptr) {
        napi_create_reference(env, succCallBack, 1, &successRef_);
    }

    napi_value failCallBack = GetOptionsFunc(env, options, FUNC_FAIL_NAME);
    if (failCallBack != nullptr) {
        napi_create_reference(env, failCallBack, 1, &failRef_);
    }

    napi_value completeCallBack = GetOptionsFunc(env, options, FUNC_COMPLETE_NAME);
    if (completeCallBack != nullptr) {
        napi_create_reference(env, completeCallBack, 1, &completeRef_);
    }
    return true;
}

void SystemBattery::SuccessCallback(napi_env env)
{
    RETURN_IF(successRef_ == nullptr);
    napi_value success = nullptr;
    napi_get_reference_value(env, successRef_, &success);
    napi_value argv = nullptr;
    argv = CreateResponse(env);
    if (argv == nullptr) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Failed to create BatteryResponse");
        napi_delete_reference(env, successRef_);
        return;
    }

    napi_value callResult = 0;
    const size_t argc = 1;
    napi_status status = napi_call_function(env, nullptr, success, argc, &argv, &callResult);
    napi_delete_reference(env, successRef_);
    if (status != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Failed to execute the callback function SUCCESS");
        return;
    }
    BATTERY_HILOGI(FEATURE_BATT_INFO, "Callbacks to incoming parameters, level: %{public}f, isCharging: %{public}d",
        batteryInfo_.GetLevel(), batteryInfo_.IsCharging());
}

void SystemBattery::FailCallback(napi_env env)
{
    RETURN_IF(failRef_ == nullptr);
    napi_value fail = nullptr;
    napi_get_reference_value(env, failRef_, &fail);

    napi_value data = nullptr;
    napi_value code = nullptr;
    std::string msg = error_.GetMsg();
    napi_status strStatus = napi_create_string_utf8(env, msg.c_str(), msg.size(), &data);
    napi_status intStatus = napi_create_int32(env, error_.GetCode(), &code);
    if (strStatus != napi_ok || intStatus != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Napi failed to create a parameter, code: %{public}d, msg: %{public}s",
            error_.GetCode(), msg.c_str());
        napi_delete_reference(env, failRef_);
        return;
    }

    napi_value argv[] = { data, code };
    napi_value callResult = 0;
    const size_t argc = 2;
    napi_status status = napi_call_function(env, nullptr, fail, argc, argv, &callResult);
    if (status != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Call fail function is failed");
    }
    napi_delete_reference(env, failRef_);
}

void SystemBattery::CompleteCallback(napi_env env)
{
    RETURN_IF(completeRef_ == nullptr);
    napi_value complete = nullptr;
    napi_get_reference_value(env, completeRef_, &complete);
    napi_value callResult = 0;
    const size_t argc = 0;
    napi_status status = napi_call_function(env, nullptr, complete, argc, nullptr, &callResult);
    if (status != napi_ok) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Call complete function is failed");
    }
    napi_delete_reference(env, completeRef_);
}

void SystemBattery::Error::SetErrorMsg(int32_t code, const std::string& msg)
{
    code_ = code;
    msg_ = msg;
    BATTERY_HILOGW(FEATURE_BATT_INFO, "Error message, code: %{public}d, msg: %{public}s",
        code_, msg_.c_str());
}

bool SystemBattery::BatteryInfo::GetBatteryInfo()
{
    BatterySrvClient& g_battClient = BatterySrvClient::GetInstance();
    capacity_ = g_battClient.GetCapacity();
    chargingState_ = g_battClient.GetChargingStatus();
    BATTERY_HILOGI(FEATURE_BATT_INFO, "Get battery info, capacity: %{public}d, charging: %{public}d",
        capacity_, static_cast<int32_t>(chargingState_));
    return (capacity_ != INVALID_BATT_INT_VALUE) && (chargingState_ != BatteryChargeState::CHARGE_STATE_BUTT);
}

double SystemBattery::BatteryInfo::GetLevel() const
{
    // Current battery level, which ranges from 0.00 to 1.00.
    return (capacity_ * LEVEL_RANGES);
}

uint32_t SystemBattery::BatteryInfo::IsCharging() const
{
    return static_cast<uint32_t>(chargingState_ == BatteryChargeState::CHARGE_STATE_ENABLE ||
        chargingState_ == BatteryChargeState::CHARGE_STATE_FULL);
}

static napi_value GetStatus(napi_env env, napi_callback_info info)
{
    BATTERY_HILOGD(FEATURE_BATT_INFO, "Call the GetBatteryStats method");
    size_t argc = MAX_ARGC;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));

    if (argc != MAX_ARGC) {
        BATTERY_HILOGW(FEATURE_BATT_INFO, "Lack of parameter, argc: %{public}zu", argc);
        return nullptr;
    }

    std::unique_ptr<SystemBattery> asyncInfo = std::make_unique<SystemBattery>();
    RETURN_IF_WITH_RET(!asyncInfo->CreateCallbackRef(env, argv[ARGC_ONE]), nullptr);

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "GetStatus", NAPI_AUTO_LENGTH, &resource));
    napi_create_async_work(
        env,
        nullptr,
        resource,
        [](napi_env env, void *data) {},
        [](napi_env env, napi_status status, void *data) {
            SystemBattery *asyncInfo = (SystemBattery*)data;
            if (asyncInfo != nullptr) {
                asyncInfo->GetBatteryStats(env);
                napi_delete_async_work(env, asyncInfo->asyncWork);
                delete asyncInfo;
            }
        },
        (void*)asyncInfo.get(),
        &asyncInfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asyncInfo->asyncWork));
    asyncInfo.release();
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

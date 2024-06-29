/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_error.h"

#include "power_common.h"
#include "battery_log.h"

namespace OHOS {
namespace PowerMgr {
std::map<BatteryError, std::string> NapiError::errorTable_ = {
    {BatteryError::ERR_CONNECTION_FAIL,   "Connecting to the service failed."},
    {BatteryError::ERR_PERMISSION_DENIED, "Permission is denied"             },
    {BatteryError::ERR_SYSTEM_API_DENIED, "System permission is denied"      },
    {BatteryError::ERR_PARAM_INVALID,     "Invalid input parameter."         }
};

napi_value NapiError::GetNapiError(napi_env& env) const
{
    napi_value napiError;
    if (!IsError()) {
        napi_get_undefined(env, &napiError);
        return napiError;
    }

    std::string msg;
    auto item = errorTable_.find(code_);
    if (item != errorTable_.end()) {
        msg = item->second;
    }
    napi_value napiMsg;
    NAPI_CALL(env, napi_create_string_utf8(env, msg.c_str(), msg.size(), &napiMsg));
    NAPI_CALL(env, napi_create_error(env, nullptr, napiMsg, &napiError));

    napi_value napiCode;
    NAPI_CALL(env, napi_create_int32(env, static_cast<int32_t>(code_), &napiCode));

    napi_set_named_property(env, napiError, "code", napiCode);
    napi_set_named_property(env, napiError, "message", napiMsg);

    BATTERY_HILOGW(COMP_FWK, "throw error code: %{public}d, msg: %{public}s,",
        static_cast<int32_t>(code_), msg.c_str());
    return napiError;
}

napi_value NapiError::ThrowError(napi_env& env, BatteryError code)
{
    Error(code);
    napi_value error = GetNapiError(env);
    RETURN_IF_WITH_RET(error == nullptr, nullptr);
    napi_throw(env, error);
    return nullptr;
}
} // namespace PowerMgr
} // namespace OHOS

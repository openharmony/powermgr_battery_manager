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

#ifndef SYSTEM_BATTERY_H
#define SYSTEM_BATTERY_H

#include <cstdint>
#include <iosfwd>
#include "errors.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class SystemBattery {
public:
    void GetBatteryStats(napi_env env);
    bool CheckValueType(napi_env env, napi_value value, napi_valuetype checkType);
    bool CreateCallbackRef(napi_env env, napi_value options);

    napi_async_work asyncWork = nullptr;
private:
    class Error {
    public:
        void SetErrorMsg(int32_t code, const std::string& msg);
        inline bool IsError() const
        {
            return !msg_.empty() && (code_ != ERR_OK);
        }
        inline int32_t GetCode() const
        {
            return code_;
        }
        inline std::string GetMsg() const
        {
            return msg_;
        }
    private:
        int32_t code_ { ERR_OK };
        std::string msg_;
    };

    class BatteryInfo {
    public:
        bool GetBatteryInfo();
        double GetLevel() const;
        uint32_t IsCharging() const;

    private:
        int32_t capacity_ { INVALID_BATT_INT_VALUE };
        BatteryChargeState chargingState_;
    };

    napi_value GetOptionsFunc(napi_env env, napi_value options, const std::string& name);
    napi_value CreateResponse(napi_env env);
    void SuccessCallback(napi_env env);
    void FailCallback(napi_env env);
    void CompleteCallback(napi_env env);

    Error error_;
    BatteryInfo batteryInfo_;
    napi_ref successRef_ = nullptr;
    napi_ref failRef_ = nullptr;
    napi_ref completeRef_ = nullptr;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // SYSTEM_BATTERY_H

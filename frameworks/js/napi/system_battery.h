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

#include "errors.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "battery_info.h"

namespace OHOS {
namespace PowerMgr {
class SystemBattery {
public:
    void GetBatteryStats(napi_env env, napi_value options);

private:
    class Error {
    public:
        void SetErrorMsg(int32_t code, std::string& msg);
        inline bool IsError()
        {
            return !msg_.empty() && (code_ != ERR_OK);
        }
        inline int32_t GetCode()
        {
            return code_;
        }
        inline std::string GetMsg()
        {
            return msg_;
        }
    private:
        int32_t code_ { ERR_OK };
        std::string msg_;
    };

    class BatteryInfo {
    public:
        void GetBatteryInfo(Error& error);
        bool DataValid();
        double GetLevel();
        uint32_t IsCharging();

    private:
        int32_t capacity_ { INVALID_BATT_INT_VALUE };
        BatteryChargeState chargingState_;
    };

    napi_value CreateResponse(napi_env env);
    bool CheckValueType(napi_env env, napi_value value, napi_valuetype checkType);
    napi_value GetOptionsFunc(napi_env env, napi_value options, const std::string& name);
    void SuccessCallback(napi_env env, napi_value options);
    void FailCallback(napi_env env, napi_value options);
    void CompleteCallback(napi_env env, napi_value options);

    Error error_;
    BatteryInfo batteryInfo_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // SYSTEM_BATTERY_H

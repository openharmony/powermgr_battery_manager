/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef CHARGER_UI_LOG_H__
#define CHARGER_UI_LOG_H__

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
enum class LogInfo {
    VERBOSE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
};

enum class UpdateState {
    UPDATE_STAGE_BEGIN,
    UPDATE_STAGE_SUCCESS,
    UPDATE_STAGE_FAIL,
    UPDATE_STAGE_OUT,
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif /* CHARGER_UI_LOG_H__ */

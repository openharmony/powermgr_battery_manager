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

#ifndef CHARGER_UI_FAULT_CODE_H
#define CHARGER_UI_FAULT_CODE_H

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
enum UpdaterErrorCode {
    CODE_VERIFY_FAIL = 1000,
    CODE_MOUNT_FAIL,
    CODE_UMOUNT_FAIL,
    CODE_DECOMPRESSION_FAIL,
    CODE_COMPRESS_FAIL,
    CODE_FORK_FAIL,
    CODE_ANALYSIS_SCRIPT_FAIL,
    CODE_MEMORY_FAIL,
    CODE_MISC_OP_FAIL,
    CODE_FACTORY_RESET_FAIL,
    CODE_INCREMENT_UPDATER_FAIL,
    CODE_FULL_UPDATER_FAIL,
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_FAULT_CODE_H

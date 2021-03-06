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

#include "charger_thread.h"

#include "hdf_log.h"

int main(int argc, char **argv)
{
    HDF_LOGI("%{public}s enter", __func__);
    using namespace OHOS::HDI::Battery::V1_0;
    std::unique_ptr<ChargerThread> chargerThread = std::make_unique<ChargerThread>();
    if (chargerThread == nullptr) {
        HDF_LOGE("%{public}s: instantiate loop error", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    chargerThread->StartThread(nullptr);

    HDF_LOGI("%{public}s exit", __func__);
    return 0;
}

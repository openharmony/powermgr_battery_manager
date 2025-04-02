/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef OHOS_BATTERY_MANAGER_MEMORY_GUARD_H
#define OHOS_BATTERY_MANAGER_MEMORY_GUARD_H
#include "nocopyable.h"
#include "battery_log.h"
#include "malloc.h"
namespace OHOS {
namespace PowerMgr {
class MemoryGuard {
public:
    DISALLOW_COPY_AND_MOVE(MemoryGuard);
    MemoryGuard()
    {
        int setCache = mallopt(M_SET_THREAD_CACHE, M_THREAD_CACHE_DISABLE);
        int setFree = mallopt(M_DELAYED_FREE, M_DELAYED_FREE_DISABLE);
        if (setCache != 1 || setFree != 1) {
            BATTERY_HILOGD(
                COMP_SVC, "disable tcache and delay free, result[%{public}d, %{public}d]", setCache, setFree);
        }
    }
    ~MemoryGuard()
    {
        int err = mallopt(M_FLUSH_THREAD_CACHE, 0);
        if (err != 1) {
            BATTERY_HILOGD(COMP_SVC, "flush cache, result: %{public}d", err);
        }
    }
};
} // namespace PowerMgr
} // namespace OHOS
#endif
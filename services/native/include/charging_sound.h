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
#ifndef POWERMGR_BATTERY_MANAGER_CHARGING_SOUND_H
#define POWERMGR_BATTERY_MANAGER_CHARGING_SOUND_H
#include <string>
#include "nocopyable.h"
namespace OHOS {
namespace Media {
class AudioHapticSound;
} // namespace Media

namespace PowerMgr {
class ChargingSoundCallBack;
class ChargingSound {
public:
    static ChargingSound& GetInstance();
    void Start() const;
    void Stop() const;
    void Prepare() const;

private:
    ChargingSound();
    DISALLOW_COPY_AND_MOVE(ChargingSound);
    std::string GetPath(const char* uri) const;
    std::string uri_;
    std::shared_ptr<Media::AudioHapticSound> sound_ {};
    std::shared_ptr<ChargingSoundCallBack> callback_ {};
};
} // namespace PowerMgr
} // namespace OHOS
#endif
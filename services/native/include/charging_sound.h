/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include <atomic>
#include <memory>
#include <string>
#include "nocopyable.h"
namespace OHOS {
namespace Media {
class Player;
} // namespace Media

namespace PowerMgr {
class ChargingSound {
public:
    ChargingSound();
    ~ChargingSound();
    bool Play();
    void Release();
    void Stop();
    bool IsPlaying();
    bool ReleaseClientListener();
    // single instance for now
    static bool IsPlayingGlobal();
    static bool PlayGlobal();
    static bool ReleaseGlobal();

private:
    DISALLOW_COPY_AND_MOVE(ChargingSound);
    std::string GetPath(const char* uri) const;
    std::string uri_;
    std::shared_ptr<Media::Player> player_ {};
    std::atomic<bool> isPlaying_ {false};
    static std::shared_ptr<ChargingSound> instance_;
};

// export apis
extern "C" {
    __attribute__ ((visibility ("default"))) bool ChargingSoundStart(void);
    __attribute__ ((visibility ("default"))) bool IsPlaying(void);
    __attribute__ ((visibility ("default"))) bool ChargingSoundRelease(void);
}
} // namespace PowerMgr
} // namespace OHOS
#endif
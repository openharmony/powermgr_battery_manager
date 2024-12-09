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
#include "charging_sound.h"

#include "battery_log.h"
#include "audio_haptic_sound.h"
#include "config_policy_utils.h"
#include "errors.h"

namespace OHOS {
namespace PowerMgr {
const std::string CHARGER_SOUND_DEFAULT_PATH = "/vendor/etc/battery/PowerConnected.ogg";
const char* CHARGER_SOUND_RELATIVE_PATH = "resource/media/audio/ui/PowerConnected.ogg";

ChargingSound& ChargingSound::GetInstance()
{
    static ChargingSound instance;
    return instance;
}

std::string ChargingSound::GetPath(const char* uri) const
{
    std::string ret {};
    char buf[MAX_PATH_LEN] = {0};
    char* path = GetOneCfgFile(uri, buf, MAX_PATH_LEN);
    if (path) {
        ret = path;
    }
    return ret;
}

ChargingSound::ChargingSound()
{
    uri_ = GetPath(CHARGER_SOUND_RELATIVE_PATH);
    if (uri_.empty()) {
        BATTERY_HILOGE(COMP_SVC, "get sound path failed, using fallback path");
        uri_ = CHARGER_SOUND_DEFAULT_PATH;
    }
    sound_ = Media::AudioHapticSound::CreateAudioHapticSound(
        Media::AUDIO_LATENCY_MODE_NORMAL, uri_, false, AudioStandard::STREAM_USAGE_SYSTEM);
}

void ChargingSound::Prepare() const
{
    if (!sound_) {
        BATTERY_HILOGE(COMP_SVC, "sound_ not created");
        return;
    }
    int32_t errcode = sound_->PrepareSound();
    if (errcode != ERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "prepare error, code: %{public}d", errcode);
    }
}

void ChargingSound::Start(bool retry) const
{
    if (!sound_) {
        BATTERY_HILOGE(COMP_SVC, "sound_ not created, retry=%{public}d", retry);
        return;
    }
    int32_t errcode = sound_->StartSound();
    if (errcode != ERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "start sound error, code: %{public}d, retry=%{pub;ic}d", errcode, retry);
        if (retry) {
            Prepare();
            Start(false);
        }
    }
}

void ChargingSound::Stop() const
{
    if (!sound_) {
        return;
    }
    (void)sound_->StopSound();
}
} // namespace PowerMgr
} // namespace OHOS
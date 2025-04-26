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
#include "charging_sound.h"

#include <dlfcn.h>
#include <mutex>
#include <securec.h>
#include "audio_stream_info.h"
#include "battery_log.h"
#include "config_policy_utils.h"
#include "errors.h"
#include "player.h"

namespace OHOS {
namespace PowerMgr {
// static non-local initializations
constexpr const char* CHARGER_SOUND_DEFAULT_PATH = "/vendor/etc/battery/PowerConnected.ogg";
constexpr const char* CHARGER_SOUND_RELATIVE_PATH = "resource/media/audio/ui/PowerConnected.ogg";
std::mutex g_playerPtrMutex;

// this static object is used as constructor/destructor, for now only one single instance is allowed.
std::shared_ptr<ChargingSound> ChargingSound::instance_ = std::make_shared<ChargingSound>();

namespace {
void ICUCleanUp()
{
    void* icuHandle = dlopen("libhmicuuc.z.so", RTLD_LAZY);
    if (!icuHandle) {
        BATTERY_HILOGE(COMP_SVC, "%{public}s: open so failed", __func__);
        return;
    }
    auto getIcuVersion = reinterpret_cast<const char* (*)(void)>(dlsym(icuHandle, "GetIcuVersion"));
    if (!getIcuVersion) {
        BATTERY_HILOGE(COMP_SVC, "find GetIcuVersion symbol failed");
        dlclose(icuHandle);
        return;
    }
    const char* version = getIcuVersion();
    constexpr int maxLength = 100;
    constexpr const char* icuCleanFuncName = "u_cleanup";
    auto buffer = std::make_unique<char[]>(maxLength);
    int ret = sprintf_s(buffer.get(), maxLength, "%s_%s", icuCleanFuncName, version);
    if (ret < 0) {
        BATTERY_HILOGE(COMP_SVC, "string operation failed");
        dlclose(icuHandle);
        return;
    }
    auto CleanUp = reinterpret_cast<void (*)(void)>(dlsym(icuHandle, buffer.get()));
    if (!CleanUp) {
        BATTERY_HILOGE(COMP_SVC, "find u_cleanup symbol failed");
        dlclose(icuHandle);
    }
    CleanUp();
    dlclose(icuHandle);
}
} // namespace

class SoundCallback : public Media::PlayerCallback {
public:
    explicit SoundCallback(const std::shared_ptr<ChargingSound>& strongSound) : weakSound_(strongSound) {}
    virtual ~SoundCallback() = default;
    void OnInfo(Media::PlayerOnInfoType type, int32_t /* extra */, const Media::Format& /* infoBody */) override
    {
        if (type == Media::INFO_TYPE_EOS) {
            auto sound = weakSound_.lock();
            if (sound) {
                sound->Stop();
            }
        }
    }
    void OnError(int32_t /* errorCode */, const std::string& /*errorMsg */) override
    {
        auto sound = weakSound_.lock();
        if (sound) {
            sound->Stop();
        }
    }

private:
    std::weak_ptr<ChargingSound> weakSound_ {};
};

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
        uri_ = std::string{CHARGER_SOUND_DEFAULT_PATH};
    }
    BATTERY_HILOGI(COMP_SVC, "ChargingSound instance created");
}

ChargingSound::~ChargingSound()
{
    Release();
    std::shared_ptr<Media::Player> tmp = std::atomic_load_explicit(&player_, std::memory_order_acquire);
    if (tmp) {
        tmp->ReleaseClientListener();
    }
    ICUCleanUp();
    BATTERY_HILOGI(COMP_SVC, "ChargingSound instance destroyed");
}

void ChargingSound::Stop()
{
    std::shared_ptr<Media::Player> tmp = std::atomic_load_explicit(&player_, std::memory_order_acquire);
    if (tmp) {
        tmp->Stop();
    }
    isPlaying_.store(false);
}

void ChargingSound::Release()
{
    std::shared_ptr<Media::Player> tmp = std::atomic_load_explicit(&player_, std::memory_order_acquire);
    if (tmp) {
        tmp->ReleaseSync();
    }
    isPlaying_.store(false);
}

bool ChargingSound::Play()
{
    std::shared_ptr<Media::Player> tmp = std::atomic_load_explicit(&player_, std::memory_order_acquire);
    if (!tmp) {
        std::lock_guard<std::mutex> lock(g_playerPtrMutex);
        tmp = std::atomic_load_explicit(&player_, std::memory_order_relaxed);
        if (!tmp) {
            tmp = Media::PlayerFactory::CreatePlayer();
        }
        std::atomic_store_explicit(&player_, tmp, std::memory_order_release);
    }
    if (!tmp) {
        BATTERY_HILOGE(COMP_SVC, "create player failed");
        return false;
    }
    tmp->Reset(); // reset avplayer
    int32_t ret = Media::MSERR_OK;
    std::shared_ptr<SoundCallback> callback = std::make_shared<SoundCallback>(instance_);
    ret = tmp->SetPlayerCallback(callback);
    if (ret != Media::MSERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "set player callback failed, ret=%{public}d", ret);
        return false;
    }
    ret = tmp->SetSource(uri_);
    if (ret != Media::MSERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "set stream source failed, ret=%{public}d", ret);
        return false;
    }
    Media::Format format;
    format.PutIntValue(Media::PlayerKeys::CONTENT_TYPE, AudioStandard::CONTENT_TYPE_UNKNOWN);
    format.PutIntValue(Media::PlayerKeys::STREAM_USAGE, AudioStandard::STREAM_USAGE_SYSTEM);
    ret = tmp->SetParameter(format);
    if (ret != Media::MSERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "Set stream usage to Player failed, ret=%{public}d", ret);
        return false;
    }
    ret = tmp->Prepare();
    if (ret != Media::MSERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "prepare failed, ret=%{public}d", ret);
        return false;
    }
    isPlaying_.store(true);
    ret = tmp->Play();
    if (ret != Media::MSERR_OK) {
        BATTERY_HILOGE(COMP_SVC, "play failed, ret=%{public}d", ret);
        isPlaying_.store(false);
        return false;
    }
    return true;
}

bool ChargingSound::IsPlaying()
{
    return isPlaying_.load();
}

bool ChargingSound::IsPlayingGlobal()
{
    return instance_->IsPlaying();
}

bool ChargingSound::PlayGlobal()
{
    bool ret = instance_->Play();
    if (!ret) {
        instance_->Release();
    }
    return ret;
}

void ChargingSound::ReleaseGlobal()
{
    instance_->Release();
}

//APIs
bool ChargingSoundStart()
{
    return ChargingSound::PlayGlobal();
}

bool IsPlaying()
{
    return ChargingSound::IsPlayingGlobal();
}

} // namespace PowerMgr
} // namespace OHOS
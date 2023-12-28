/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "charger_graphic_engine.h"
#include "charger_log.h"
#include "common/graphic_startup.h"
#include "common/image_decode_ability.h"
#include "common/task_manager.h"
#include "draw/draw_utils.h"
#include "font/ui_font_header.h"

#include <cinttypes>

namespace OHOS {
namespace PowerMgr {
ChargerGraphicEngine& ChargerGraphicEngine::GetInstance()
{
    static ChargerGraphicEngine instance;
    static bool isRegister = false;
    if (!isRegister) {
        OHOS::SoftEngine::InitGfxEngine(&instance);
        isRegister = true;
    }

    return instance;
}

void ChargerGraphicEngine::Init(uint32_t bkgColor, uint8_t mode, const char* fontPath, const char* ttfName)
{
    bkgColor_ = bkgColor;
    colorMode_ = mode;
    [[maybe_unused]] static bool initOnce = [this, fontPath, ttfName]() {
        sfDev_ = std::make_unique<GraphicDev>();
        if (!sfDev_->Init()) {
            BATTERY_HILOGE(FEATURE_CHARGING, "ChargerGraphicEngine::Init failed");
            return false;
        }
        sfDev_->GetScreenSize(width_, height_);
        buffInfo_ = nullptr;
        virAddr_ = nullptr;
        InitFontEngine(fontPath, ttfName);
        InitImageDecodeAbility();
        InitFlushThread();
        return true;
    }();
}

void ChargerGraphicEngine::InitFontEngine(const char* fontPath, const char* ttfName)
{
    constexpr uint32_t uiFontMemAlignment = 4;
    static uint32_t fontMemBaseAddr[OHOS::MIN_FONT_PSRAM_LENGTH / uiFontMemAlignment];
    static uint8_t icuMemBaseAddr[OHOS::SHAPING_WORD_DICT_LENGTH];
    OHOS::GraphicStartUp::InitFontEngine(
        reinterpret_cast<uintptr_t>(fontMemBaseAddr), OHOS::MIN_FONT_PSRAM_LENGTH, fontPath, ttfName);
    OHOS::GraphicStartUp::InitLineBreakEngine(reinterpret_cast<uintptr_t>(icuMemBaseAddr),
        OHOS::SHAPING_WORD_DICT_LENGTH, fontPath, DEFAULT_LINE_BREAK_RULE_FILENAME);
}

void ChargerGraphicEngine::InitImageDecodeAbility()
{
    uint32_t imageType = OHOS::IMG_SUPPORT_BITMAP | OHOS::IMG_SUPPORT_JPEG | OHOS::IMG_SUPPORT_PNG;
    OHOS::ImageDecodeAbility::GetInstance().SetImageDecodeAbility(imageType);
}

void ChargerGraphicEngine::InitFlushThread()
{
    flushStop_ = false;
    flushLoop_ = std::thread(&ChargerGraphicEngine::FlushThreadLoop, this);
    flushLoop_.detach();
}

void ChargerGraphicEngine::UsSleep(int usec)
{
    constexpr int USECONDS_PER_SECONDS = 1000000; // 1s = 1000000us
    constexpr int NANOSECS_PER_USECONDS = 1000;   // 1us = 1000ns
    auto seconds = usec / USECONDS_PER_SECONDS;
    long nanoSeconds = static_cast<long>(usec) % USECONDS_PER_SECONDS * NANOSECS_PER_USECONDS;
    struct timespec ts = {static_cast<time_t>(seconds), nanoSeconds};
    while (nanosleep(&ts, &ts) < 0 && errno == EINTR) {}
}

void ChargerGraphicEngine::FlushThreadLoop()
{
    while (!flushStop_) {
        OHOS::TaskManager::GetInstance()->TaskHandler();
        UsSleep(THREAD_USLEEP_TIME);
    }
}

OHOS::BufferInfo* ChargerGraphicEngine::GetFBBufferInfo()
{
    if (buffInfo_ != nullptr) {
        return buffInfo_.get();
    }

    uint8_t pixelBytes = OHOS::DrawUtils::GetByteSizeByColorMode(colorMode_);
    if (pixelBytes == 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "ChargerGraphicEngine get pixelBytes fail");
        return nullptr;
    }

    if ((width_ == 0) || (height_ == 0)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "input error, width: %{public}d, height: %{public}d", width_, height_);
        return nullptr;
    }
    virAddr_ = std::make_unique<uint8_t[]>(width_ * height_ * pixelBytes);
    buffInfo_ = std::make_unique<OHOS::BufferInfo>();
    buffInfo_->rect = {0, 0, static_cast<int16_t>(width_ - 1), static_cast<int16_t>(height_ - 1)};
    buffInfo_->mode = static_cast<OHOS::ColorMode>(colorMode_);
    buffInfo_->color = bkgColor_;
    buffInfo_->virAddr = virAddr_.get();
    buffInfo_->phyAddr = buffInfo_->virAddr;
    buffInfo_->stride = static_cast<uint32_t>(width_ * pixelBytes);
    buffInfo_->width = width_;
    buffInfo_->height = height_;
    return buffInfo_.get();
}

void ChargerGraphicEngine::Flush(const OHOS::Rect& flushRect)
{
    if ((sfDev_ == nullptr) || (buffInfo_ == nullptr)) {
        BATTERY_HILOGE(FEATURE_CHARGING, "null error");
        return;
    }
    std::lock_guard<std::mutex> lock {mtx_};
    sfDev_->Flip(reinterpret_cast<const uint8_t*>(buffInfo_->virAddr));
}

uint16_t ChargerGraphicEngine::GetScreenWidth()
{
    return width_;
}

uint16_t ChargerGraphicEngine::GetScreenHeight()
{
    return height_;
}
} // namespace PowerMgr
} // namespace OHOS

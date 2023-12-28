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

#ifndef CHARGER_GRAPHIC_ENGINE_H
#define CHARGER_GRAPHIC_ENGINE_H

#include "dev/graphic_dev.h"
#include "engines/gfx/soft_engine.h"
#include <memory>
#include <mutex>
#include <thread>

namespace OHOS {
namespace PowerMgr {
class ChargerGraphicEngine : public OHOS::SoftEngine {
    static constexpr uint32_t THREAD_USLEEP_TIME = 10000;
    DISALLOW_COPY_AND_MOVE(ChargerGraphicEngine);

public:
    ChargerGraphicEngine() = default;
    virtual ~ChargerGraphicEngine() = default;
    static ChargerGraphicEngine& GetInstance();
    void Init(uint32_t bkgColor, uint8_t mode, const char* fontPath, const char* ttfName);
    void Flush(const OHOS::Rect& flushRect) override;
    OHOS::BufferInfo* GetFBBufferInfo() override;
    uint16_t GetScreenWidth() override;
    uint16_t GetScreenHeight() override;

    static void UsSleep(int usec);

private:
    void FlushThreadLoop();
    void InitFontEngine(const char* fontPath, const char* ttfName);
    void InitImageDecodeAbility();
    void InitFlushThread();
    std::thread flushLoop_;
    std::unique_ptr<OHOS::BufferInfo> buffInfo_ = nullptr;
    std::unique_ptr<uint8_t[]> virAddr_ = nullptr;
    std::unique_ptr<GraphicDev> sfDev_ = nullptr;
    uint32_t bkgColor_ = 0;
    uint16_t height_ = 0;
    uint16_t width_ = 0;
    uint8_t colorMode_ = 0;
    bool flushStop_ = true;
    std::mutex mtx_ {};
};
} // namespace PowerMgr
} // namespace OHOS
#endif

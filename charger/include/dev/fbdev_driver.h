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

#ifndef FBDEV_DRIVER_H
#define FBDEV_DRIVER_H

#include "display_drv.h"
#include "nocopyable.h"
#include <cstdint>
#include <linux/fb.h>
#include <string>

namespace OHOS {
namespace PowerMgr {
struct FbBufferObject {
    uint32_t width {};
    uint32_t height {};
    uint32_t size {};
    void* vaddr {};
};

class FbdevDriver : public DisplayDrv {
    DISALLOW_COPY_AND_MOVE(FbdevDriver);
    using FbBlankHook = std::function<void(int, bool)>;

public:
    FbdevDriver() = default;
    ~FbdevDriver() override;
    bool Init() override;
    void Flip(const uint8_t* buf) override;
    void GetDisplayInfo(DisplayInfo& dsInfo) override;
    void Blank(bool blank) override;
    void Exit() override;
    static void SetDevPath(const std::string& devPath);
    static void RegisterBlankHook(FbBlankHook blankHook);

private:
    void ReleaseFb(const struct FbBufferObject* fbo);
    struct FbBufferObject buff_ {};
    struct fb_fix_screeninfo finfo_ {};
    struct fb_var_screeninfo vinfo_ {};
    bool FbPowerContrl(int fd, bool powerOn);
    static inline std::string devPath_ = FB_DEV_PATH;
    static inline FbBlankHook blankHook_ {};
};
} // namespace PowerMgr
} // namespace OHOS
#endif

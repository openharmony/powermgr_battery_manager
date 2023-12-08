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

#ifndef DISPLAY_DRV_H
#define DISPLAY_DRV_H

#include <stdint.h>
namespace OHOS {
namespace PowerMgr {
constexpr const char* FB_DEV_PATH = "/dev/graphics/fb0";
constexpr const char* DRM_DEV_PATH = "/dev/dri/card0";
using DisplayInfo = struct DisplayInfo_ {
    int width {};
    int height {};
    unsigned int rowBytes {};
    unsigned int pixelBytes {};
};
class DisplayDrv {
public:
    DisplayDrv() : fd_(-1) {}
    virtual ~DisplayDrv() {}
    virtual bool Init() = 0;
    virtual void Flip(const uint8_t* buf) = 0;
    virtual void GetDisplayInfo(DisplayInfo& dsInfo) = 0;
    virtual void Blank(bool blank) = 0;
    virtual void Exit() = 0;

protected:
    int fd_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif

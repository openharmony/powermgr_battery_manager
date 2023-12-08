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

#ifndef GRAPHIC_DEV_H
#define GRAPHIC_DEV_H
#include "display_drv.h"
#include "nocopyable.h"
#include <string>

namespace OHOS {
namespace PowerMgr {
enum class DevType { FB_DEVICE = 0, DRM_DEVICE, UNKNOW_DEVICE };

class GraphicDev {
    DISALLOW_COPY_AND_MOVE(GraphicDev);

public:
    GraphicDev() = default;
    ~GraphicDev() = default;
    void Flip(const uint8_t* buf);
    void GetScreenSize(uint16_t& w, uint16_t& h);
    bool Init();
    void Blank(bool blank);
    void Exit();

private:
    DevType GetDevType();
    std::unique_ptr<DisplayDrv> MakeDrv(DevType devType);
    std::unique_ptr<DisplayDrv> drv_{};
};
} // namespace PowerMgr
} // namespace OHOS
#endif

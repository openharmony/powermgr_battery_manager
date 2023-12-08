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
#include "dev/graphic_dev.h"
#include "charger_log.h"
#include "dev/drm_driver.h"
#include "dev/fbdev_driver.h"
#include <unistd.h>

namespace OHOS {
namespace PowerMgr {
std::unique_ptr<DisplayDrv> GraphicDev::MakeDrv(DevType devType)
{
    std::unique_ptr<DisplayDrv> drv = nullptr;
    switch (devType) {
        case DevType::DRM_DEVICE:
            drv = std::make_unique<DrmDriver>();
            break;
        case DevType::FB_DEVICE:
            drv = std::make_unique<FbdevDriver>();
            break;
        default:
            BATTERY_HILOGE(FEATURE_CHARGING, "not support device type");
            break;
    }
    return drv;
}

void GraphicDev::Flip(const uint8_t* buf)
{
    if (buf == nullptr) {
        BATTERY_HILOGE(FEATURE_CHARGING, "buf is null pointer");
        return;
    }
    if (drv_ != nullptr) {
        drv_->Flip(buf);
    }
}

DevType GraphicDev::GetDevType()
{
    if (access(DRM_DEV_PATH, 0) == 0) {
        return DevType::DRM_DEVICE;
    } else if (access(FB_DEV_PATH, 0) == 0) {
        return DevType::FB_DEVICE;
    }
    return DevType::UNKNOW_DEVICE;
}

bool GraphicDev::Init()
{
    drv_ = MakeDrv(GetDevType());
    if (drv_ != nullptr) {
        return drv_->Init();
    }
    return false;
}

void GraphicDev::Blank(bool blank)
{
    if (drv_ != nullptr) {
        drv_->Blank(blank);
    }
}

void GraphicDev::Exit()
{
    if (drv_ != nullptr) {
        drv_->Exit();
    }
}

void GraphicDev::GetScreenSize(uint16_t& w, uint16_t& h)
{
    DisplayInfo dsInfo {0};
    if (drv_ != nullptr) {
        drv_->GetDisplayInfo(dsInfo);
    }
    w = dsInfo.width;
    h = dsInfo.height;
}
} // namespace PowerMgr
} // namespace OHOS

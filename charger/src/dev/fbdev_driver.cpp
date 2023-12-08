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
#include "dev/fbdev_driver.h"
#include "charger_log.h"

#include "securec.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

namespace OHOS {
namespace PowerMgr {
FbdevDriver::~FbdevDriver()
{
    ReleaseFb(&buff_);
}

bool FbdevDriver::Init()
{
    if (devPath_.empty()) {
        BATTERY_HILOGE(FEATURE_CHARGING, "dev path is empty, init failed, check whether SetDevPath correctly called");
        return false;
    }
    int fd = open(devPath_.c_str(), O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "cannot open fb0");
        return false;
    }

    (void)FbPowerContrl(fd, false);
    (void)FbPowerContrl(fd, true);

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo_) < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to get fb0 info");
        close(fd);
        return false;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo_) < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to get fb0 info");
        close(fd);
        return false;
    }

    buff_.width = vinfo_.xres;
    buff_.height = vinfo_.yres;
    buff_.size = finfo_.line_length * vinfo_.yres;
    buff_.vaddr = mmap(nullptr, finfo_.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buff_.vaddr == MAP_FAILED) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to mmap framebuffer");
        close(fd);
        return false;
    }
    (void)memset_s(buff_.vaddr, finfo_.smem_len, 0, finfo_.smem_len);
    fd_ = fd;
    return true;
}

void FbdevDriver::Flip(const uint8_t* buf)
{
    if (fd_ < 0 || memcpy_s(buff_.vaddr, buff_.size, buf, buff_.size) != EOK) {
        return;
    }
    if (ioctl(fd_, FBIOPAN_DISPLAY, &vinfo_) < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to display fb0!");
    }
}

void FbdevDriver::GetDisplayInfo(DisplayInfo& dsInfo)
{
    dsInfo.width = static_cast<int>(vinfo_.xres);
    dsInfo.height = static_cast<int>(vinfo_.yres);
    dsInfo.rowBytes = finfo_.line_length;
    dsInfo.pixelBytes = vinfo_.bits_per_pixel / 8; // 8: byte bit len
}

void FbdevDriver::Blank(bool blank)
{
    FbPowerContrl(fd_, !blank);
    if (blankHook_ != nullptr) {
        blankHook_(fd_, blank);
    }
}

void FbdevDriver::Exit()
{
    ReleaseFb(&buff_);
}

void FbdevDriver::SetDevPath(const std::string& devPath)
{
    devPath_ = devPath;
}

void FbdevDriver::RegisterBlankHook(FbBlankHook blankHook)
{
    blankHook_ = blankHook;
}

void FbdevDriver::ReleaseFb(const struct FbBufferObject* fbo)
{
    /*
     * When fd_ isn't less than 0, then fbo->vaddr is valid and can by safely munmap.
     * this can be guaranteed by FbdevDriver::Init.
     */
    if (fd_ < 0) {
        return;
    }
    munmap(fbo->vaddr, fbo->size);
    close(fd_);
    fd_ = -1;
}

bool FbdevDriver::FbPowerContrl(int fd, bool powerOn)
{
    if (fd < 0) {
        return false;
    }
    if (ioctl(fd, FBIOBLANK, powerOn ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN) < 0) {
        BATTERY_HILOGE(FEATURE_CHARGING, "failed to set fb0, %{public}s", strerror(errno));
        return false;
    }
    return true;
}
} // namespace PowerMgr
} // namespace OHOS

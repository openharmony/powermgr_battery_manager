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

#ifndef DRM_DRIVER_H
#define DRM_DRIVER_H

#include "display_drv.h"
#include "nocopyable.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <drm_fourcc.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

namespace OHOS {
namespace PowerMgr {
struct BufferObject {
    uint32_t width {};
    uint32_t height {};
    uint32_t pitch {};
    uint32_t handle {};
    uint32_t size {};
    uint8_t* vaddr {};
    uint32_t fbId {};
};

class DrmDriver : public DisplayDrv {
    DISALLOW_COPY_AND_MOVE(DrmDriver);

public:
    DrmDriver() : conn_(nullptr), res_(nullptr), crtc_(nullptr) {}
    ~DrmDriver() override;
    bool Init() override;
    void Flip(const uint8_t* buf) override;
    void Blank(bool blank) override;
    void Exit() override;
    void GetDisplayInfo(DisplayInfo& dsInfo) override;

private:
    int ModesetCreateFb(struct BufferObject* bo);
    void ModesetDestroyFb(struct BufferObject* bo);
    int DrmInit();
    drmModeCrtc* GetCrtc(const drmModeRes& res, const int fd, const drmModeConnector& conn) const;
    drmModeConnector* GetFirstConnector(const drmModeRes& res, const int fd) const;
    drmModeConnector* GetConnectorByType(const drmModeRes& res, const int fd, const uint32_t type) const;
    drmModeConnector* GetConnector(const drmModeRes& res, const int fd, uint32_t& modeId) const;
    drmModeRes* GetResources(int& fd) const;
    drmModeRes* GetOneResources(const int devIndex, int& fd) const;
    drmModeConnector* conn_;
    drmModeRes* res_;
    drmModeCrtc* crtc_;
    struct BufferObject buff_ {};
};
} // namespace PowerMgr
} // namespace OHOS
#endif

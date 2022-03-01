/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "view.h"

#include "log.h"
#include "securec.h"
#include "utils/hdf_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
constexpr int RGBA_PIXEL_SIZE = 4;
void* View::CreateBuffer(int w, int h, View::PixelFormat pixelFormat)
{
    HDF_LOGD("%{public}s enter", __func__);
    int pixelSize = -1;
    switch (int(pixelFormat)) {
        case int(View::PixelFormat::BGRA888):
            pixelSize = RGBA_PIXEL_SIZE;
            break;
        default:
            HDF_LOGD("%{public}s, Unsupported pixel_format: %{public}d", __func__, int(pixelFormat));
            HDF_LOGD("%{public}s, Use default BGRA888.", __func__);
            pixelSize = RGBA_PIXEL_SIZE;
            break;
    }
    bufferSize_ = w * h * pixelSize;
    viewBuffer_ = static_cast<char*>(malloc(bufferSize_));
    if (viewBuffer_ == nullptr) {
        HDF_LOGE("%{public}s, Allocate memory for view failed: %{public}d", __func__, errno);
        return nullptr;
    }
    shadowBuffer_ = static_cast<char*>(malloc(bufferSize_));
    if (shadowBuffer_ == nullptr) {
        HDF_LOGE("%{public}s, Allocate memory for shadow failed: %{public}d", __func__, errno);
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    if (memset_s(viewBuffer_, bufferSize_, 0, bufferSize_) != EOK) {
        HDF_LOGE("%{public}s, Clean view buffer failed.", __func__);
        free(viewBuffer_);
        viewBuffer_ = nullptr;
        return nullptr;
    }
    viewWidth_ = w;
    viewHeight_ = h;
    return viewBuffer_;
}

void View::SetBackgroundColor(BRGA888Pixel* color)
{
    HDF_LOGD("%{public}s enter", __func__);
    BRGA888Pixel pixelBuffer[viewWidth_];
    for (int w = 0; w < viewWidth_; w++) {
        pixelBuffer[w].r = color->r;
        pixelBuffer[w].g = color->g;
        pixelBuffer[w].b = color->b;
        pixelBuffer[w].a = color->a;
    }
    for (int h = 0; h < viewHeight_; h++) {
        if (memcpy_s(viewBuffer_ + h * viewWidth_ * sizeof(BRGA888Pixel), viewWidth_ * sizeof(BRGA888Pixel) + 1,
            reinterpret_cast<char*>(pixelBuffer), viewWidth_ * sizeof(BRGA888Pixel)) != EOK) {
            return;
        }
    }
    if (isVisiable_) {
        OnDraw();
        HDF_LOGD("%{public}s, view---visable", __func__);
    }
}

void View::DrawSubView(int x, int y, int w, int h, char* buf)
{
    HDF_LOGD("%{public}s enter", __func__);
    int minWidth = ((x + w) <= viewWidth_) ? w : (viewWidth_ - x);
    int minHeight = ((y + h) <= viewHeight_) ? h : (viewHeight_ - y);
    HDF_LOGD("%{public}s, x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d", __func__, x, y, w, h);
    HDF_LOGD("%{public}s, minWidth = %{public}d, minHeight = %{public}d", __func__, minWidth, minHeight);
    for (int i = 0; i < minHeight; i++) {
        char* src = buf + i * w * static_cast<int32_t>(sizeof(BRGA888Pixel));
        char* dst = shadowBuffer_ + (i + y) * viewWidth_ * static_cast<int32_t>(sizeof(BRGA888Pixel)) +
            x * static_cast<int32_t>(sizeof(BRGA888Pixel));
        if (memcpy_s(dst, minWidth * static_cast<int32_t>(sizeof(BRGA888Pixel)) + 1, src,
            minWidth * static_cast<int32_t>(sizeof(BRGA888Pixel))) != EOK) {
            return;
        }
    }
}

void View::OnDraw()
{
    HDF_LOGD("%{public}s enter", __func__);
    std::unique_lock<std::mutex> locker(mutex_);
    SyncBuffer();
}

void View::Hide()
{
    HDF_LOGD("%{public}s enter", __func__);
    if (isVisiable_) {
        isVisiable_ = false;
        OnDraw();
    }
}

void View::Show()
{
    HDF_LOGD("%{public}s enter", __func__);
    if (!isVisiable_) {
        isVisiable_ = true;
        OnDraw();
    }
}

void View::SyncBuffer()
{
    HDF_LOGD("%{public}s enter", __func__);
    if (memcpy_s(shadowBuffer_, bufferSize_, viewBuffer_, bufferSize_) != EOK) {
        HDF_LOGD("%{public}s, Sync buffer failed.", __func__);
    }
}

char* View::GetBuffer() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return shadowBuffer_;
}

void* View::GetRawBuffer() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return viewBuffer_;
}

void View::OnFocus(bool foucsed)
{
    HDF_LOGD("%{public}s enter", __func__);
    isFocused_ = foucsed;
    OnDraw();
}

void View::SetViewId(int id)
{
    HDF_LOGD("%{public}s enter", __func__);
    viewId_ = id;
}

int View::GetViewId() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return viewId_;
}

void View::FreeBuffer()
{
    HDF_LOGD("%{public}s enter", __func__);
    free(viewBuffer_);
    free(shadowBuffer_);
    viewBuffer_ = nullptr;
    shadowBuffer_ = nullptr;
}

bool View::IsVisiable() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return isVisiable_;
}

bool View::IsSelected() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return isFocused_;
}

bool View::IsFocusAble() const
{
    HDF_LOGD("%{public}s enter", __func__);
    return focusable_;
}

void View::SetFocusAble(bool focusable)
{
    HDF_LOGD("%{public}s enter", __func__);
    focusable_ = focusable;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

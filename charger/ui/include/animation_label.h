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

#ifndef CHARGER_UI_ANIMATION_LABLE_H
#define CHARGER_UI_ANIMATION_LABLE_H
#include <ctime>
#include <thread>
#include <unistd.h>
#include <vector>
#include <memory>
#include "frame.h"
#include "png.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class AnimationLabel : public View {
public:
    enum class PlayMode {
        ANIMATION_MODE = 0,
        STATIC_MODE,
    };
    AnimationLabel(int startX, int startY, int w, int h, Frame* mParent);
    AnimationLabel() {}
    ~AnimationLabel() override;
public:
    void AddImg(const std::string& imgFileName);
    int AddStaticImg(const std::string& imgFileName);
    void SetStaticImg(int picId);
    void SetPlayMode(AnimationLabel::PlayMode mode);
    void SetInterval(int ms);
    std::thread updateThread;
    bool selectable = false;
    static void SetIsVisible(const bool visible);
    void UpdateLoop();
    static bool needStop_;
private:
    struct PictureAttr {
        png_uint_32 pictureWidth;
        png_uint_32 pictureHeight;
        png_byte pictureChannels;
        int bitDepth;
        int colorType;
    };
    void* LoadPng(const std::string& imgFileName);
    View::BRGA888Pixel* HandleLoadPng(FILE** fp, char** pictureBufferTmp, struct PictureAttr& attr);
    int LoadPngInternalWithFile(FILE* fp, png_structpp pngPtr, png_infopp pngInfoPtr, struct PictureAttr& attr);
    void CopyPictureBuffer(struct PictureAttr& attr, char* pictureBufferTmp, BRGA888Pixel* pictureBuffer) const;
    Frame* parent_ {};
    uint32_t intervalMs_ = 50;
    std::vector<char*> imgList_ {};
    char* staticImgList_[255] {};
    int staticShowId_ = 0;
    bool showStatic_ = false;
    int staticImgSize_ = 0;
    static bool isVisible_;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif // CHARGER_UI_ANIMATION_LABLE_H

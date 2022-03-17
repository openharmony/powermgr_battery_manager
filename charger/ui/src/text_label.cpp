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

#include "text_label.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <linux/input.h>
#include "log.h"
#include "png.h"
#include "securec.h"
#include "utils/hdf_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const std::string DEFAULT_FONT_NAME = "font";
constexpr int MAX_TEXT_SIZE = 512;
constexpr int MAX_FONT_BUFFER_SIZE_HW  = 4096;

TextLabel::TextLabel(int mStartX, int mStartY, int w, int h, Frame* mparent)
{
    startX_ = mStartX;
    startY_ = mStartY;
    this->CreateBuffer(w, h, View::PixelFormat::BGRA888);
    parent_ = mparent;
    parent_->ViewRegister(this);

    SetFocusAble(true);
    outlineColor_.r = 0x00;
    outlineColor_.g = 0x00;
    outlineColor_.b = 0x00;
    outlineColor_.a = 0x00;
    boldTopLine_ = false;
    boldTopLine_ = false;

    const char midAlpha = 0xAA;
    actionBgColor_.r = 0x00;
    actionBgColor_.g = 0x00;
    actionBgColor_.b = 0x00;
    actionBgColor_.a = midAlpha;

    const char maxAlpha = 0xff;
    normalBgColor_.r = 0x00;
    normalBgColor_.g = 0x00;
    normalBgColor_.b = 0x00;
    normalBgColor_.a = maxAlpha;

    const char maxLevel = 0xff;
    textColor_.r = maxLevel;
    textColor_.g = maxLevel;
    textColor_.b = maxLevel;
    textColor_.a = maxLevel;

    if (memset_s(textBuf_, MAX_TEXT_SIZE + 1, 0, MAX_TEXT_SIZE) != 0) {
        HDF_LOGE("%{public}s, memset_s failed!", __func__);
        return;
    }
    InitFont();
}

void TextLabel::SetFont(FontType fType)
{
    HDF_LOGD("%{public}s enter", __func__);
    fontType_ = fType;
    InitFont();
    OnDraw();
}

static void PngInitSet(png_structp fontPngPtr, FILE* fp, int size, png_infop fontInfoPtr)
{
    HDF_LOGD("%{public}s enter", __func__);
    png_init_io(fontPngPtr, fp);
    png_set_sig_bytes(fontPngPtr, size);
    png_read_info(fontPngPtr, fontInfoPtr);
    return;
}

static void PNGReadRow(png_uint_32 fontWidth, png_uint_32 fontHeight, png_structp fontPngPtr, char* fontBuf)
{
    HDF_LOGD("%{public}s enter", __func__);
    if ((fontWidth > MAX_FONT_BUFFER_SIZE_HW) || (fontHeight > MAX_FONT_BUFFER_SIZE_HW)) {
        HDF_LOGE("%{public}s, font file size is too big!", __func__);
        return;
    }
    for (unsigned int y = 0; y < fontHeight; y++) {
        uint8_t* pRow = reinterpret_cast<uint8_t*>((fontBuf) + y * MAX_FONT_BUFFER_SIZE_HW);
        png_read_row(fontPngPtr, pRow, nullptr);
    }
    return;
}

static void CheckInitFont(png_structp fontPngPtr, FILE **fp, png_infop fontInfoPtr)
{
    HDF_LOGD("%{public}s enter", __func__);
    png_destroy_read_struct(&fontPngPtr, &fontInfoPtr, 0);
    if (*fp != nullptr) {
        fclose(*fp);
        *fp = nullptr;
    }
}

void TextLabel::InitFont()
{
    HDF_LOGD("%{public}s enter", __func__);
    png_infop fontInfoPtr = nullptr;
    png_uint_32 fontWidth = 0;
    png_uint_32 fontHeight = 0;
    png_byte fontChannels = 0;
    png_structp fontPngPtr = nullptr;
    int fontBitDepth = 0;
    int fontColorType = 0;

    char resPath[MAX_TEXT_SIZE + 1];
    uint32_t offset = 2;

    if (memset_s(resPath, MAX_TEXT_SIZE + offset, 0, MAX_TEXT_SIZE + 1)) {
        return;
    }
    switch (fontType_) {
        case FontType::DEFAULT_FONT:
            if (snprintf_s(resPath, sizeof(resPath), sizeof(resPath) - 1,
                "/system/etc/resources/%s.png", DEFAULT_FONT_NAME.c_str()) == -1) {
                return;
            }
            break;
        default:
            if (snprintf_s(resPath, sizeof(resPath), sizeof(resPath) - 1,
                "/system/etc/resources/%s.png", DEFAULT_FONT_NAME.c_str()) == -1) {
                return;
            }
            break;
    }
    FILE* fp = fopen(resPath, "rb");
    if (fp == nullptr) {
        HDF_LOGD("%{public}s: open font failed.", __func__);
        return;
    }

    const int headerNumber = 8;
    uint8_t header[headerNumber];
    size_t bytesRead = fread(header, 1, sizeof(header), fp);
    if (!(bytesRead == sizeof(header))) {
        HDF_LOGE("%{public}s, read header failed!", __func__);
        fclose(fp);
        return;
    }
    if (png_sig_cmp(header, 0, sizeof(header))) {
        HDF_LOGE("%{public}s, cmp header failed!", __func__);
        fclose(fp);
        return;
    }
    fontPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!fontPngPtr) {
        HDF_LOGE("%{public}s, creat font ptr_ failed!", __func__);
        fclose(fp);
        return;
    }
    fontInfoPtr = png_create_info_struct(fontPngPtr);
    if (fontInfoPtr == nullptr) {
        png_destroy_read_struct(&fontPngPtr, nullptr, nullptr);
        fclose(fp);
        return;
    }
    PngInitSet(fontPngPtr, fp, sizeof(header), fontInfoPtr);
    png_get_IHDR(fontPngPtr, fontInfoPtr, &fontWidth, &fontHeight, &fontBitDepth, &fontColorType,
        nullptr, nullptr, nullptr);
    fontChannels = png_get_channels(fontPngPtr, fontInfoPtr);
    const int defaultFontBitDepth = 8;
    if (fontBitDepth <= defaultFontBitDepth && fontChannels == 1 && fontColorType == PNG_COLOR_TYPE_GRAY) {
        png_set_expand_gray_1_2_4_to_8(fontPngPtr);
    }
    const int defaultFontWidth = 96;
    fontWidth_ = fontWidth / defaultFontWidth;
    fontHeight_ = fontHeight >> 1;
    PNGReadRow(fontWidth_, fontHeight_, fontPngPtr, fontBuf_);
    CheckInitFont(fontPngPtr, &fp, fontInfoPtr);
}

FILE* TextLabel::InitFontType()
{
    HDF_LOGD("%{public}s enter", __func__);
    char resPath[MAX_TEXT_SIZE + 1];
    uint32_t offset = 2;

    if (memset_s(resPath, MAX_TEXT_SIZE + offset, 0, MAX_TEXT_SIZE + 1) != EOK) {
        return nullptr;
    }
    switch (fontType_) {
        case FontType::DEFAULT_FONT:
            if (snprintf_s(resPath, sizeof(resPath), sizeof(resPath) - 1,
                "/system/etc/resources/%s.png", DEFAULT_FONT_NAME.c_str()) == -1) {
                return nullptr;
            }
            break;
        default:
            if (snprintf_s(resPath, sizeof(resPath), sizeof(resPath) - 1,
                "/system/etc/resources/%s.png", DEFAULT_FONT_NAME.c_str()) == -1) {
                return nullptr;
            }
            break;
    }
    FILE* fp = fopen(resPath, "rb");
    if (fp == nullptr) {
        HDF_LOGD("%{public}s: open font failed.", __func__);
        return nullptr;
    }
    return fp;
}

void TextLabel::SetText(const char* str)
{
    HDF_LOGD("%{public}s enter", __func__);
    if (memset_s(textBuf_, MAX_TEXT_SIZE + 1, 0, MAX_TEXT_SIZE) != EOK) {
        HDF_LOGD("%{public}s: memset_s fail.", __func__);
        return;
    }
    OnDraw();
    if (memcpy_s(textBuf_, MAX_TEXT_SIZE + 1, str, strlen(const_cast<char*>(str))) != EOK) {
        HDF_LOGD("%{public}s: mmecpy_s fail.", __func__);
        return;
    }
    OnDraw();
}

void TextLabel::OnDraw()
{
    HDF_LOGD("%{public}s enter", __func__);
    std::unique_lock<std::mutex> locker(mutex_);
    SyncBuffer();
    if (IsSelected()) {
        DrawFocus();
    }
    DrawOutline();
    DrawText();
    if (parent_ != nullptr) {
        parent_->OnDraw();
    }
}

void TextLabel::SetOutLineBold(bool topBold, bool bottomBold)
{
    HDF_LOGD("%{public}s enter", __func__);
    boldTopLine_ = topBold;
    boldBottomLine_ = bottomBold;
    OnDraw();
}

void TextLabel::DrawOutline()
{
    HDF_LOGD("%{public}s enter", __func__);
    void* tmpBuf = GetBuffer();
    auto* pixelBuf = static_cast<BRGA888Pixel*>(tmpBuf);
    for (int i = 0; i < viewWidth_; i++) {
        if (boldTopLine_) {
            pixelBuf[i].r = outlineColor_.r;
            pixelBuf[i].g = outlineColor_.g;
            pixelBuf[i].b = outlineColor_.b;
            pixelBuf[i].a = outlineColor_.a;

            pixelBuf[viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[viewWidth_ + i].a = outlineColor_.a;
        }

        const int lines = 2;
        if (boldBottomLine_) {
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[(viewHeight_ - lines) * viewWidth_ + i].a = outlineColor_.a;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].r = outlineColor_.r;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].g = outlineColor_.g;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].b = outlineColor_.b;
            pixelBuf[(viewHeight_ - 1) * viewWidth_ + i].a = outlineColor_.a;
        }
    }
}

void TextLabel::SetTextColor(BRGA888Pixel color)
{
    HDF_LOGD("%{public}s enter", __func__);
    textColor_.r = color.r;
    textColor_.g = color.g;
    textColor_.b = color.b;
    textColor_.a = color.a;
}

void TextLabel::SetTextAlignmentMethod(AlignmentMethod methodH, AlignmentMethod methodV)
{
    HDF_LOGD("%{public}s enter", __func__);
    fontAligMethodLevel_ = methodH;
    fontAligMethodUpright_ = methodV;
}

void TextLabel::DrawText()
{
    HDF_LOGD("%{public}s enter", __func__);
    void* tmpBuf = GetBuffer();
    uint32_t textSx = 0;
    uint32_t textSy = 0;
    switch (fontAligMethodUpright_) {
        case AlignmentMethod::ALIGN_CENTER:
            textSy = (static_cast<uint32_t>(viewHeight_) - fontHeight_) >> 1;
            break;
        case AlignmentMethod::ALIGN_TO_TOP:
            textSy = 0;
            break;
        default:
            break;
    }

    const uint32_t minPosition = 10;
    const uint32_t average = 2;
    switch (fontAligMethodLevel_) {
        case AlignmentMethod::ALIGN_CENTER:
            textSx = (static_cast<uint32_t>(viewWidth_) - (strlen(textBuf_) * fontWidth_)) / average;
            if (textSx < minPosition) {
                textSx = minPosition;
            }
            break;
        case AlignmentMethod::ALIGN_TO_LEFT:
            textSx = minPosition;
            break;
        default:
            break;
    }
    unsigned char ch;
    char* s = textBuf_;
    while ((ch = *s++)) {
        if ((ch < ' ') || (ch > '~')) {
            ch = '?';
        }
        DrawTextLoop(ch, static_cast<char*>(tmpBuf), textSx, textSy);
        textSx += fontWidth_;
    }
}

void TextLabel::DrawTextLoop(unsigned char ch, char* tmpBuf, int textSx, int textSy)
{
    HDF_LOGD("%{public}s enter", __func__);
    auto* srcP = reinterpret_cast<uint8_t*>(static_cast<char*>(fontBuf_) + ((ch - ' ') * fontWidth_));
    auto* dstP = reinterpret_cast<BRGA888Pixel*>(tmpBuf + (textSy * viewWidth_ + textSx) * sizeof(BRGA888Pixel));
    for (unsigned int j = 0; j < fontHeight_; j++) {
        for (unsigned int i = 0; i < fontWidth_; i++) {
            uint8_t a = srcP[i];
            if (a > 0) {
                dstP[i].r = textColor_.r;
                dstP[i].g = textColor_.g;
                dstP[i].b = textColor_.b;
                dstP[i].a = textColor_.a;
            }
        }
        srcP += MAX_FONT_BUFFER_SIZE_HW;
        dstP = dstP + viewWidth_;
    }
}

void TextLabel::DrawFocus()
{
    HDF_LOGD("%{public}s enter", __func__);
    BRGA888Pixel pixBuf[viewWidth_];
    for (int a = 0; a < viewWidth_; a++) {
        pixBuf[a].r = actionBgColor_.r;
        pixBuf[a].g = actionBgColor_.g;
        pixBuf[a].b = actionBgColor_.b;
        pixBuf[a].a = actionBgColor_.a;
    }
    void* viewBgBuf = GetBuffer();
    for (int i = 0; i < viewHeight_; i++) {
        if (memcpy_s(static_cast<char*>(static_cast<char*>(viewBgBuf) + i * viewWidth_ * sizeof(BRGA888Pixel)),
            viewWidth_ * sizeof(BRGA888Pixel) + 1, reinterpret_cast<char*>(pixBuf),
            viewWidth_ * sizeof(BRGA888Pixel)) != EOK) {
            HDF_LOGD("%{public}s: memcpy_s failed.", __func__);
            return;
        }
    }
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

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

#include "battery_backlight.h"

#include <hdf_log.h>
#include <hdf_base.h>
#include <string>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
static const std::string BACKLIGHT_DEVICE_PATH = "/sys/class/backlight/sprd_backlight/brightness";
static const std::string SEMICOLON = ";";
static const int MAX_STR = 255;
static const unsigned int BACKLIGHT_ON = 128;
static const unsigned int BACKLIGHT_OFF = 0;

void BatteryBacklight::TurnOnScreen()
{
    HDF_LOGI("%{public}s enter", __func__);
    HandleBacklight(BACKLIGHT_ON);
    screenOn_ = true;
}

void BatteryBacklight::TurnOffScreen()
{
    HDF_LOGI("%{public}s enter", __func__);
    HandleBacklight(BACKLIGHT_OFF);
    screenOn_ = false;
}

bool BatteryBacklight::GetScreenState()
{
    HDF_LOGI("%{public}s enter", __func__);
    return screenOn_;
}

int BatteryBacklight::HandleBacklight(const unsigned int backlight)
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE *fp = nullptr;
    int writeFile = -1;
    char *path = nullptr;
    char *pathGroup = nullptr;
    unsigned int bufferLen;

    HDF_LOGI("%{public}s: backlight value is %{public}d", __func__, backlight);
    bufferLen = strnlen(BACKLIGHT_DEVICE_PATH.c_str(), MAX_STR) + 1;
    pathGroup = (char*)malloc(bufferLen);
    if (pathGroup == nullptr) {
        HDF_LOGD("%{public}s: malloc error.", __func__);
        return writeFile;
    }

    strlcpy(pathGroup, BACKLIGHT_DEVICE_PATH.c_str(), bufferLen);

    path = pathGroup;
    while ((path = strtok(path, SEMICOLON.c_str())) != nullptr) {
        fp = fopen(path, "w");
        if (fp != nullptr) {
            writeFile = fprintf(fp, "%u\n", backlight);
            fclose(fp);
        }
        if (writeFile <= 0) {
            HDF_LOGD("%{public}s: failed to set backlight path=%{pusblic}s.", __func__, path);
        }
        path = nullptr;
    }
    free(pathGroup);

    HDF_LOGI("%{public}s exit", __func__);
    return writeFile;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

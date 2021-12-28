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

#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <hdf_log.h>
#include <hdf_base.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const std::string SEMICOLON = ";";
const int MAX_STR = 255;
const unsigned int BACKLIGHT_ON = 128;
const unsigned int BACKLIGHT_OFF = 0;
const unsigned int MKDIR_WAIT_TIME = 1;
std::vector<std::string> g_backlightNodeName;
const std::string BACKLIGHT_BASE_PATH = "/sys/class/leds";
std::string g_backlightNode = "backlight";

BatteryBacklight::BatteryBacklight()
{
    InitDefaultSysfs();
}

void BatteryBacklight::TraversalBacklightNode()
{
    HDF_LOGD("%{public}s: enter", __func__);
    std::string::size_type idx;

    for (auto iter = g_backlightNodeName.begin(); iter != g_backlightNodeName.end(); ++iter) {
        idx = iter->find(g_backlightNode);
        if (idx == std::string::npos) {
            HDF_LOGD("%{public}s: not found backlight node", __func__);
        } else {
            g_backlightNode = *iter;
            HDF_LOGD("%{public}s: backlight node is %{public}s", __func__, iter->c_str());
        }
    }

    HDF_LOGD("%{public}s: exit", __func__);
}

int32_t BatteryBacklight::InitBacklightSysfs()
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;
    int maxSize = 64;

    dir = opendir(BACKLIGHT_BASE_PATH.c_str());
    if (dir == nullptr) {
        HDF_LOGE("%{public}s: backlight base path is not exist", __func__);
        return HDF_ERR_IO;
    }

    while (true) {
        entry = readdir(dir);
        if (entry == nullptr) {
            break;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (entry->d_type == DT_DIR || entry->d_type == DT_LNK) {
            HDF_LOGD("%{public}s: init backlight info of %{public}s", __func__, entry->d_name);
            if (index >= maxSize) {
                HDF_LOGE("%{public}s: too many backlight types", __func__);
                break;
            }
            g_backlightNodeName.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalBacklightNode();
    HDF_LOGD("%{public}s: index is %{public}d", __func__, index);
    closedir(dir);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

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

bool BatteryBacklight::GetScreenState() const
{
    HDF_LOGI("%{public}s enter", __func__);
    return screenOn_;
}

std::string BatteryBacklight::CreateFile(std::string path, std::string content) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: Cannot create file %{public}s", __func__, path.c_str());
        return nullptr;
    }
    stream << content.c_str() << std::endl;
    stream.close();
    return path;
}

void BatteryBacklight::InitDefaultSysfs(void) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string brightnessPath = "/data";
    if (access(brightnessPath.c_str(), 0) == -1) {
        mkdir("/data", S_IRWXU);
        sleep(MKDIR_WAIT_TIME);
    }

    HDF_LOGE("%{public}s: create default brightness path for Hi3516DV300", __func__);
    CreateFile("/data/brightness", "127");
}

void BatteryBacklight::InitDevicePah(std::string& path) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (access(path.c_str(), F_OK) == 0) {
        HDF_LOGI("%{public}s: system backlight path exist", __func__);
        return;
    } else {
        HDF_LOGI("%{public}s: create mock backlight path", __func__);
        path = "/data/brightness";
        return;
    }

    HDF_LOGI("%{public}s exit", __func__);
}

int BatteryBacklight::HandleBacklight(const unsigned int backlight) const
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE* fp = nullptr;
    int writeFile = -1;
    char* path = nullptr;
    char* pathGroup = nullptr;
    unsigned int bufferLen;
    std::string devicePath = BACKLIGHT_BASE_PATH + "/" + g_backlightNode + "/" + "brightness";
    HDF_LOGI("%{public}s: backlight devicepath is %{public}s", __func__, devicePath.c_str());
    InitDevicePah(devicePath);

    HDF_LOGI("%{public}s: backlight value is %{public}d", __func__, backlight);
    bufferLen = strnlen(devicePath.c_str(), MAX_STR) + 1;
    pathGroup = (char*)malloc(bufferLen);
    if (pathGroup == nullptr) {
        HDF_LOGD("%{public}s: malloc error.", __func__);
        return writeFile;
    }

    strlcpy(pathGroup, devicePath.c_str(), bufferLen);

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

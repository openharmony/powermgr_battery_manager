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

#include "battery_led.h"

#include <hdf_log.h>
#include <hdf_base.h>
#include <fstream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const int CAPACITY_FULL = 100;
const int MKDIR_WAIT_TIME = 1;
const int LED_COLOR_GREEN = 2;
const int LED_COLOR_RED = 4;
const int LED_COLOR_YELLOW = 6;
std::vector<std::string> g_ledsNodeName;
const std::string LEDS_BASE_PATH = "/sys/class/leds";
std::string g_redLedsNode = "red";
std::string g_greenLedsNode = "green";
std::string g_blueLedsNode = "blue";

void BatteryLed::TraversalNode()
{
    HDF_LOGD("%{public}s: enter", __func__);
    std::string::size_type idx;

    for (auto iter = g_ledsNodeName.begin(); iter != g_ledsNodeName.end(); ++iter) {
        idx = iter->find(g_redLedsNode);
        if (idx == std::string::npos) {
            HDF_LOGD("%{public}s: not found red leds node", __func__);
        } else {
            g_redLedsNode = *iter;
            HDF_LOGD("%{public}s: red leds node is %{public}s", __func__, iter->c_str());
        }

        idx = iter->find(g_greenLedsNode);
        if (idx == std::string::npos) {
            HDF_LOGD("%{public}s: not found green leds node", __func__);
        } else {
            g_greenLedsNode = *iter;
            HDF_LOGD("%{public}s: green leds node is %{public}s", __func__, iter->c_str());
        }

        idx = iter->find(g_blueLedsNode);
        if (idx == std::string::npos) {
            HDF_LOGD("%{public}s: not found blue leds node", __func__);
        } else {
            g_blueLedsNode = *iter;
            HDF_LOGD("%{public}s: blue leds node is %{public}s", __func__, iter->c_str());
        }
    }

    HDF_LOGD("%{public}s: exit", __func__);
}

int32_t BatteryLed::InitLedsSysfs()
{
    HDF_LOGI("%{public}s enter", __func__);
    DIR* dir = nullptr;
    struct dirent* entry = nullptr;
    int32_t index = 0;
    int maxSize = 64;

    dir = opendir(LEDS_BASE_PATH.c_str());
    if (dir == nullptr) {
        HDF_LOGE("%{public}s: leds base path is not exist", __func__);
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
            HDF_LOGD("%{public}s: init leds info of %{public}s", __func__, entry->d_name);
            if (index >= maxSize) {
                HDF_LOGE("%{public}s: too many leds types", __func__);
                break;
            }
            g_ledsNodeName.emplace_back(entry->d_name);
            index++;
        }
    }

    TraversalNode();
    HDF_LOGD("%{public}s: index is %{public}d", __func__, index);
    closedir(dir);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

void BatteryLed::TurnOffLed()
{
    HDF_LOGI("%{public}s enter", __func__);
    WriteLedInfoToSys(0, 0, 0);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryLed::UpdateLedColor(const int32_t& chargestate, const int32_t& capacity)
{
    HDF_LOGI("%{public}s enter", __func__);
    if ((chargestate == PowerSupplyProvider::CHARGE_STATE_NONE) ||
        (chargestate == PowerSupplyProvider::CHARGE_STATE_RESERVED)) {
        TurnOffLed();
        HDF_LOGD("%{public}s: reset led color.", __func__);
        return;
    }

    std::unique_ptr<BatteryConfig> batteryConfig = std::make_unique<BatteryConfig>();
    if (batteryConfig == nullptr) {
        HDF_LOGD("%{public}s: batteryConfig is nullptr", __func__);
        return;
    }
    batteryConfig->Init();

    auto ledConf = batteryConfig->GetLedConf();
    for (auto it = ledConf.begin(); it != ledConf.end(); ++it) {
        HDF_LOGD("%{public}s: ledConf.begin()=%{public}d, ledConf.end()=%{public}d", __func__, \
            it->capacityBegin, it->capacityEnd);
        if ((capacity >= it->capacityBegin) && (capacity < it->capacityEnd)) {
            switch (it->color) {
                case (LED_COLOR_GREEN): {
                    HDF_LOGD("%{public}s: led color display green.", __func__);
                    WriteLedInfoToSys(0, it->brightness, 0);
                    break;
                }
                case (LED_COLOR_RED): {
                    HDF_LOGD("%{public}s: led color display red.", __func__);
                    WriteLedInfoToSys(it->brightness, 0, 0);
                    break;
                }
                case (LED_COLOR_YELLOW): {
                    HDF_LOGD("%{public}s: led color display yellow.", __func__);
                    WriteLedInfoToSys(it->brightness, it->brightness, 0);
                    break;
                }
                default: {
                    HDF_LOGD("%{public}s: led color display error.", __func__);
                    break;
                }
            }
            break;
        }

        if (capacity == CAPACITY_FULL) {
            HDF_LOGD("%{public}s: led color display green.", __func__);
            WriteLedInfoToSys(0, it->brightness, 0);
            break;
        }
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryLed::WriteLedInfoToSys(const int redbrightness, const int greenbrightness, const int bluebrightness)
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE* file = nullptr;
    std::string redLedPath = LEDS_BASE_PATH + "/" + g_redLedsNode + "/" + "brightness";
    std::string greenLedPath = LEDS_BASE_PATH + "/" + g_greenLedsNode + "/" + "brightness";
    std::string blueLedPath = LEDS_BASE_PATH + "/" + g_blueLedsNode + "/" + "brightness";
    HDF_LOGD("%{public}s: redLedPath is %{public}s, greenLedPath is %{public}s, blueLedPath is %{public}s", __func__,
        redLedPath.c_str(), greenLedPath.c_str(), blueLedPath.c_str());
    InitMockLedFile(redLedPath, greenLedPath, blueLedPath);

    file = fopen(redLedPath.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: red led file open failed. redLedPath is %{public}s", __func__, redLedPath.c_str());
        return;
    }
    int ret = fprintf(file, "%d\n", redbrightness);
    if (ret < 0) {
        HDF_LOGD("%{public}s: red led file fprintf failed.", __func__);
    }
    ret = fclose(file);
    if (ret < 0) {
        return;
    }

    file = fopen(greenLedPath.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: green led file open failed. greenLedPath is %{public}s", __func__, greenLedPath.c_str());
        return;
    }
    ret = fprintf(file, "%d\n", greenbrightness);
    if (ret < 0) {
        HDF_LOGD("%{public}s: green led file fprintf failed.", __func__);
    }
    ret = fclose(file);
    if (ret < 0) {
        return;
    }

    file = fopen(blueLedPath.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: blue led file open failed.", __func__);
        return;
    }
    ret = fprintf(file, "%d\n", bluebrightness);
    if (ret < 0) {
        HDF_LOGD("%{public}s: blue led file fprintf failed. blueLedPath is %{public}s", __func__, blueLedPath.c_str());
    }
    ret = fclose(file);
    if (ret < 0) {
        return;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

std::string BatteryLed::CreateFile(std::string path, std::string content) const
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

void BatteryLed::InitMockLedFile(std::string& redPath, std::string& greenPath, std::string& bluePath) const
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string mockLedsPath = "/data/local/tmp/leds";
    std::string sysLedsPath = "/sys/class/leds";
    std::string redLedPath = "/data/local/tmp/leds/sc27xx:red";
    std::string greenLedPath = "/data/local/tmp/leds/sc27xx:green";
    std::string blueLedPath = "/data/local/tmp/leds/sc27xx:blue";

    if (access(sysLedsPath.c_str(), F_OK) == 0) {
        HDF_LOGD("%{public}s: system leds path exist.", __func__);
        return;
    } else {
        redPath = "/data/local/tmp/leds/sc27xx:red/brightness";
        greenPath = "/data/local/tmp/leds/sc27xx:green/brightness";
        bluePath = "/data/local/tmp/leds/sc27xx:blue/brightness";
    }

    if (access(mockLedsPath.c_str(), 0) == -1) {
        int ret = mkdir("/data/local/tmp/leds", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create leds path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    InitRedLedPath(redLedPath);
    InitGreenLedPath(greenLedPath);
    InitBlueLedPath(blueLedPath);

    HDF_LOGE("%{public}s: create mock path for Hi3516DV300", __func__);
    CreateFile("/data/local/tmp/leds/sc27xx:red/brightness", "0");
    CreateFile("/data/local/tmp/leds/sc27xx:green/brightness", "0");
    CreateFile("/data/local/tmp/leds/sc27xx:blue/brightness", "0");
}

void BatteryLed::InitRedLedPath(std::string& redLedPath) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (access(redLedPath.c_str(), 0) == -1) {
        int ret = mkdir("/data/local/tmp/leds/sc27xx:red", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create red led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    HDF_LOGI("%{public}s exit", __func__);
}

void BatteryLed::InitGreenLedPath(std::string& greenLedPath) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (access(greenLedPath.c_str(), 0) == -1) {
        int ret = mkdir("/data/local/tmp/leds/sc27xx:green", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create green led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    HDF_LOGI("%{public}s exit", __func__);
}

void BatteryLed::InitBlueLedPath(std::string& blueLedPath) const
{
    HDF_LOGI("%{public}s enter", __func__);
    if (access(blueLedPath.c_str(), 0) == -1) {
        int ret = mkdir("/data/local/tmp/leds/sc27xx:blue", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create blue led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    HDF_LOGI("%{public}s exit", __func__);
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

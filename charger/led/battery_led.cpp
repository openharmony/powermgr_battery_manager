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
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define Hi3516DV300

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
#ifdef Hi3516DV300
static const std::string LED_RED_PATH = "/data/local/tmp/leds/sc27xx:red/brightness";
static const std::string LED_GREEN_PATH = "/data/local/tmp/leds/sc27xx:green/brightness";
static const std::string LED_BLUE_PATH = "/data/local/tmp/leds/sc27xx:blue/brightness";
#else
static const std::string LED_RED_PATH = "/sys/class/leds/sc27xx:red/brightness";
static const std::string LED_GREEN_PATH = "/sys/class/leds/sc27xx:green/brightness";
static const std::string LED_BLUE_PATH = "/sys/class/leds/sc27xx:blue/brightness";
#endif
static const int CAPACITY_FULL = 100;
static const int MKDIR_WAIT_TIME = 1;
static const int LED_COLOR_GREEN = 2;
static const int LED_COLOR_RED = 4;
static const int LED_COLOR_YELLOW = 6;

void BatteryLed::TurnOffLed()
{
    HDF_LOGI("%{public}s enter", __func__);
    WriteLedInfoToSys(0, 0, 0);

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

void BatteryLed::UpdateLedColor(const int32_t &chargestate, const int32_t &capacity)
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
    FILE *file = nullptr;

#ifdef Hi3516DV300
    InitMockLedFile();
#endif
    file = fopen(LED_RED_PATH.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: red led file open failed.", __func__);
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

    file = fopen(LED_GREEN_PATH.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: green led file open failed.", __func__);
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

    file = fopen(LED_BLUE_PATH.c_str(), "w");
    if (file == nullptr) {
        HDF_LOGD("%{public}s: blue led file open failed.", __func__);
        return;
    }
    ret = fprintf(file, "%d\n", bluebrightness);
    if (ret < 0) {
        HDF_LOGD("%{public}s: blue led file fprintf failed.", __func__);
    }
    ret = fclose(file);
    if (ret < 0) {
        return;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}

const char *BatteryLed::CreateFile(const char *path, const char *content)
{
    HDF_LOGI("%{public}s enter", __func__);
    std::ofstream stream(path);
    if (!stream.is_open()) {
        HDF_LOGD("%{public}s: Cannot create file %{public}s", __func__, path);
        return nullptr;
    }
    stream << content << std::endl;
    stream.close();
    return path;
}

void BatteryLed::InitMockLedFile()
{
    HDF_LOGI("%{public}s enter", __func__);
    std::string redLedPath = "/data/local/tmp/leds/sc27xx:red";
    std::string greenLedPath = "/data/local/tmp/leds/sc27xx:green";
    std::string blueLedPath = "/data/local/tmp/leds/sc27xx:blue";
    std::string ledsPath = "/data/local/tmp/leds";
    int ret = 0;

    if (access(ledsPath.c_str(), 0) == -1) {
        ret = mkdir("/data/local/tmp/leds", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create leds path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(redLedPath.c_str(), 0) == -1) {
        ret = mkdir("/data/local/tmp/leds/sc27xx:red", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create red led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(greenLedPath.c_str(), 0) == -1) {
        ret = mkdir("/data/local/tmp/leds/sc27xx:green", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create green led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }

    if (access(blueLedPath.c_str(), 0) == -1) {
        ret = mkdir("/data/local/tmp/leds/sc27xx:blue", S_IRWXU);
        if (ret == -1) {
            HDF_LOGD("%{public}s: create blue led path fail.", __func__);
            return;
        }
        sleep(MKDIR_WAIT_TIME);
    }
    HDF_LOGE("%{public}s: create mock path for Hi3516DV300", __func__);
    CreateFile("/data/local/tmp/leds/sc27xx:red/brightness", "0");
    CreateFile("/data/local/tmp/leds/sc27xx:green/brightness", "0");
    CreateFile("/data/local/tmp/leds/sc27xx:blue/brightness", "0");
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

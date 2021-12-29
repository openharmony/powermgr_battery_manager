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

#include "battery_vibrate.h"

#include <hdf_log.h>
#include <hdf_base.h>
#include <unistd.h>
#include "sys/stat.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const std::string VIBRATOR_PLAYMODE_PATH = "/sys/class/leds/vibrator/play_mode";
const std::string VIBRATOR_DURATIONMODE_PATH = "/sys/class/leds/vibrator/duration";
const std::string VIBRATOR_ACTIVATEMODE_PATH = "/sys/class/leds/vibrator/activate";
const int VIBRATION_PLAYMODE = 0;
const int VIBRATION_DURATIONMODE = 1;
const int VIBRATE_DELAY_MS = 5;
const int USEC_TO_MSEC = 1000;

int BatteryVibrate::VibrateInit()
{
    HDF_LOGI("%{public}s enter", __func__);
    struct stat st {};

    if (!stat(VIBRATOR_PLAYMODE_PATH.c_str(), &st)) {
        HDF_LOGD("%{public}s: vibrate path is play mode path", __func__);
        vibrateMode_ = VIBRATION_PLAYMODE;
        return 0;
    }

    if (!stat(VIBRATOR_DURATIONMODE_PATH.c_str(), &st)) {
        HDF_LOGD("%{public}s: vibrate path is duration path", __func__);
        vibrateMode_ = VIBRATION_DURATIONMODE;
        return 0;
    }

    HDF_LOGI("%{public}s exit", __func__);
    return -1;
}

FILE* BatteryVibrate::HandlePlayModePath() const
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE* file = nullptr;

    file = fopen(VIBRATOR_PLAYMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return nullptr;
    }
    if (fprintf(file, "%s\n", "direct") < 0) {
        HDF_LOGD("%{public}s: fprintf direct failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return nullptr;
    }

    return file;
}

void BatteryVibrate::HandlePlayMode(const int time) const
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE* file = nullptr;

    file = HandlePlayModePath();
    if (file == nullptr) {
        return;
    }

    file = fopen(VIBRATOR_DURATIONMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%d\n", time) < 0) {
        HDF_LOGD("%{public}s: fprintf time failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }

    file = fopen(VIBRATOR_ACTIVATEMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%d\n", 1) < 0) {
        HDF_LOGD("%{public}s: fprintf 1 failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }

    usleep((time + VIBRATE_DELAY_MS) * USEC_TO_MSEC);
    file = fopen(VIBRATOR_PLAYMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%s\n", "audio") < 0) {
        HDF_LOGD("%{public}s: fprintf audio failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }
}

void BatteryVibrate::HandleDurationMode(const int time) const
{
    HDF_LOGI("%{public}s enter", __func__);
    FILE* file = nullptr;

    file = fopen(VIBRATOR_DURATIONMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%d\n", time) < 0) {
        HDF_LOGD("%{public}s: fprintf time failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }

    file = fopen(VIBRATOR_ACTIVATEMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%d\n", 1) < 0) {
        HDF_LOGD("%{public}s: fprintf 1 failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }

    usleep((time + VIBRATE_DELAY_MS) * USEC_TO_MSEC);
    file = fopen(VIBRATOR_ACTIVATEMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        return;
    }
    if (fprintf(file, "%d\n", 0) < 0) {
        HDF_LOGD("%{public}s: fprintf 0 failed.", __func__);
    }
    if (fclose(file) < 0) {
        HDF_LOGD("%{public}s: fclose failed.", __func__);
        return;
    }
}

void BatteryVibrate::HandleVibrate(const int time)
{
    HDF_LOGI("%{public}s enter", __func__);
    switch (vibrateMode_) {
        case VIBRATION_PLAYMODE: {
            HDF_LOGD("%{public}s: vibrate mode sysfs1", __func__);
            HandlePlayMode(time);
            break;
        }
        case VIBRATION_DURATIONMODE: {
            HDF_LOGD("%{public}s: vibrate mode sysfs2", __func__);
            HandleDurationMode(time);
            break;
        }
        default: {
            HDF_LOGD("%{public}s: vibrate mode unknown", __func__);
            break;
        }
    }

    HDF_LOGI("%{public}s exit", __func__);
    return;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

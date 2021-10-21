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

#include "battery_config.h"

#include <hdf_log.h>
#include <hdf_base.h>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
const std::string CONFIG_FILE = "/system/etc/ledconfig/led_config.json";
static const int DEFAULT_CAPACITY_CONF = 3;
static const int DEFAULT_UPPER_TEMP_CONF = 600;
static const int DEFAULT_LOWER_TEMP_CONF = -100;
static const int DEFAULT_CAPACITY_BEGIN_CONF = 0;
static const int DEFAULT_CAPACITY_END_CONF = 100;
static const int DEFAULT_LED_COLOR_CONF = 4;
static const int DEFAULT_BRIGHTNESS_CONF = 255;

int32_t BatteryConfig::Init()
{
    HDF_LOGI("%{public}s enter", __func__);

    return ParseConfig(CONFIG_FILE);
}

std::vector<BatteryConfig::LedConf> BatteryConfig::GetLedConf()
{
    HDF_LOGI("%{public}s enter", __func__);

    return ledConf_;
}

BatteryConfig::TempConf BatteryConfig::GetTempConf()
{
    HDF_LOGI("%{public}s enter", __func__);

    return tempConf_;
}

int BatteryConfig::GetCapacityConf()
{
    HDF_LOGI("%{public}s enter", __func__);

    return capacityConf_;
}

int32_t BatteryConfig::ParseLedConf(Json::Value &root)
{
    HDF_LOGI("%{public}s enter", __func__);
    struct LedConf ledConf;
    int size = root["led"]["table"].size();
    HDF_LOGD("%{public}s: size = %{public}d", __func__, size);
    if (size == 0) {
        HDF_LOGD("%{public}s: read json file fail.", __func__);
        ledConf.capacityBegin = DEFAULT_CAPACITY_BEGIN_CONF;
        ledConf.capacityEnd = DEFAULT_CAPACITY_END_CONF;
        ledConf.color = DEFAULT_LED_COLOR_CONF;
        ledConf.brightness = DEFAULT_BRIGHTNESS_CONF;
        ledConf_.emplace_back(ledConf);
        return HDF_ERR_INVALID_OBJECT;
    }
    ledConf_.clear();

    for (int i = 0; i < size; ++i) {
        ledConf.capacityBegin = root["led"]["table"][i][INDEX_ZERO].asInt();
        ledConf.capacityEnd = root["led"]["table"][i][INDEX_ONE].asInt();
        ledConf.color = root["led"]["table"][i][INDEX_TWO].asInt();
        ledConf.brightness = root["led"]["table"][i][INDEX_THREE].asInt();
        HDF_LOGD("%{public}s: capacityBegin= %{public}d, capacityEnd=%{public}d, color=%{public}d, \
            brightness=%{public}d", __func__, ledConf.capacityBegin, ledConf.capacityEnd, ledConf.color, \
            ledConf.brightness);
        ledConf_.emplace_back(ledConf);
    }

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryConfig::ParseTempConf(Json::Value &root)
{
    HDF_LOGI("%{public}s enter", __func__);
    int size = root["temperature"]["table"].size();
    if (size == 0) {
        HDF_LOGD("%{public}s parse temperature config file fail.", __func__);
        tempConf_.lower = DEFAULT_LOWER_TEMP_CONF;
        tempConf_.upper = DEFAULT_UPPER_TEMP_CONF;
        return HDF_ERR_INVALID_OBJECT;
    }

    tempConf_.lower = root["temperature"]["table"][INDEX_ZERO].asInt();
    tempConf_.upper = root["temperature"]["table"][INDEX_ONE].asInt();
    HDF_LOGD("%{public}s: tempConf_.lower=%{public}d, tempConf_.upper=%{public}d", __func__, \
        tempConf_.lower, tempConf_.upper);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryConfig::ParseCapacityConf(Json::Value &root)
{
    HDF_LOGI("%{public}s enter", __func__);
    int size = root["soc"]["table"].size();
    if (size == 0) {
        HDF_LOGD("%{public}s parse capacity config file fail.", __func__);
        capacityConf_ = DEFAULT_CAPACITY_CONF;
        return HDF_ERR_INVALID_OBJECT;
    }

    capacityConf_ = root["soc"]["table"][INDEX_ZERO].asInt();
    HDF_LOGD("%{public}s: capacityConf_ = %{public}d", __func__, capacityConf_);

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}

int32_t BatteryConfig::ParseConfig(const std::string filename)
{
    HDF_LOGI("%{public}s enter", __func__);
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;

    std::ifstream ledConfig;
    ledConfig.open(filename);

    root.clear();
    readerBuilder["collectComments"] = false;
    JSONCPP_STRING errs;

    if (parseFromStream(readerBuilder, ledConfig, &root, &errs)) {
        int32_t ret = ParseLedConf(root);
        if (ret != HDF_SUCCESS) {
            HDF_LOGD("%{public}s: parse led config fail. ", __func__);
        }

        ret = ParseTempConf(root);
        if (ret != HDF_SUCCESS) {
            HDF_LOGD("%{public}s: parse temperature config fail. ", __func__);
        }

        ret = ParseCapacityConf(root);
        if (ret != HDF_SUCCESS) {
            HDF_LOGD("%{public}s: parse soc config fail. ", __func__);
        }
    }

    ledConfig.close();

    HDF_LOGI("%{public}s exit", __func__);
    return HDF_SUCCESS;
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

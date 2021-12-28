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

#ifndef BATTERY_CONFIG_H
#define BATTERY_CONFIG_H

#include <string>
#include <vector>
#include <fstream>
#include <json/json.h>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryConfig {
public:
    struct LedConf {
        int capacityBegin;
        int capacityEnd;
        int color;
        int brightness;
    };

    struct TempConf {
        int lower;
        int upper;
    };

    int32_t Init();
    std::vector<LedConf> GetLedConf();
    BatteryConfig::TempConf GetTempConf();
    int GetCapacityConf();

private:
    enum JsonConfIndex {
        INDEX_ZERO = 0,
        INDEX_ONE,
        INDEX_TWO,
        INDEX_THREE,
    };

    int32_t ParseLedConf(Json::Value& root);
    int32_t ParseTempConf(Json::Value& root);
    int32_t ParseCapacityConf(Json::Value& root);
    int32_t ParseConfig(const std::string filename);
    std::vector<BatteryConfig::LedConf> ledConf_;
    struct TempConf tempConf_;
    int capacityConf_ = -1;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif

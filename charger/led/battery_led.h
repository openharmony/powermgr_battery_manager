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

#ifndef BATTERY_LED_H
#define BATTERY_LED_H

#include "power_supply_provider.h"
#include "battery_config.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryLed {
public:
    int32_t InitLedsSysfs();
    void TurnOffLed();
    void WriteLedInfoToSys(const int redbrightness, const int greenbrightness, const int bluebrightness);
    void UpdateLedColor(const int32_t& chargestate, const int32_t& capacity);
private:
    void InitMockLedFile(std::string& redPath, std::string& greenPath, std::string& bluePath) const;
    void TraversalNode();
    void InitRedLedPath(std::string& redLedPath) const;
    void InitGreenLedPath(std::string& greenLedPath) const;
    void InitBlueLedPath(std::string& blueLedPath) const;
    std::string CreateFile(std::string path, std::string content) const;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif

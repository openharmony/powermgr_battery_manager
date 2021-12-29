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

#ifndef BATTERY_BACKLIGHT_H
#define BATTERY_BACKLIGHT_H

#include <string>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryBacklight {
public:
    BatteryBacklight();
    int32_t InitBacklightSysfs();
    void TurnOnScreen();
    void TurnOffScreen();
    int HandleBacklight(const unsigned int backlight) const;
    bool GetScreenState() const;
private:
    std::string CreateFile(std::string path, std::string content) const;
    void InitDevicePah(std::string& path) const;
    void InitDefaultSysfs(void) const;
    void TraversalBacklightNode();
    bool screenOn_ = false;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif

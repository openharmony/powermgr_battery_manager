/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "test_utils.h"

#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "battery_log.h"

using namespace std;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* MOCK_PATH = "/data/service/el0/battery";
constexpr const char* POWER_SUPPLY_PATH = "/sys/class/power_supply";
vector<string> MOCK_DIR_NAME = {
    "battery",
    "USB",
    "Wireless",
    "Mains",
    "ohos_charger",
    "ohos-fgu"
};

vector<vector<string>> MOCK_FILE_NAME = {
    { "capacity", "voltage_now", "temp", "health", "status", "present", "charge_counter", "technology" },
    { "type", "online", "current_max", "voltage_max" },
    { "type", "online" },
    { "type", "online" },
    { "type", "online" },
    { "type", "online" }
};
}

void TestUtils::WriteMock(const std::string& path, const std::string content)
{
    std::ofstream stream(path.c_str());
    if (!stream.is_open()) {
        BATTERY_HILOGI(LABEL_TEST, "Cannot create file");
        return;
    }
    stream << content.c_str() << std::endl;
    stream.close();
}

void TestUtils::InitTest()
{
    mkdir(MOCK_PATH, S_IRWXU);
    for (size_t i = 0; i < MOCK_DIR_NAME.size(); ++i) {
        mkdir((std::string(MOCK_PATH) + "/" + MOCK_DIR_NAME[i]).c_str(), S_IRWXU);
    }
}

void TestUtils::ResetOnline()
{
    for (size_t dInd = 0; dInd < MOCK_DIR_NAME.size(); ++dInd) {
        for (size_t fInd = 0; fInd < MOCK_FILE_NAME[dInd].size(); ++fInd) {
            if (MOCK_FILE_NAME[dInd][fInd] == "online") {
                std::string file = std::string(MOCK_PATH) + "/" + MOCK_DIR_NAME[dInd] + "/" + "online";
                WriteMock(file, "0");
            }
        }
    }
}

bool TestUtils::IsMock()
{
    DIR* dir = opendir(POWER_SUPPLY_PATH);
    if (dir == NULL) {
        return true;
    }
    struct dirent* ptr = NULL;
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(".", ptr->d_name) != 0 && strcmp("..", ptr->d_name) != 0) {
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}
} // namespace PowerMgr
} // namespace OHOS

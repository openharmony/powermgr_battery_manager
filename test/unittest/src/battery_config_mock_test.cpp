/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "battery_config_mock_test.h"

#include <string>

#include "battery_config.h"
#include "battery_log.h"
#include "config_policy_utils.h"

using namespace testing::ext;
namespace {
bool g_returnBool = false;
char* g_returnCharPtr = nullptr;
}

char* GetOneCfgFile(const char *pathSuffix, char *buf, unsigned int bufLength)
{
    return g_returnCharPtr;
}

bool Json::parseFromStream(
    Json::CharReader::Factory const& fact, Json::IStream& sin, Json::Value* root, Json::String* errs)
{
    return g_returnBool;
}

namespace OHOS {
namespace PowerMgr {
namespace {
BatteryConfig& g_configTest = BatteryConfig::GetInstance();
} // namespace

/**
 * @tc.name: BatteryConfigMock001
 * @tc.desc: Test ParseConfig
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigMockTest, BatteryConfigMock001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfigMock001 function start!");
    bool ret = g_configTest.ParseConfig();
    EXPECT_TRUE(ret);
    static char nullChar = '\0';
    g_returnCharPtr = &nullChar;
    g_returnBool = true;
    ret = g_configTest.ParseConfig();
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfigMock001 function end!");
}
} // namespace PowerMgr
} // namespace OHOS

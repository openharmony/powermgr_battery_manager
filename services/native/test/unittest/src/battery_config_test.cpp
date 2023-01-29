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

#include "battery_config_test.h"

#include <string>

#include "battery_config.h"
#include "battery_log.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* SYSTEM_BATTERY_CONFIG_PATH = "/system/etc/battery/battery_config.json";
auto& g_configTest = BatteryConfig::GetInstance();
} // namespace

/**
 * @tc.name: BatteryConfig001
 * @tc.desc: Parse config, and configPath parameter is real path
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig001 begin");
    EXPECT_TRUE(g_configTest.ParseConfig(SYSTEM_BATTERY_CONFIG_PATH));
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig001 end");
}

/**
 * @tc.name: BatteryConfig002
 * @tc.desc: Get battery light config
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig002, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig002 begin");
    const std::vector<BatteryConfig::LightConf> lightConf = g_configTest.GetLightConf();
    EXPECT_TRUE(lightConf.size());

    uint32_t maxRgb = (255 << 16) | (255 << 8) | 255;
    for (uint32_t i = 0; i < lightConf.size(); ++i) {
        // The value ranges from 0 to 100
        EXPECT_TRUE(lightConf[i].beginSoc >= 0 && lightConf[i].beginSoc <= 100);
        EXPECT_TRUE(lightConf[i].endSoc >= 0 && lightConf[i].endSoc <= 100);
        // The start range is smaller than the end range
        EXPECT_TRUE(lightConf[i].beginSoc < lightConf[i].endSoc);
        // The value ranges from 0 to maxRgb
        EXPECT_TRUE(lightConf[i].rgb >= 0 && lightConf[i].rgb <= maxRgb);
    }
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig002 end");
}

/**
 * @tc.name: BatteryConfig003
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig003, TestSize.Level1)
{
    std::string key = "soc.warning";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig003 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t invalid = -1;
    int32_t warnCapacity = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig003 warnCapacity=%{public}d", warnCapacity);
    // The value ranges from 0 to 100
    EXPECT_TRUE(warnCapacity >= 0 && warnCapacity <= 100);
}

/**
 * @tc.name: BatteryConfig004
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig004, TestSize.Level1)
{
    std::string key = "temperature.high";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig004 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t minTemp = -900; // (-90℃)
    int32_t maxTemp = 900;  // (90℃)
    int32_t highTemperature = g_configTest.GetInt(key, maxTemp);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig004 highTemperature=%{public}d", highTemperature);
    // The value ranges from -900 to 900
    EXPECT_TRUE(highTemperature > minTemp && highTemperature < maxTemp);
}

/**
 * @tc.name: BatteryConfig005
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig005, TestSize.Level1)
{
    std::string key = "temperature.low";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig005 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t minTemp = -900; // (-90℃)
    int32_t maxTemp = 900;  // (90℃)
    int32_t lowTemperature = g_configTest.GetInt(key, minTemp);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig005 lowTemperature=%{public}d", lowTemperature);
    // The value ranges from -900 to 900
    EXPECT_TRUE(lowTemperature < maxTemp && lowTemperature > minTemp);
}

/**
 * @tc.name: BatteryConfig006
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig006, TestSize.Level1)
{
    std::string key = "soc.shutdown";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig006 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t invalid = -1;
    int32_t shtdwonCapacity = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig006 shtdwonCapacity=%{public}d", shtdwonCapacity);
    // The value ranges from 0 to 100
    EXPECT_TRUE(shtdwonCapacity >= 0 && shtdwonCapacity <= 100);
}

/**
 * @tc.name: BatteryConfig007
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig007, TestSize.Level1)
{
    std::string key = "soc.low";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig007 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t invalid = -1;
    int32_t low_battery_event = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig007 low_battery_event=%{public}d", low_battery_event);
    // The value ranges from 0 to 100
    EXPECT_TRUE(low_battery_event >= 0 && low_battery_event <= 100);
}

/**
 * @tc.name: BatteryConfig008
 * @tc.desc: Get unknown configuration, return default value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig008 begin");
    int32_t defValue = 100;
    EXPECT_EQ(defValue, g_configTest.GetInt("XXXXXXXXX", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig008 end");
}

/**
 * @tc.name: BatteryConfig009
 * @tc.desc: Get a maximum nesting depth of 5 or more
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig009 begin");
    int32_t defValue = 200;
    EXPECT_EQ(defValue, g_configTest.GetInt("X.X.X.X.X.X", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig009 end");
}

/**
 * @tc.name: BatteryConfig010
 * @tc.desc: Get empty configuration, return default value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig010 begin");
    int32_t defValue = 300;
    EXPECT_EQ(defValue, g_configTest.GetInt("", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig010 end");
}

/**
 * @tc.name: BatteryConfig0011
 * @tc.desc: Parse config, and configPath parameter is empty
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig011, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig0011 begin");
    EXPECT_TRUE(g_configTest.ParseConfig(""));
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig0011 end");
}

/**
 * @tc.name: BatteryConfig012
 * @tc.desc: Get config Int value, paramter is real key
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig012, TestSize.Level1)
{
    std::string key = "light.high.soc";
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGD(LABEL_TEST, "BatteryConfig012 %{public}s does not exist", key.c_str());
        return;
    }
    int32_t defVal = 90;
    int32_t highSoc = g_configTest.GetInt(key, defVal);
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig012 highSoc=%{public}d", highSoc);
    // The value ranges from 0 to 100
    EXPECT_TRUE(highSoc >= 0 && highSoc <= 100);
}

/**
 * @tc.name: BatteryConfig0013
 * @tc.desc: Get config Int value, paramter is invalid key
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig013, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig013 begin");
    std::string key = "invalid.key";
    EXPECT_TRUE(!g_configTest.IsExist(key));
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig013 end");
}
} // namespace PowerMgr
} // namespace OHOS
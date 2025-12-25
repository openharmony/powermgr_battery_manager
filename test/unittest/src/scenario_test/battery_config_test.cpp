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

#include "battery_config_test.h"

#ifdef GTEST
#define private   public
#define protected public
#endif
#include "battery_config.h"
#include "battery_log.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
auto& g_configTest = BatteryConfig::GetInstance();
}

void BatteryConfigTest::DestroyJsonValue(cJSON*& value)
{
    if (value) {
        cJSON_Delete(value);
        value = nullptr;
    }
}

bool BatteryConfigTest::ParseJsonStr(const std::string& jsonStr, bool isAssignToConfig)
{
    cJSON* parseResult = cJSON_Parse(jsonStr.c_str());
    if (!parseResult) {
        return false;
    }
    if (isAssignToConfig) {
        g_configTest.config_ = parseResult;
    }
    return true;
}

/**
 * @tc.name: BatteryConfig001
 * @tc.desc: Parse config, and configPath parameter is real path
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig001 function start!");
    EXPECT_TRUE(g_configTest.ParseConfig());
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig001 function end!");
}

/**
 * @tc.name: BatteryConfig002
 * @tc.desc: Get battery light config
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig002 function start!");
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
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig002 function end!");
}

/**
 * @tc.name: BatteryConfig003
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig003 function start!");
    std::string key = "soc.warning";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t invalid = -1;
    int32_t warnCapacity = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig003 warnCapacity=%{public}d", warnCapacity);
    // The value ranges from 0 to 100
    EXPECT_TRUE(warnCapacity >= 0 && warnCapacity <= 100);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig003 function end!");
}

/**
 * @tc.name: BatteryConfig004
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig004 function start!");
    std::string key = "temperature.high";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t minTemp = -900; // (-90℃)
    int32_t maxTemp = 900;  // (90℃)
    int32_t highTemperature = g_configTest.GetInt(key, maxTemp);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig004 highTemperature=%{public}d", highTemperature);
    // The value ranges from -900 to 900
    EXPECT_TRUE(highTemperature > minTemp && highTemperature < maxTemp);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig004 function end!");
}

/**
 * @tc.name: BatteryConfig005
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig005 function start!");
    std::string key = "temperature.low";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t minTemp = -900; // (-90℃)
    int32_t maxTemp = 900;  // (90℃)
    int32_t lowTemperature = g_configTest.GetInt(key, minTemp);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig005 lowTemperature=%{public}d", lowTemperature);
    // The value ranges from -900 to 900
    EXPECT_TRUE(lowTemperature < maxTemp && lowTemperature > minTemp);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig005 function end!");
}

/**
 * @tc.name: BatteryConfig006
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig006 function start!");
    std::string key = "soc.shutdown";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t invalid = -1;
    int32_t shtdwonCapacity = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig006 shtdwonCapacity=%{public}d", shtdwonCapacity);
    // The value ranges from 0 to 100
    EXPECT_TRUE(shtdwonCapacity >= 0 && shtdwonCapacity <= 100);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig006 function end!");
}

/**
 * @tc.name: BatteryConfig007
 * @tc.desc: Get config Int value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig007 function start!");
    std::string key = "soc.low";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t invalid = -1;
    int32_t low_battery_event = g_configTest.GetInt(key, invalid);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig007 low_battery_event=%{public}d", low_battery_event);
    // The value ranges from 0 to 100
    EXPECT_TRUE(low_battery_event >= 0 && low_battery_event <= 100);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig007 function end!");
}

/**
 * @tc.name: BatteryConfig008
 * @tc.desc: Get unknown configuration, return default value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig008 function start!");
    int32_t defValue = 100;
    EXPECT_EQ(defValue, g_configTest.GetInt("XXXXXXXXX", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig008 function end!");
}

/**
 * @tc.name: BatteryConfig009
 * @tc.desc: Get a maximum nesting depth of 5 or more
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig009 function start!");
    int32_t defValue = 200;
    EXPECT_EQ(defValue, g_configTest.GetInt("X.X.X.X.X.X", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig009 function end!");
}

/**
 * @tc.name: BatteryConfig010
 * @tc.desc: Get empty configuration, return default value
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig010 function start!");
    int32_t defValue = 300;
    EXPECT_EQ(defValue, g_configTest.GetInt("", defValue));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig010 function end!");
}

/**
 * @tc.name: BatteryConfig0011
 * @tc.desc: Parse config, and configPath parameter is empty
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig011, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0011 function start!");
    EXPECT_TRUE(g_configTest.ParseConfig());
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0011 function end!");
}

/**
 * @tc.name: BatteryConfig012
 * @tc.desc: Get config Int value, paramter is real key
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig012, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0012 function start!");
    std::string key = "light.high.soc";
    ASSERT_TRUE(g_configTest.IsExist(key));
    int32_t defVal = 90;
    int32_t highSoc = g_configTest.GetInt(key, defVal);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig012 highSoc=%{public}d", highSoc);
    // The value ranges from 0 to 100
    EXPECT_TRUE(highSoc >= 0 && highSoc <= 100);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0012 function end!");
}

/**
 * @tc.name: BatteryConfig0013
 * @tc.desc: Get config Int value, paramter is invalid key
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryConfigTest, BatteryConfig013, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig013 function start!");
    std::string key = "invalid.key";
    EXPECT_TRUE(!g_configTest.IsExist(key));
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig013 function end!");
}

/**
 * @tc.name: BatteryConfig0014
 * @tc.desc: test ParseLightConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0014, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0014 function start!");
    std::string key = "low";
    g_configTest.lightConf_.clear();
    std::string jsonStr = R"({"light": {"low": {"soc": "soc", "rgb": []}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [], "rgb": "rgb"}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0014 function end!");
}

/**
 * @tc.name: BatteryConfig0015
 * @tc.desc: test ParseLightConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0015, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0015 function start!");
    std::string key = "low";
    g_configTest.lightConf_.clear();
    std::string jsonStr = R"({"light": {"low": {"soc": [], "rgb": []}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": ["0", 10], "rgb": []}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [0, "10"], "rgb": []}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": []}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": ["255", 0, 0]}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": [255, "0", 0]}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": [255, 0, "0"]}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0015 function end!");
}

/**
 * @tc.name: BatteryConfig0016
 * @tc.desc: test ParseWirelessChargerConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0016, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0016 function start!");
    std::string jsonStr = R"({"wirelesscharger": 0})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"wirelesscharger": null})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"wirelesscharger": "0"})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0016 function end!");
}

/**
 * @tc.name: BatteryConfig0017
 * @tc.desc: test ParseBootActionsConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0017, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0017 function start!");
    g_configTest.commonEventConf_.clear();
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    std::string jsonStr = R"({"boot_actions": ""})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0017 function end!");
}

/**
 * @tc.name: BatteryConfig0018
 * @tc.desc: test ParseCommonEventConf part1
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0018, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0018 function start!");
    g_configTest.ParseCommonEventConf(nullptr);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    std::string jsonStr = R"({"sendcommonevent": ""})";
    cJSON* parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": 123,
            "scene_config": { "name" : "wireless", "not_equal" : "0" },
            "uevent": "battery common event"
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : 123, "not_equal" : "0" },
            "uevent": "battery common event"
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "not_equal" : "0" },
            "uevent": 123
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0018 function end!");
}

/**
 * @tc.name: BatteryConfig0019
 * @tc.desc: test ParseCommonEventConf part2
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0019, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0019 function start!");
    std::string jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "equal" : "0" },
            "uevent": "battery common event"
        }]})";
    cJSON* parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 1);
    DestroyJsonValue(parseResult);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "equal" : null },
            "uevent": "battery common event"
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    DestroyJsonValue(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "not_equal" : null },
            "uevent": "battery common event"
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "equal" : 0 },
            "uevent": "battery common event"
        }]})";
    parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(parseResult);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(parseResult);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0019 function end!");
}

/**
 * @tc.name: BatteryConfig0020
 * @tc.desc: test ParsePopupConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0020, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0020 function start!");
    std::string jsonStr = R"({"popup": null})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.popupConfig_.clear();
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": "popup"})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": {"XXX": null}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": {"XXX": "XXX"}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": {"XXX": [{"name": 123}]}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": {"XXX": [{"name": "123", "action": "456"}]}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"popup": {"XXX": [{"name": "123", "action": 456}]}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0020 function end!");
}

/**
 * @tc.name: BatteryConfig0021
 * @tc.desc: test ParseNotificationConf part1
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0021, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0021 function start!");
    std::string jsonStr = R"({"notification": null})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.popupConfig_.clear();
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": "notification"})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": 1}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": 2}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": 3}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0021 function end!");
}

/**
 * @tc.name: BatteryConfig0022
 * @tc.desc: test ParseNotificationConf part2
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0022, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0022 function start!");
    std::string jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": 4}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.popupConfig_.clear();
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": 5}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": []}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": [1, {}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": [{}, 2]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": 1}, {}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0022 function end!");
}

/**
 * @tc.name: BatteryConfig0023
 * @tc.desc: test ParseNotificationConf part3
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0023, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0023 function start!");
    std::string jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": 1}, {}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.popupConfig_.clear();
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": 2}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": "2", "action": 2}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": "2", "action": "2"}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "bannerFlags": 512, "button": [{"name": "1", "action": "1"}, {"name": "2", "action": "2"}]}]})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0023 function end!");
}

/**
 * @tc.name: BatteryConfig0024
 * @tc.desc: test GetNotificationConf
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0024, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0024 function start!");
    auto& ntfMap = g_configTest.GetNotificationConf();
    EXPECT_TRUE(ntfMap.size() != 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0024 function end!");
}

/**
 * @tc.name: BatteryConfig0025
 * @tc.desc: test GetValue
 * @tc.type: FUNC
 * @tc.require: issueIBWNLX
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0025, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0025 function start!");
    std::string str = "";
    cJSON* jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE((!jsonValue || cJSON_IsNull(jsonValue)));
    str = "light.low.rbg.a.b.c";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE((!jsonValue || cJSON_IsNull(jsonValue)));
    std::string jsonStr = R"({"light": {"low": {"soc": "soc"}}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    str = "light.low.rbg";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_FALSE((jsonValue && cJSON_IsNull(jsonValue)));
    str = "light.low.soc";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE((strcmp(cJSON_GetStringValue(jsonValue), "soc") == 0));
    str = "light.low.soc.a";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE((strcmp(cJSON_GetStringValue(jsonValue), "soc") == 0));
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0025 function end!");
}

/**
 * @tc.name: BatteryConfig0026
 * @tc.desc: test ParseBootActionsConf
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0026, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0026 function start!");
    g_configTest.commonEventConf_.clear();
    std::string jsonStr = R"({"boot_actions": {"sendcommonevent": ""}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);

    jsonStr = R"({"boot_actions": null})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);

    jsonStr = R"({"boot_actions": {"sendcommonevent": null}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0026 function end!");
}

/**
 * @tc.name: BatteryConfig0027
 * @tc.desc: test ParsePopupConf
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig0027, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0027 function start!");
    g_configTest.popupConfig_.clear();
    std::string jsonStr = R"({"popup": {"XXX": [{"action": 456},{"name": "123"},{"name": 123, "action": 456}]}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    DestroyJsonValue(g_configTest.config_);

    jsonStr = R"({"popup": {"XXX": [{"name": "123", "action": "456"}]}})";
    ASSERT_TRUE(ParseJsonStr(jsonStr, true));
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    DestroyJsonValue(g_configTest.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0027 function end!");
}
} // namespace PowerMgr
} // namespace OHOS

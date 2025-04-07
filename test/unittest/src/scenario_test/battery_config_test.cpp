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
Json::Value root;
Json::CharReaderBuilder builder;
std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
std::string errors;
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig003 %{public}s does not exist", key.c_str());
        return;
    }
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig004 %{public}s does not exist", key.c_str());
        return;
    }
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig005 %{public}s does not exist", key.c_str());
        return;
    }
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig006 %{public}s does not exist", key.c_str());
        return;
    }
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig007 %{public}s does not exist", key.c_str());
        return;
    }
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
    if (!g_configTest.IsExist(key)) {
        BATTERY_HILOGI(LABEL_TEST, "BatteryConfig012 %{public}s does not exist", key.c_str());
        return;
    }
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [], "rgb": "rgb"}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": ["0", 10], "rgb": []}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [0, "10"], "rgb": []}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": []}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": ["255", 0, 0]}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": [255, "0", 0]}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
    jsonStr = R"({"light": {"low": {"soc": [0, 10], "rgb": [255, 0, "0"]}}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseLightConf(key);
    EXPECT_TRUE(g_configTest.lightConf_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
    jsonStr = R"({"wirelesscharger": null})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
    jsonStr = R"({"wirelesscharger": "0"})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseWirelessChargerConf();
    EXPECT_FALSE(g_configTest.wirelessChargerEnable_);
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
    g_configTest.config_.clear();
    g_configTest.commonEventConf_.clear();
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    std::string jsonStr = R"({"boot_actions": ""})";
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseBootActionsConf();
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
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
    root.clear();
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    std::string jsonStr = R"({"sendcommonevent": ""})";
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": 123,
            "scene_config": { "name" : "wireless", "not_equal" : "0" },
            "uevent": "battery common event"
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : 123, "not_equal" : "0" },
            "uevent": "battery common event"
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "not_equal" : "0" },
            "uevent": 123
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 1);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "equal" : null },
            "uevent": "battery common event"
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "not_equal" : null },
            "uevent": "battery common event"
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
    jsonStr = R"({"sendcommonevent": [{
            "event_name": "usual.event.BATTERY_CHANGED",
            "scene_config": { "name" : "wireless", "equal" : 0 },
            "uevent": "battery common event"
        }]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.ParseCommonEventConf(root);
    EXPECT_TRUE(g_configTest.commonEventConf_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    jsonStr = R"({"popup": "popup"})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    jsonStr = R"({"popup": {"XXX": null}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    jsonStr = R"({"popup": {"XXX": "XXX"}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 0);
    jsonStr = R"({"popup": {"XXX": [{"name": 123}]}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    jsonStr = R"({"popup": {"XXX": [{"name": "123", "action": "456"}]}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
    jsonStr = R"({"popup": {"XXX": [{"name": "123", "action": 456}]}})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParsePopupConf();
    EXPECT_TRUE(g_configTest.popupConfig_.size() == 1);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": "notification"})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": 1}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": 2}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": 3}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": 5}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": []}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": [1, {}]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4", "button": [{}, 2]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": 1}, {}]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
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
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": 2}]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": "2", "action": 2}]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 0);
    jsonStr = R"({"notification": [{"name": "1", "icon": "2", "title": "3", "text": "4",
        "button": [{"name": "1", "action": "1"}, {"name": "2", "action": "2"}]}]})";
    parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    g_configTest.ParseNotificationConf();
    EXPECT_TRUE(g_configTest.notificationConfMap_.size() == 1);
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
    Json::Value jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE(jsonValue.empty());
    str = "light.low.rbg.a.b.c";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE(jsonValue.empty());
    std::string jsonStr = R"({"light": {"low": {"soc": "soc"}}})";
    bool parseResult = reader->parse(jsonStr.c_str(), jsonStr.c_str() + jsonStr.size(), &root, &errors);
    EXPECT_TRUE(parseResult);
    g_configTest.config_ = root;
    str = "light.low.rbg";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_FALSE(jsonValue.empty());
    str = "light.low.soc";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE(jsonValue.asString() == "soc");
    str = "light.low.soc.a";
    jsonValue = g_configTest.GetValue(str);
    EXPECT_TRUE(jsonValue.asString() == "soc");
    BATTERY_HILOGI(LABEL_TEST, "BatteryConfig0025 function end!");
}
} // namespace PowerMgr
} // namespace OHOS

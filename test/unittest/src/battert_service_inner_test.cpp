/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "battery_callback_test.h"
#include "battery_config_test.h"
#include "battery_dump_test.h"

#include <string>
#include <memory>

#include "battery_callback.h"
#include "battery_config.h"
#include "battery_log.h"

#ifdef GTEST
#define private   public
#define protected public
#endif
#include "battery_dump.h"
#include "battery_service.h"
#include "power_common.h"

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
sptr<BatteryService> g_service;
auto& g_configTest = BatteryConfig::GetInstance();
}

int32_t HandleBatteryCallbackEvent(const OHOS::HDI::Battery::V2_0::BatteryInfo& event)
{
    return ERR_OK;
}

void BatteryCallbackTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
}

void BatteryCallbackTest::TearDownTestCase()
{
    g_service = nullptr;
}

void BatteryDumpTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
    g_service->isBootCompleted_ = true;
}

/**
 * @tc.name: BatteryCallback001
 * @tc.desc: Update BatteryInfo, the eventCb_ is valid
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryCallbackTest, BatteryCallback001, TestSize.Level1)
{
    sptr<HDI::Battery::V2_0::IBatteryInterface> iBatteryInterface;
    iBatteryInterface = HDI::Battery::V2_0::IBatteryInterface::Get();
    sptr<HDI::Battery::V2_0::IBatteryCallback> callback = new BatteryCallback();
    EXPECT_EQ(iBatteryInterface->Register(callback), HDF_SUCCESS);

    BatteryCallback::BatteryEventCallback eventCb = std::bind(&HandleBatteryCallbackEvent, std::placeholders::_1);
    EXPECT_EQ(BatteryCallback::RegisterBatteryEvent(eventCb), HDF_SUCCESS);
    HDI::Battery::V2_0::BatteryInfo event;
    iBatteryInterface->GetBatteryInfo(event);
    EXPECT_NE(callback->Update(event), HDF_FAILURE);
}

/**
 * @tc.name: BatteryCallback002
 * @tc.desc: Update BatteryInfo, the eventCb_ is invalid
 * @tc.type: FUNC
 */
HWTEST_F(BatteryCallbackTest, BatteryCallback002, TestSize.Level1)
{
    BatteryCallback::BatteryEventCallback eventCb = nullptr;
    EXPECT_EQ(BatteryCallback::RegisterBatteryEvent(eventCb), HDF_SUCCESS);
    HDI::Battery::V2_0::BatteryInfo event;
    sptr<HDI::Battery::V2_0::IBatteryCallback> callback = new BatteryCallback();
    EXPECT_EQ(callback->Update(event), HDF_FAILURE);
}

/**
 * @tc.name: BatteryConfig001
 * @tc.desc: Parse config, and configPath parameter is real path
 * @tc.type: FUNC
 */
HWTEST_F(BatteryConfigTest, BatteryConfig001, TestSize.Level1)
{
    BATTERY_HILOGD(LABEL_TEST, "BatteryConfig001 begin");
    EXPECT_TRUE(g_configTest.ParseConfig());
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
    EXPECT_TRUE(g_configTest.ParseConfig());
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

/**
 * @tc.name: BatteryDump001
 * @tc.desc: Dump parameter is -i, Get battery information
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump001, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-i";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/**
 * @tc.name: BatteryDump002
 * @tc.desc: Dump parameter is -u, MockUnplugged
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump002, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-u";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/**
 * @tc.name: BatteryDump003
 * @tc.desc: Dump parameter is -r, Reset
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump003, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-r";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/*
 * @tc.name: BatteryDump005
 * @tc.desc: Test functions Dump
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryDumpTest, BatteryDump005, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-l";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_NO_INIT);
}

/**
 * @tc.name: BatteryDump006
 * @tc.desc: Test functions Dump
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryDumpTest, BatteryDump006, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-ls";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_NO_INIT);
}

/**
 * @tc.name: BatteryDump007
 * @tc.desc: Test functions Dump
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryDumpTest, BatteryDump007, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/**
 * @tc.name: BatteryDump008
 * @tc.desc: Dump parameter is empty, Get battery information
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump008, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.GetBatteryInfo(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump009
 * @tc.desc: Dump parameter is empty, MockUnplugged
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump009, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.MockUnplugged(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump010
 * @tc.desc: Dump parameter is empty, Reset
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump010, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.Reset(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump011
 * @tc.desc: Test functions Dump, capacity cmd normal
 * @tc.type: FUNC
 * @tc.require: issueI6Z8RB
 */
static HWTEST_F(BatteryDumpTest, BatteryDump011, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string argParam = u"--capacity";
    std::u16string argCapacity = u"20";
    args.push_back(argParam);
    args.push_back(argCapacity);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/**
 * @tc.name: BatteryDump012
 * @tc.desc: Test functions Dump, capacity cmd invalid
 * @tc.type: FUNC
 * @tc.require: issueI6Z8RB
 */
static HWTEST_F(BatteryDumpTest, BatteryDump012, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"--capacity";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_NO_INIT);
}

/**
 * @tc.name: BatteryDump013
 * @tc.desc: Test functions Dump, capacity out of range
 * @tc.type: FUNC
 * @tc.require: issueI6Z8RB
 */
static HWTEST_F(BatteryDumpTest, BatteryDump013, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"--capacity 200";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_NO_INIT);
}

/**
 * @tc.name: BatteryDump014
 * @tc.desc: Dump parameter is empty, Capacity
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump014, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.MockCapacity(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump015
 * @tc.desc: Dump parameter is empty, Uevent
 * @tc.type: FUNC
 */
HWTEST_F(BatteryDumpTest, BatteryDump015, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.MockUevent(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump016
 * @tc.desc: Test functions Dump, Uevent cmd normal
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryDumpTest, BatteryDump016, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string argParam = u"--uevent";
    std::u16string argCapacity = u"BATTERY_UNDER_VOLTAGE=3$sendcommonevent";
    args.push_back(argParam);
    args.push_back(argCapacity);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_OK);
}

/**
 * @tc.name: BatteryDump017
 * @tc.desc: Test functions Dump, Uevent cmd invalid
 * @tc.type: FUNC
 */
static HWTEST_F(BatteryDumpTest, BatteryDump017, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"--uevent";
    args.push_back(arg);
    EXPECT_EQ(g_service->Dump(fd, args), ERR_NO_INIT);
}
} // namespace PowerMgr
} // namespace OHOS
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

#include "battery_notify_test.h"

#include <string>
#ifdef GTEST
#define private   public
#define protected public
#endif

#include "battery_log.h"
#include "battery_notify.h"
#include "battery_service.h"
#include "battery_config.h"
#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
#include "charging_sound.h"
#include "ffrt_utils.h"
#endif
#include "notification_locale.h"
using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
bool g_retval = false;
bool g_called = false;
#endif
} // namespace
BatteryInfo* g_batteryInfo;
std::shared_ptr<BatteryNotify> g_batteryNotify;
sptr<BatteryService> g_service = DelayedSpSingleton<BatteryService>::GetInstance();

void BatteryNotifyTest::SetUpTestCase()
{
    g_batteryNotify = std::make_shared<BatteryNotify>();
}

void BatteryNotifyTest::SetUp()
{
    g_batteryInfo = new BatteryInfo();
    const int32_t capacity = 100;
    g_batteryInfo->SetCapacity(capacity);
    const int32_t voltage = 1;
    g_batteryInfo->SetVoltage(voltage);
    const int32_t temperature = 25;
    g_batteryInfo->SetTemperature(temperature);
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_UNKNOWN;
    g_batteryInfo->SetHealthState(healthState);
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
    g_batteryInfo->SetPluggedType(pluggedType);
    const int32_t plugMaxCur = 50;
    g_batteryInfo->SetPluggedMaxCurrent(plugMaxCur);
    const int32_t plugMaxVol = 50;
    g_batteryInfo->SetPluggedMaxVoltage(plugMaxVol);
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_NONE;
    g_batteryInfo->SetChargeState(chargeState);
    const int32_t chargeCounter = 1;
    g_batteryInfo->SetChargeCounter(chargeCounter);
    const int32_t totalEnergy = 50;
    g_batteryInfo->SetTotalEnergy(totalEnergy);
    const int32_t curAverage = 10;
    g_batteryInfo->SetCurAverage(curAverage);
    const int32_t remainEnergy = 30;
    g_batteryInfo->SetRemainEnergy(remainEnergy);
    g_batteryInfo->SetPresent(true);
    const string tec = "H2";
    g_batteryInfo->SetTechnology(tec);
    const int32_t nowCur = 10;
    g_batteryInfo->SetNowCurrent(nowCur);
    const string uevent = "BatteryNotifyTest";
    g_batteryInfo->SetUevent(uevent);
}

void BatteryNotifyTest::TearDown()
{
    if (g_batteryInfo != nullptr) {
        delete g_batteryInfo;
        g_batteryInfo = nullptr;
    }
}

void BatteryNotifyTest::DestroyJsonValue(cJSON*& value)
{
    if (value) {
        cJSON_Delete(value);
        value = nullptr;
    }
}

#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
// the static member function may be inlined in the C API func be the compiler
// thus redefine the non-static one which can't be inlined out
bool ChargingSound::Play()
{
    BATTERY_HILOGI(LABEL_TEST, "mock function called");
    g_called = true;
    return g_retval;
}
#endif

/**
 * @tc.name: BatteryNotify001
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify001, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify001 function start!");
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify001 function end!");
}

/**
 * @tc.name: BatteryNotify002
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify002, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify002 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_GOOD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify002 function end!");
}

/**
 * @tc.name: BatteryNotify003
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify003, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify003 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_OVERHEAT;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify003 function end!");
}

/**
 * @tc.name: BatteryNotify004
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify004, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify004 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_OVERVOLTAGE;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify004 function end!");
}

/**
 * @tc.name: BatteryNotify005
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify005, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify005 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_COLD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify005 function end!");
}

/**
 * @tc.name: BatteryNotify006
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify006, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify006 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_DEAD;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify006 function end!");
}

/**
 * @tc.name: BatteryNotify007
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify007, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify007 function start!");
    const BatteryHealthState healthState = BatteryHealthState::HEALTH_STATE_BUTT;
    g_batteryInfo->SetHealthState(healthState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify007 function end!");
}

/**
 * @tc.name: BatteryNotify008
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify008, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify008 function start!");
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify008 function end!");
}

/**
 * @tc.name: BatteryNotify009
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify009, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify009 function start!");
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_AC;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify009 function end!");
}

/**
 * @tc.name: BatteryNotify010
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify010, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify010 function start!");
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_USB;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify010 function end!");
}

/**
 * @tc.name: BatteryNotify011
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify011, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify011 function start!");
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_WIRELESS;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify011 function end!");
}

/**
 * @tc.name: BatteryNotify012
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify012, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify012 function start!");
    const BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_BUTT;
    g_batteryInfo->SetPluggedType(pluggedType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify012 function end!");
}

/**
 * @tc.name: BatteryNotify013
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify013, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify013 function start!");
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_ENABLE;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify013 function end!");
}

/**
 * @tc.name: BatteryNotify014
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify014, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify014 function start!");
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_DISABLE;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify014 function end!");
}

/**
 * @tc.name: BatteryNotify015
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify015, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify015 function start!");
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_FULL;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify015 function end!");
}

/**
 * @tc.name: BatteryNotify016
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify016, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify016 function start!");
    const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_BUTT;
    g_batteryInfo->SetChargeState(chargeState);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify016 function end!");
}

/**
 * @tc.name: BatteryNotify017
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify017, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify017 function start!");
    const int32_t capacity = -100;
    g_batteryInfo->SetCapacity(capacity);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify017 function end!");
}

/**
 * @tc.name: BatteryNotify018
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify018, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify018 function start!");
    for (int i = 0; i < 2; i++) {
        const BatteryChargeState chargeState = BatteryChargeState::CHARGE_STATE_ENABLE;
        g_batteryInfo->SetChargeState(chargeState);
        auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
        EXPECT_EQ(ret, ERR_OK);
        BATTERY_HILOGI(LABEL_TEST, "BatteryNotify018 function end!");
    }
}

/**
 * @tc.name: BatteryNotify019
 * @tc.desc: Test PublishEvents
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify019, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify019 function start!");
    const int32_t capacity = -100;
    g_batteryInfo->SetCapacity(capacity);
    for (int i = 0; i < 2; i++) {
        auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
        EXPECT_EQ(ret, ERR_OK);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify019 function end!");
}

/**
 * @tc.name: BatteryNotify020
 * @tc.desc: Test PublishChargeTypeEvent
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify020, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify020 function start!");
    const ChargeType chargeType = ChargeType::WIRED_NORMAL;
    g_batteryInfo->SetChargeType(chargeType);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify020 function end!");
}

/**
 * @tc.name: BatteryNotify021
 * @tc.desc: Test PublishUEvent
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify021, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify021 function start!");
    const string uevent1 = "TEST_BATTERY_UNDER_VOLTAGE=3";
    g_batteryInfo->SetUevent(uevent1);
    EXPECT_EQ(g_batteryInfo->GetUevent(), uevent1);

    const string uevent2 = "BATTERY_UNDER_VOLTAGE=3";
    g_batteryInfo->SetUevent(uevent2);
    auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);

    const string uevent3 = "XXXBATTERY_UNDER_VOLTAGE=1";
    g_batteryInfo->SetUevent(uevent3);
    ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
    EXPECT_EQ(ret, ERR_OK);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify021 function end!");
}

/**
 * @tc.name: BatteryNotify023
 * @tc.desc: Test PublishChangedEvent--BatteryInfo::COMMON_EVENT_KEY_CAPACITY_LEVEL
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify023, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify023 function start!");
    g_service->isMockCapacity_ = true;
    g_service->batteryInfo_.SetCapacity(100);
    g_service->InitConfig();
    bool ret = g_batteryNotify->PublishChangedEvent(*g_batteryInfo);
    EXPECT_TRUE(ret);
    g_service->batteryInfo_.SetCapacity(50);
    ret = g_batteryNotify->PublishChangedEvent(*g_batteryInfo);
    EXPECT_TRUE(ret);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify023 function end!");
}

/**
 * @tc.name: BatteryNotify024
 * @tc.desc: Test HandleNotification
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify024, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify024 function start!");
    std::string jsonStr = R"({"popup": {"XXX": [{"name": 123}]}})";
    cJSON* parseResult = cJSON_Parse(jsonStr.c_str());
    EXPECT_TRUE(parseResult);
    auto& batteryConfig = BatteryConfig::GetInstance();
    batteryConfig.config_ = parseResult;
    batteryConfig.ParsePopupConf();
    EXPECT_TRUE(batteryConfig.popupConfig_.size() == 1);
    bool ret = g_batteryNotify->HandleNotification("XXX");
    BATTERY_HILOGI(LABEL_TEST, "HandleNotification ret[%d]", static_cast<int32_t>(ret));
#ifndef BATTERY_SUPPORT_NOTIFICATION
    EXPECT_TRUE(ret);
#endif
    DestroyJsonValue(batteryConfig.config_);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify024 function end!");
}

/**
 * @tc.name: BatteryNotify025
 * @tc.desc: Test SaveJsonToMap
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify025, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify025 function start!");
    auto& localeConfig = NotificationLocale::GetInstance();
    std::unordered_map<std::string, std::string> stringMap;
    std::string jsonStr = "mock data";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"(null)";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"([{"wirelesscharger": null}])";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"({"wirelesscharger": null})";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"({"string": null})";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"({"string": 1})";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"({"string": [{"name": ""},{"value": ""},{"name": 1,"value": ""}]})";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);

    jsonStr = R"({"string": [{"name": "","value": 1},{"name": "","value": ""}]})";
    localeConfig.SaveJsonToMap(jsonStr, "", stringMap);
    EXPECT_TRUE(stringMap.size() == 0);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify025 function end!");
}

/**
 * @tc.name: BatteryNotify026
 * @tc.desc: Test Charger Sound
 * @tc.type: FUNC
 */
#ifdef BATTERY_MANAGER_ENABLE_CHARGING_SOUND
HWTEST_F(BatteryNotifyTest, BatteryNotify026, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify026 function start!");
    DelayedSpSingleton<BatteryService>::GetInstance()->Init();
    g_batteryInfo->SetUevent("");
    for (bool retval : {true, false}) {
        g_retval = retval;
        BatteryPluggedType pluggedType = BatteryPluggedType::PLUGGED_TYPE_NONE;
        g_batteryInfo->SetPluggedType(pluggedType);
        auto ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
        EXPECT_EQ(ret, ERR_OK);
        pluggedType = BatteryPluggedType::PLUGGED_TYPE_AC;
        g_batteryInfo->SetPluggedType(pluggedType);
        ret = g_batteryNotify->PublishEvents(*g_batteryInfo);
        EXPECT_EQ(ret, ERR_OK);
        ffrt::wait();
    }
    EXPECT_TRUE(g_called);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify026 function end!");
    ffrt::wait();
}
#endif

/**
 * @tc.name: BatteryNotify027
 * @tc.desc: Test GetPowerDisplayString
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify027, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify027 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    std::string fmtStr = "power: %s.";
    std::map<std::string, std::string> params = {
        {"77", "power: 77W."},
        {"1234", "power: 1234W."},
        {"", "power: %s."}
    };
    for (auto&[param, expected]: params) {
        auto ret = notificationLocale.GetPowerDisplayString(fmtStr, param);
        EXPECT_EQ(ret, expected);
    }
    std::string bigFmtStr(1025, 's');
    auto ret = notificationLocale.GetPowerDisplayString(bigFmtStr, "123");
    EXPECT_EQ(ret, bigFmtStr);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify027 function end!");
}

/**
 * @tc.name: BatteryNotify028
 * @tc.desc: Test FillTextWithPower
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify028, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify028 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    std::string text = "%s";
    auto ret = notificationLocale.FillTextWithPower(text);
    EXPECT_EQ(ret, text);
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify028 function end!");
}

/**
 * @tc.name: BatteryNotify029
 * @tc.desc: Test GetStringByKey
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify029, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify029 function start!");
    constexpr const char* REVERSE_CHARGE_WITH_POWER_DISPLAY_TEXT_KEY =
        "reverse_super_charge_with_power_display_start_text";
    auto& notificationLocale = NotificationLocale::GetInstance();
    auto tmpMap = notificationLocale.stringMap_;
    notificationLocale.stringMap_.insert(std::make_pair(REVERSE_CHARGE_WITH_POWER_DISPLAY_TEXT_KEY, "power: %s"));
    notificationLocale.stringMap_.insert(std::make_pair("key", "text"));

    auto ret = notificationLocale.GetStringByKey("key");
    EXPECT_EQ(ret, "text");
    ret = notificationLocale.GetStringByKey(REVERSE_CHARGE_WITH_POWER_DISPLAY_TEXT_KEY);
    EXPECT_EQ(ret, "power: %s");
    ret = notificationLocale.GetStringByKey("invalid key");
    EXPECT_EQ(ret, "");

    notificationLocale.stringMap_ = tmpMap;
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify029 function end!");
}

/**
 * @tc.name: BatteryNotify030
 * @tc.desc: Test GetPowerDisplayString with abnormal inputs
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify030, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify030 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string fmtStr = "";
    std::string power = "100";
    auto ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, "");

    fmtStr = "No placeholder";
    ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, "No placeholder");

    fmtStr = "Power: %s";
    power = "";
    ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, fmtStr);

    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify030 function end!");
}

/**
 * @tc.name: BatteryNotify031
 * @tc.desc: Test FillTextWithPower with empty or valid power
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify031, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify031 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string text = "Charging %s";
    auto ret = notificationLocale.FillTextWithPower(text);
    EXPECT_EQ(ret, text);

    text = "";
    ret = notificationLocale.FillTextWithPower(text);
    EXPECT_EQ(ret, "");
    
    text = "No placeholder text";
    ret = notificationLocale.FillTextWithPower(text);
    EXPECT_EQ(ret, text);

    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify031 function end!");
}

/**
 * @tc.name: BatteryNotify032
 * @tc.desc: Test GetPowerDisplayString with complex formats
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify032, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify032 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string fmtStr = "Power: %s%%";
    std::string power = "90";
    auto ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, "Power: 90W%");
    
    fmtStr = "Current power: %s";
    power = "50";
    ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, "Current power: 50W");
    
    fmtStr = "Status: %s";
    power = "High";
    ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, "Status: HighW");
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify032 function end!");
}

/**
 * @tc.name: BatteryNotify033
 * @tc.desc: Test GetPowerDisplayString boundary conditions
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify033, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify033 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string longPrefix(900, 'A');
    std::string fmtStr = longPrefix + "%s";
    std::string power = "10";
    auto ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, longPrefix + "10W");
    
    std::string hugePrefix(1050, 'B');
    fmtStr = hugePrefix + "%s";
    ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
    EXPECT_EQ(ret, fmtStr);
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify033 function end!");
}

/**
 * @tc.name: BatteryNotify034
 * @tc.desc: Test FillTextWithPower simulation via map manipulation
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify034, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify034 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
   
    std::string text = "Turbo Charge %s";
    auto ret = notificationLocale.FillTextWithPower(text);
    EXPECT_EQ(ret, text);
    
    ret = notificationLocale.FillTextWithPower("");
    EXPECT_EQ(ret, "");

    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify034 function end!");
}

/**
 * @tc.name: BatteryNotify035
 * @tc.desc: Test GetPowerDisplayString with multiple calls and state stability
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify035, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify035 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    std::string fmtStr = "Power: %s";
    
    for (int i = 0; i < 100; ++i) {
        std::string power = std::to_string(i);
        std::string expected = "Power: " + power + "W";
        auto ret = notificationLocale.GetPowerDisplayString(fmtStr, power);
        EXPECT_EQ(ret, expected);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify035 function end!");
}

/**
 * @tc.name: BatteryNotify036
 * @tc.desc: Test GetPowerDisplayString with various special characters in power string
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify036, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify036 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    std::string fmtStr = "Value: %s";
    
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"10.5", "Value: 10.5W"},
        {"-5", "Value: -5W"},
        {"0", "Value: 0W"},
        {" ", "Value:  W"},
        {"!", "Value: !W"},
        {"@#$", "Value: @#$W"},
        {"\n", "Value: \nW"},
        {"\t", "Value: \tW"}
    };
    
    for (const auto& testCase : testCases) {
        auto ret = notificationLocale.GetPowerDisplayString(fmtStr, testCase.first);
        EXPECT_EQ(ret, testCase.second);
    }
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify036 function end!");
}

/**
 * @tc.name: BatteryNotify037
 * @tc.desc: Test GetPowerDisplayString with different placeholder positions and repeated placeholders
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify037, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify037 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("%s is power", "50"), "50W is power");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("Power is %s", "50"), "Power is 50W");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("P%sr", "50"), "P50Wr");
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify037 function end!");
}

/**
 * @tc.name: BatteryNotify038
 * @tc.desc: Test FillTextWithPower with long input text
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify038, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify038 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string longText(500, 'A');
    longText += " %s ";
    longText += std::string(400, 'B');
    
    auto ret = notificationLocale.FillTextWithPower(longText);
    EXPECT_EQ(ret, longText);
    EXPECT_EQ(ret.length(), longText.length());
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify038 function end!");
}

/**
 * @tc.name: BatteryNotify039
 * @tc.desc: Test GetPowerDisplayString with null-like characters in format
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify039, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify039 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string fmtStr = "Prefix";
    fmtStr.push_back('\0');
    fmtStr += "Suffix %s";
    
    auto ret = notificationLocale.GetPowerDisplayString(fmtStr, "10");
    EXPECT_EQ(ret, "Prefix");
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify039 function end!");
}

/**
 * @tc.name: BatteryNotify040
 * @tc.desc: Test GetPowerDisplayString with very short format strings
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify040, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify040 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("%s", "1"), "1W");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString(" %s", "1"), " 1W");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("%s ", "1"), "1W ");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("A", "1"), "A");
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify040 function end!");
}

/**
 * @tc.name: BatteryNotify041
 * @tc.desc: Test FillTextWithPower with special characters in input text
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify041, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify041 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string specialText = "!@#$%^&*()_+{}|:\"<>?~`-=[]\\;',./";
    auto ret = notificationLocale.FillTextWithPower(specialText);
    EXPECT_EQ(ret, specialText);
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify041 function end!");
}

/**
 * @tc.name: BatteryNotify042
 * @tc.desc: Test NotificationLocale singleton consistency in multi-threaded context (simulated)
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify042, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify042 function start!");
    auto& instance1 = NotificationLocale::GetInstance();
    auto& instance2 = NotificationLocale::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify042 function end!");
}

/**
 * @tc.name: BatteryNotify043
 * @tc.desc: Test GetPowerDisplayString with numerical formatting (should be treated as string)
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify043, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify043 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("Power: %s", "0.001"), "Power: 0.001W");
    EXPECT_EQ(notificationLocale.GetPowerDisplayString("Power: %s", "1e6"), "Power: 1e6W");
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify043 function end!");
}

/**
 * @tc.name: BatteryNotify044
 * @tc.desc: Test FillTextWithPower with very long strings and no placeholders
 * @tc.type: FUNC
 */
HWTEST_F(BatteryNotifyTest, BatteryNotify044, TestSize.Level1)
{
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify044 function start!");
    auto& notificationLocale = NotificationLocale::GetInstance();
    
    std::string longText(1000, 'Z');
    auto ret = notificationLocale.FillTextWithPower(longText);
    EXPECT_EQ(ret, longText);
    
    BATTERY_HILOGI(LABEL_TEST, "BatteryNotify044 function end!");
}
} // namespace PowerMgr
} // namespace OHOS

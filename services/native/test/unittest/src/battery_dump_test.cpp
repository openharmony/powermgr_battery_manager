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

#include "battery_dump_test.h"
#include "battery_dump.h"
#include "power_common.h"
#include <memory>

using namespace testing::ext;

namespace OHOS {
namespace PowerMgr {
namespace {
sptr<BatteryService> g_service;
}
void BatteryDumpTest::SetUpTestCase()
{
    g_service = DelayedSpSingleton<BatteryService>::GetInstance();
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
 * @tc.desc: Dump parameter is -r, ResetPlugged
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

/**
 * @tc.name: BatteryDump004
 * @tc.desc: Test functions Dump
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
static HWTEST_F(BatteryDumpTest, BatteryDump004, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-d";
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
    g_service->Dump(fd, args);
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
 * @tc.desc: Dump parameter is -h, DumpBatteryHelp
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump009, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    std::u16string arg = u"-h";
    args.push_back(arg);
    EXPECT_TRUE(batteryDump.DumpBatteryHelp(fd, args));
}

/**
 * @tc.name: BatteryDump010
 * @tc.desc: Dump parameter is empty, GDumpBatteryHelp
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump010, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.DumpBatteryHelp(fd, args));
}

/**
 * @tc.name: BatteryDump011
 * @tc.desc: Dump parameter is empty, MockUnplugged
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump011, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.MockUnplugged(fd, g_service, args));
}

/**
 * @tc.name: BatteryDump012
 * @tc.desc: Dump parameter is empty, ResetPlugged
 * @tc.type: FUNC
 * @tc.require: issueI5YZR1
 */
HWTEST_F(BatteryDumpTest, BatteryDump013, TestSize.Level1)
{
    BatteryDump& batteryDump = BatteryDump::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> args;
    EXPECT_FALSE(batteryDump.ResetPlugged(fd, g_service, args));
}
} // namespace PowerMgr
} // namespace OHOS

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "battery_info.h"
#include "battery_srv_client.h"

using namespace std;

namespace OHOS {
namespace PowerMgr {
class BatteryBenchmarkTest : public benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& state) {}
    void TearDown(const ::benchmark::State& state) {}
};

namespace {
auto& g_batterySrvClient = BatterySrvClient::GetInstance();
const int32_t ITERATION_FREQUENCY = 100;
const int32_t REPETITION_FREQUENCY = 3;

/**
 * @tc.name: GetCapacity
 * @tc.desc: Testcase for testing "GetCapacity" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetCapacity)(benchmark::State& st)
{
    for (auto _ : st) {
        int32_t capactiy = g_batterySrvClient.GetCapacity();
        // capacity is range of 0 - 100
        ASSERT_TRUE(capactiy >= 0 && capactiy <= 100);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetCapacity)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetChargingStatus
 * @tc.desc: Testcase for testing "GetChargingStatus" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetChargingStatus)(benchmark::State& st)
{
    for (auto _ : st) {
        auto chargeState = g_batterySrvClient.GetChargingStatus();
        ASSERT_TRUE(chargeState >= BatteryChargeState::CHARGE_STATE_NONE &&
            chargeState < BatteryChargeState::CHARGE_STATE_BUTT);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetChargingStatus)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetHealthStatus
 * @tc.desc: Testcase for testing "GetHealthStatus" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetHealthStatus)(benchmark::State& st)
{
    for (auto _ : st) {
        auto healthStatus = g_batterySrvClient.GetHealthStatus();
        ASSERT_TRUE(healthStatus >= BatteryHealthState::HEALTH_STATE_UNKNOWN &&
            healthStatus < BatteryHealthState::HEALTH_STATE_BUTT);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetHealthStatus)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetPluggedType
 * @tc.desc: Testcase for testing "GetPluggedType" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetPluggedType)(benchmark::State& st)
{
    for (auto _ : st) {
        auto pluggedType = g_batterySrvClient.GetPluggedType();
        ASSERT_TRUE(pluggedType >= BatteryPluggedType::PLUGGED_TYPE_NONE &&
            pluggedType < BatteryPluggedType::PLUGGED_TYPE_BUTT);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetPluggedType)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetVoltage
 * @tc.desc: Testcase for testing "GetVoltage" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetVoltage)(benchmark::State& st)
{
    for (auto _ : st) {
        // voltage above 0μV
        ASSERT_TRUE(g_batterySrvClient.GetVoltage() >= 0);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetVoltage)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetPresent
 * @tc.desc: Testcase for testing "GetPresent" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetPresent)(benchmark::State& st)
{
    for (auto _ : st) {
        bool present = g_batterySrvClient.GetPresent();
        ASSERT_TRUE(present || !present);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetPresent)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetTechnology
 * @tc.desc: Testcase for testing "GetTechnology" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetTechnology)(benchmark::State& st)
{
    for (auto _ : st) {
        auto technology = g_batterySrvClient.GetTechnology();
        ASSERT_TRUE(!technology.empty());
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetTechnology)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetBatteryTemperature
 * @tc.desc: Testcase for testing "GetBatteryTemperature" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetBatteryTemperature)(benchmark::State& st)
{
    for (auto _ : st) {
        auto temperature = g_batterySrvClient.GetBatteryTemperature();
        // Temperature range -20.0­°C to 60.0­°C
        ASSERT_TRUE(temperature >= -200 && temperature <= 600);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetBatteryTemperature)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetNowCurrent
 * @tc.desc: Testcase for testing "GetNowCurrent" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetNowCurrent)(benchmark::State& st)
{
    for (auto _ : st) {
        auto currnow = g_batterySrvClient.GetNowCurrent();
        // Now current range -20000mA - 20000mA
        ASSERT_TRUE(currnow >= -20000 && currnow <= 20000);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetNowCurrent)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetRemainEnergy
 * @tc.desc: Testcase for testing "GetRemainEnergy" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetRemainEnergy)(benchmark::State& st)
{
    for (auto _ : st) {
        auto chargenow = g_batterySrvClient.GetRemainEnergy();
        // RemainEnergy above 0mA
        ASSERT_TRUE(chargenow >= 0);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetRemainEnergy)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetTotalEnergy
 * @tc.desc: Testcase for testing "GetTotalEnergy" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetTotalEnergy)(benchmark::State& st)
{
    for (auto _ : st) {
        auto totalenergy = g_batterySrvClient.GetTotalEnergy();
        // Totalenergy above 0mA
        ASSERT_TRUE(totalenergy >= 0);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetTotalEnergy)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetCapacityLevel
 * @tc.desc: Testcase for testing "GetCapacityLevel" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetCapacityLevel)(benchmark::State& st)
{
    for (auto _ : st) {
        auto batterylevel = g_batterySrvClient.GetCapacityLevel();
        ASSERT_TRUE(batterylevel >= BatteryCapacityLevel::LEVEL_NONE &&
            batterylevel < BatteryCapacityLevel::LEVEL_RESERVED);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetCapacityLevel)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();

/**
 * @tc.name: GetRemainingChargeTime
 * @tc.desc: Testcase for testing "GetRemainingChargeTime" function.
 * @tc.type: FUNC
 */
BENCHMARK_F(BatteryBenchmarkTest, GetRemainingChargeTime)(benchmark::State& st)
{
    for (auto _ : st) {
        // RemainingChargeTime above 0ms
        ASSERT_TRUE(g_batterySrvClient.GetRemainingChargeTime() >= 0);
    }
}
BENCHMARK_REGISTER_F(BatteryBenchmarkTest, GetRemainingChargeTime)
    ->Iterations(ITERATION_FREQUENCY)
    ->Repetitions(REPETITION_FREQUENCY)
    ->ReportAggregatesOnly();
} // namespace
} // namespace PowerMgr
} // namespace OHOS

// Run the benchmark
BENCHMARK_MAIN();

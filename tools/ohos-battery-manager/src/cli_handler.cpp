/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "cli_handler.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_srv_errors.h"

namespace {
#define BATTERY_CLI_LOGI(fmt, ...) fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define BATTERY_CLI_LOGE(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

using CommandHandler = std::function<int(int, char**)>;
struct Command {
    const char* name;
    const char* description;
    CommandHandler handler;
};

static std::unordered_map<std::string, Command> g_commands;
static constexpr uint32_t CLI_CMD_MIN_PARAM_COUNT = 2;
}

namespace json_builder {
std::string EscapeString(const std::string& s)
{
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"':
                oss << "\\\"";
                break;
            case '\\':
                oss << "\\\\";
                break;
            case '\n':
                oss << "\\n";
                break;
            case '\r':
                oss << "\\r";
                break;
            case '\t':
                oss << "\\t";
                break;
            default:
                oss << c;
                break;
        }
    }
    return oss.str();
}

void PrintSuccess(const std::string& dataJson)
{
    std::cout << "{\"success\":true,\"data\":" << dataJson << "}" << std::endl;
}

void PrintError(const std::string& code, const std::string& message, const std::string& suggestion = "")
{
    std::cout << "{\"success\":false,\"error\":{\"code\":\"" << EscapeString(code) << "\",\"message\":\"" <<
        EscapeString(message) << "\"}";
    if (!suggestion.empty()) {
        std::cout << ",\"suggestion\":\"" << EscapeString(suggestion) << "\"";
    }
    std::cout << "}" << std::endl;
}

} // namespace json_builder

static void RegisterCommand(const char* name, const char* desc, CommandHandler handler)
{
    g_commands[name] = {name, desc, std::move(handler)};
}

static int CmdHelp([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    BATTERY_CLI_LOGI("Usage: ohos-battery-manager <command> [options]");
    BATTERY_CLI_LOGI("");
    BATTERY_CLI_LOGI("Available commands:");
    for (const auto& pair : g_commands) {
        BATTERY_CLI_LOGI("  %-20s %s", pair.first.c_str(), pair.second.description);
    }
    BATTERY_CLI_LOGI("");
    BATTERY_CLI_LOGI("Run 'ohos-battery-manager help <command>' for details on a specific command.");
    return 0;
}

static int CmdCapacity([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        BATTERY_CLI_LOGE("Unexpected argument for 'capacity' command");
        BATTERY_CLI_LOGI("Usage: ohos-battery-manager capacity");
        return 1;
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t capacity = client.GetCapacity();
    if (capacity == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        BATTERY_CLI_LOGE("Failed to get battery capacity, service may be unavailable");
        json_builder::PrintError("ERR_CONNECTION_FAIL",
            "Failed to get battery capacity.",
            "Check if powermgr process is running.");
        return 1;
    }

    std::ostringstream oss;
    oss << "{\"capacity\":" << capacity << "}";
    json_builder::PrintSuccess(oss.str());
    return 0;
}

static int CmdTotalEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        BATTERY_CLI_LOGE("Unexpected argument for 'total-energy' command");
        BATTERY_CLI_LOGI("Usage: ohos-battery-manager total-energy");
        return 1;
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t totalEnergy = client.GetTotalEnergy();
    if (totalEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        BATTERY_CLI_LOGE("Failed to get total battery energy, service may be unavailable or permission denied");
        json_builder::PrintError("ERR_CONNECTION_FAIL",
            "Failed to get total battery energy.",
            "Check if powermgr process is running and caller has system permission.");
        return 1;
    }

    std::ostringstream oss;
    oss << "{\"totalEnergy\":" << totalEnergy << "}";
    json_builder::PrintSuccess(oss.str());
    return 0;
}

static int CmdRemainEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        BATTERY_CLI_LOGE("Unexpected argument for 'remain-energy' command");
        BATTERY_CLI_LOGI("Usage: ohos-battery-manager remain-energy");
        return 1;
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t remainEnergy = client.GetRemainEnergy();
    if (remainEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        BATTERY_CLI_LOGE("Failed to get remaining battery energy, service may be unavailable or permission denied");
        json_builder::PrintError("ERR_CONNECTION_FAIL",
            "Failed to get remaining battery energy.",
            "Check if powermgr process is running and caller has system permission.");
        return 1;
    }

    std::ostringstream oss;
    oss << "{\"remainEnergy\":" << remainEnergy << "}";
    json_builder::PrintSuccess(oss.str());
    return 0;
}

static void InitCommands()
{
    RegisterCommand("help", "Show help message", CmdHelp);
    RegisterCommand("capacity", "Query battery capacity percentage (0-100%)", CmdCapacity);
    RegisterCommand("total-energy", "Query battery total energy (mAh)", CmdTotalEnergy);
    RegisterCommand("remain-energy", "Query battery remaining energy (mAh)", CmdRemainEnergy);
}

static void PrintUsage(const char* prog)
{
    BATTERY_CLI_LOGI("Usage: %s <command> [options]", prog);
    BATTERY_CLI_LOGI("Run '%s help' for more information.", prog);
}

int HandleCommand(int argc, char** argv)
{
    if (argc < CLI_CMD_MIN_PARAM_COUNT) {
        PrintUsage(argv[0]);
        return 1;
    }

    InitCommands();
    std::string cmdName = argv[1];
    auto it = g_commands.find(cmdName);
    if (it == g_commands.end()) {
        BATTERY_CLI_LOGE("Unknown command: %s", cmdName.c_str());
        PrintUsage(argv[0]);
        return 1;
    }

    int cmdArgc = argc - CLI_CMD_MIN_PARAM_COUNT;
    char** cmdArgv = argv + CLI_CMD_MIN_PARAM_COUNT;
    return it->second.handler(cmdArgc, cmdArgv);
}

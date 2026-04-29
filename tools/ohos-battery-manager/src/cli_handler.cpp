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

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_srv_errors.h"
#include "nlohmann/json.hpp"

namespace {
#define BATTERY_CLI_LOGI(fmt, ...) fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define BATTERY_CLI_LOGE(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

using CommandHandler = std::function<int(int, char**)>;
struct Command {
    const char* name;
    const char* description;
    CommandHandler handler;
    const char* usage;  // Usage for subcommand help
};

static std::unordered_map<std::string, Command> g_commands;
static constexpr uint32_t CLI_CMD_MIN_PARAM_COUNT = 2;
} // namespace

static void PrintSuccess(const nlohmann::json& data)
{
    nlohmann::json result;
    result["status"] = "success";
    result["data"] = data;
    std::cout << result.dump() << std::endl;
}

static void PrintError(const std::string& errorCode, const std::string& errorMessage)
{
    nlohmann::json result;
    result["status"] = "error";
    result["error_code"] = errorCode;
    result["error_msg"] = errorMessage;
    std::cout << result.dump() << std::endl;
}

static void RegisterCommand(const char* name, const char* desc, CommandHandler handler, const char* usage = "")
{
    g_commands[name] = {name, desc, std::move(handler), usage};
}

static int CmdHelp([[maybe_unused]] int argc, char** argv)
{
    // Support both 'help' and 'help <command>'
    if (argc == 1) {
        const char* subcmd = argv[0];
        auto it = g_commands.find(subcmd);
        if (it != g_commands.end() && it->second.usage && strlen(it->second.usage) > 0) {
            BATTERY_CLI_LOGI("Command: %s", it->second.name);
            BATTERY_CLI_LOGI("Description: %s", it->second.description);
            BATTERY_CLI_LOGI("Usage: ohos-battery-manager %s", it->second.usage);
            return 0;
        } else {
            BATTERY_CLI_LOGE("Unknown command or no detailed help available: %s", subcmd);
            BATTERY_CLI_LOGI("Run 'ohos-battery-manager help' to see all available commands.");
            return 1;
        }
    }

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
        PrintError("ERR_CONNECTION_FAIL",
            "Failed to get battery capacity. Please check if powermgr process is running.");
        return 1;
    }

    nlohmann::json data;
    data["capacity"] = capacity;
    PrintSuccess(data);
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
        PrintError("ERR_CONNECTION_FAIL",
            "Failed to get total battery energy. Please check if powermgr process is running and caller has system permission.");
        return 1;
    }

    nlohmann::json data;
    data["totalEnergy"] = totalEnergy;
    PrintSuccess(data);
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
        PrintError("ERR_CONNECTION_FAIL",
            "Failed to get remaining battery energy. Please check if powermgr process is running and caller has system permission.");
        return 1;
    }

    nlohmann::json data;
    data["remainEnergy"] = remainEnergy;
    PrintSuccess(data);
    return 0;
}

static void InitCommands()
{
    RegisterCommand("help", "Show help message", CmdHelp, "[command]");
    RegisterCommand("capacity", "Query battery capacity percentage (0-100%)", CmdCapacity, "");
    RegisterCommand("total-energy", "Query battery total energy (mAh)", CmdTotalEnergy, "");
    RegisterCommand("remain-energy", "Query battery remaining energy (mAh)", CmdRemainEnergy, "");
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

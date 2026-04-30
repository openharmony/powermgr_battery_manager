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
#include <cstring>

#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_srv_errors.h"
#include "nlohmann/json.hpp"

namespace {
// Log macros: help output uses CLI_LOG (no prefix), errors use CLI_ERROR
#define CLI_LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define CLI_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)

using CommandHandler = std::function<int(int, char**)>;

// Command struct matching spec template (03-1-code-template.md)
struct Command {
    const char* name;
    const char* description;
    const char* usage;
    const char* parameters;
    const char* examples;
    CommandHandler handler;
};

static std::unordered_map<std::string, Command> gCommands;
static const char* gProgramName = "";
static bool gHasSubcommands = false;
static const char* gToolDescription =
    "Battery capacity and energy query tool. "
    "Used for system management, maintenance troubleshooting and battery status inspection. "
    "Not applicable for real-time battery monitoring or battery event subscription.";
static constexpr uint32_t CLI_CMD_MIN_PARAM_COUNT = 2;

// Registration macro matching spec template
#define REGISTER_CMD(name, desc, usage, params, examples, handler) \
    gCommands[name] = { name, desc, usage, params, examples, handler }
} // namespace

// Success response: includes type field per spec
static int OutputSuccess(const nlohmann::json& data)
{
    nlohmann::json response;
    response["type"] = "result";
    response["status"] = "success";
    response["data"] = data;
    std::cout << response.dump() << std::endl;
    return 0;
}

// Error response: includes type field, no data field per spec
static int OutputError(const std::string& errCode, const std::string& errMsg,
    const std::string& suggestion)
{
    nlohmann::json response;
    response["type"] = "result";
    response["status"] = "error";
    response["errCode"] = errCode;
    response["errMsg"] = errMsg;
    response["suggestion"] = suggestion;
    std::cout << response.dump() << std::endl;
    return 1;
}

// Help command matching spec template (03-1-code-template.md)
static int CmdHelp(int argc, char** argv)
{
    std::string targetCmd;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            targetCmd = argv[i];
            break;
        }
    }

    // ========== Full help (no subcommand argument) ==========
    if (targetCmd.empty()) {
        // Title: <name> - <description>
        CLI_LOG("%s - %s", gProgramName, gToolDescription);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s [options]", gProgramName);
        if (gHasSubcommands) {
            CLI_LOG("  %s <command> [options]", gProgramName);
        }
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --help             Display this help message");
        if (gHasSubcommands) {
            CLI_LOG("");
            CLI_LOG("SubCommands:");
            for (const auto& pair : gCommands) {
                CLI_LOG("  %-18s %s", pair.first.c_str(),
                    pair.second.description ? pair.second.description : "");
            }
        }
        CLI_LOG("");
        CLI_LOG("Examples:");
        if (gHasSubcommands) {
            CLI_LOG("  %s --help", gProgramName);
            for (const auto& pair : gCommands) {
                CLI_LOG("  %s %s --help", gProgramName, pair.first.c_str());
                break;
            }
        }
        return 0;
    }

    // ========== Single command help ==========
    auto it = gCommands.find(targetCmd);
    if (it == gCommands.end()) {
        CLI_ERROR("Unknown command: %s", targetCmd.c_str());
        return 1;
    }
    const Command& cmd = it->second;
    // Title: <name> <cmd> - <description>
    CLI_LOG("%s %s - %s", gProgramName, cmd.name, cmd.description ? cmd.description : "N/A");
    // Usage
    if (cmd.usage) {
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s", cmd.usage);
    }
    // Parameters
    if (cmd.parameters) {
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("%s", cmd.parameters);
    }
    CLI_LOG("    %-18s %s", "--help", "Display this help message");
    // Examples
    if (cmd.examples) {
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("%s", cmd.examples);
    }
    return 0;
}

static int CmdCapacity([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        CLI_ERROR("Unexpected argument for 'capacity' command");
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'capacity' command. This command takes no arguments.",
            "Usage: ohos-batteryManager capacity");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t capacity = client.GetCapacity();
    if (capacity == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        CLI_ERROR("Failed to get battery capacity, service may be unavailable");
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get battery capacity. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running: ps -ef | grep powermgr");
    }

    nlohmann::json data;
    data["capacity"] = capacity;
    return OutputSuccess(data);
}

static int CmdTotalEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        CLI_ERROR("Unexpected argument for 'total-energy' command");
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'total-energy' command. This command takes no arguments.",
            "Usage: ohos-batteryManager total-energy");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t totalEnergy = client.GetTotalEnergy();
    if (totalEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        CLI_ERROR("Failed to get total battery energy, service may be unavailable or permission denied");
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get total battery energy. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running and caller has system permission.");
    }

    nlohmann::json data;
    data["totalEnergy"] = totalEnergy;
    return OutputSuccess(data);
}

static int CmdRemainEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        CLI_ERROR("Unexpected argument for 'remain-energy' command");
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'remain-energy' command. This command takes no arguments.",
            "Usage: ohos-batteryManager remain-energy");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t remainEnergy = client.GetRemainEnergy();
    if (remainEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        CLI_ERROR("Failed to get remaining battery energy, service may be unavailable or permission denied");
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get remaining battery energy. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running and caller has system permission.");
    }

    nlohmann::json data;
    data["remainEnergy"] = remainEnergy;
    return OutputSuccess(data);
}

static void InitCommands()
{
    REGISTER_CMD("capacity", "Query battery capacity percentage (0-100%)",
        "ohos-batteryManager capacity",
        "    (No parameters. This command takes no arguments.)",
        "    # Query battery capacity\n"
        "    ohos-batteryManager capacity",
        CmdCapacity);

    REGISTER_CMD("total-energy", "Query battery total energy (mAh)",
        "ohos-batteryManager total-energy",
        "    (No parameters. This command takes no arguments.)",
        "    # Query total energy\n"
        "    ohos-batteryManager total-energy",
        CmdTotalEnergy);

    REGISTER_CMD("remain-energy", "Query battery remaining energy (mAh)",
        "ohos-batteryManager remain-energy",
        "    (No parameters. This command takes no arguments.)",
        "    # Query remaining energy\n"
        "    ohos-batteryManager remain-energy",
        CmdRemainEnergy);

    gHasSubcommands = (gCommands.size() > 1);
}

static void PrintUsage(const char* prog)
{
    CLI_ERROR("Usage: %s <command> [options]", prog);
    CLI_ERROR("Run '%s --help' for more information.", prog);
}

int HandleCommand(int argc, char** argv)
{
    gProgramName = argv[0];

    // Top-level --help check (before minimum args check)
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        InitCommands();
        char* helpArgv[1] = { argv[0] };
        CmdHelp(1, helpArgv);
        return 0;
    }

    if (argc < CLI_CMD_MIN_PARAM_COUNT) {
        PrintUsage(argv[0]);
        return 1;
    }

    InitCommands();
    std::string cmdName = argv[1];

    // Subcommand --help check (before command dispatch)
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            char* helpArgv[2] = { argv[0], const_cast<char*>(cmdName.c_str()) };
            CmdHelp(2, helpArgv);
            return 0;
        }
    }

    auto it = gCommands.find(cmdName);
    if (it == gCommands.end()) {
        CLI_ERROR("Unknown command: %s", cmdName.c_str());
        PrintUsage(argv[0]);
        return 1;
    }

    int cmdArgc = argc - CLI_CMD_MIN_PARAM_COUNT;
    char** cmdArgv = argv + CLI_CMD_MIN_PARAM_COUNT;
    return it->second.handler(cmdArgc, cmdArgv);
}

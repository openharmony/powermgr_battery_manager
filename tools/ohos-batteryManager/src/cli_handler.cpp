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
#include "cJSON.h"

namespace {
using CommandHandler = std::function<int(int, char**)>;
struct Command {
    const char* name;
    const char* description;
    const char* usage;
    const char* parameters;
    const char* examples;
    CommandHandler handler;
};

struct cJSONDeleter {
    void operator()(cJSON* obj) const { cJSON_Delete(obj); }
};
using cJSONPtr = std::unique_ptr<cJSON, cJSONDeleter>;

static constexpr uint32_t CLI_SUCCESS = 0;
static constexpr uint32_t CLI_FAILURE = 1;
static constexpr uint32_t CLI_CMD_PARAM_INDEX_1 = 1;
static constexpr uint32_t CLI_CMD_PARAM_INDEX_2 = 2;
static const char* CLI_TOOL_NAME = nullptr;
static constexpr const char* TOOL_DESCRIPTION =
    "Battery capacity and energy query tool. "
    "Used for system management, maintenance troubleshooting and battery status inspection. "
    "Not applicable for real-time battery monitoring or battery event subscription.";
static std::unordered_map<std::string, Command> g_commands;
static bool g_hasSubcommands = false;

#define CLI_LOG(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define REGISTER_CMD(name, desc, usage, params, examples, handler) \
    g_commands[name] = { name, desc, usage, params, examples, handler }

bool HasHelpFlag(int argc, char** argv, int start)
{
    for (int i = start; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            return true;
        }
    }
    return false;
}
} // namespace

static int OutputFallbackError(const std::string& errCode, const std::string& errMsg,
    const std::string& suggestion)
{
    std::cout << "{\"type\":\"result\",\"status\":\"failed\",\"errCode\":\"" << errCode
              << "\",\"errMsg\":\"" << errMsg
              << "\",\"suggestion\":\"" << suggestion << "\"}" << std::endl;
    return CLI_FAILURE;
}

// Transfers ownership of data to internal JSON response. Caller must not use data after this call.
static int OutputSuccess(cJSONPtr& data)
{
    cJSONPtr root(cJSON_CreateObject());
    if (!root) {
        return OutputFallbackError("ERR_JSON_ALLOC_FAILED",
            "Failed to create JSON response.",
            "System memory may be insufficient. Try again later.");
    }
    cJSON_AddStringToObject(root.get(), "type", "result");
    cJSON_AddStringToObject(root.get(), "status", "success");
    cJSON_AddItemToObject(root.get(), "data", data.release());

    char* output = cJSON_PrintUnformatted(root.get());
    if (output) {
        std::cout << output << std::endl;
        cJSON_free(output);
    }
    return CLI_SUCCESS;
}

static int OutputError(const std::string& errCode, const std::string& errMsg,
    const std::string& suggestion)
{
    cJSONPtr root(cJSON_CreateObject());
    if (!root) {
        return OutputFallbackError("ERR_JSON_ALLOC_FAILED",
            "Failed to create JSON response.",
            "System memory may be insufficient. Try again later.");
    }
    cJSON_AddStringToObject(root.get(), "type", "result");
    cJSON_AddStringToObject(root.get(), "status", "failed");
    cJSON_AddStringToObject(root.get(), "errCode", errCode.c_str());
    cJSON_AddStringToObject(root.get(), "errMsg", errMsg.c_str());
    cJSON_AddStringToObject(root.get(), "suggestion", suggestion.c_str());

    char* output = cJSON_PrintUnformatted(root.get());
    if (output) {
        std::cout << output << std::endl;
        cJSON_free(output);
    }
    return CLI_FAILURE;
}

static void PrintFullHelp()
{
    CLI_LOG("%s - %s", CLI_TOOL_NAME, TOOL_DESCRIPTION);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s [options]", CLI_TOOL_NAME);
    if (g_hasSubcommands) {
        CLI_LOG("  %s <command> [options]", CLI_TOOL_NAME);
    }
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --help             Display this help message");
    if (g_hasSubcommands) {
        CLI_LOG("");
        CLI_LOG("SubCommands:");
        for (const auto& pair : g_commands) {
            CLI_LOG("  %-18s %s", pair.first.c_str(),
                pair.second.description ? pair.second.description : "");
        }
    }
    CLI_LOG("");
    CLI_LOG("Examples:");
    if (g_hasSubcommands) {
        CLI_LOG("  %s --help", CLI_TOOL_NAME);
        for (const auto& pair : g_commands) {
            CLI_LOG("  %s %s --help", CLI_TOOL_NAME, pair.first.c_str());
            break;
        }
    }
}

static int PrintSubcommandHelp(const std::string& targetCmd)
{
    auto it = g_commands.find(targetCmd);
    if (it == g_commands.end()) {
        return OutputError("ERR_UNKNOWN_COMMAND",
            std::string("Unknown command: '") + targetCmd + "'.",
            std::string("Run '") + CLI_TOOL_NAME + " --help' for available commands.");
    }
    const Command& cmd = it->second;
    CLI_LOG("%s %s - %s", CLI_TOOL_NAME, cmd.name, cmd.description ? cmd.description : "N/A");
    if (cmd.usage) {
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s", cmd.usage);
    }
    if (cmd.parameters) {
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("%s", cmd.parameters);
    }
    CLI_LOG("    %-18s %s", "--help", "Display this help message");
    if (cmd.examples) {
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("%s", cmd.examples);
    }
    return CLI_SUCCESS;
}

static int CmdCapacity([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'capacity' command. This command takes no arguments.",
            "Usage: ohos-batteryManager capacity");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t capacity = client.GetCapacity();
    if (capacity == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get battery capacity. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running: ps -ef | grep powermgr");
    }

    cJSONPtr data(cJSON_CreateObject());
    cJSON_AddNumberToObject(data.get(), "capacity", capacity);
    return OutputSuccess(data);
}

static int CmdTotalEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'total-energy' command. This command takes no arguments.",
            "Usage: ohos-batteryManager total-energy");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t totalEnergy = client.GetTotalEnergy();
    if (totalEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get total battery energy. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running and caller has system permission.");
    }

    cJSONPtr data(cJSON_CreateObject());
    cJSON_AddNumberToObject(data.get(), "totalEnergy", totalEnergy);
    return OutputSuccess(data);
}

static int CmdRemainEnergy([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    if (argc > 0) {
        return OutputError("ERR_ARG_INVALID",
            "Unexpected argument for 'remain-energy' command. This command takes no arguments.",
            "Usage: ohos-batteryManager remain-energy");
    }
    auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
    int32_t remainEnergy = client.GetRemainEnergy();
    if (remainEnergy == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
        return OutputError("ERR_BATT_SERVICE_UNAVAILABLE",
            "Failed to get remaining battery energy. BatterySrvClient returned invalid value.",
            "Check if powermgr process is running and caller has system permission.");
    }

    cJSONPtr data(cJSON_CreateObject());
    cJSON_AddNumberToObject(data.get(), "remainEnergy", remainEnergy);
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

    g_hasSubcommands = (!g_commands.empty());
}

int HandleCommand(int argc, char** argv)
{
    CLI_TOOL_NAME = argv[0];
    InitCommands();

    if (argc < CLI_CMD_PARAM_INDEX_2) {
        return OutputError("ERR_NO_COMMAND",
            "No command specified.",
            std::string("Run '") + argv[0] + " --help' for available commands.");
    }
    if (strcmp(argv[CLI_CMD_PARAM_INDEX_1], "--help") == 0) {
        PrintFullHelp();
        return CLI_SUCCESS;
    }

    std::string cmdName = argv[CLI_CMD_PARAM_INDEX_1];
    auto it = g_commands.find(cmdName);
    if (it == g_commands.end()) {
        return OutputError("ERR_UNKNOWN_COMMAND",
            std::string("Unknown command: '") + cmdName + "'.",
            std::string("Run '") + argv[0] + " --help' for available commands.");
    }
    if (HasHelpFlag(argc, argv, CLI_CMD_PARAM_INDEX_2)) {
        return PrintSubcommandHelp(cmdName);
    }
    return it->second.handler(argc - CLI_CMD_PARAM_INDEX_2, argv + CLI_CMD_PARAM_INDEX_2);
}

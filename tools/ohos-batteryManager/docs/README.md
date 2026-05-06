# ohos-batteryManager

## 概述

Battery capacity and energy query CLI tool. Queries battery status via BatteryService (SA 3302).

## 功能列表

- Query battery capacity percentage (0-100%)
- Query battery total energy (mAh)
- Query battery remaining energy (mAh)
- View command help information (--help)

## 依赖

- System Ability: BatteryService (SA 3302, process: powermgr)
- Inner API: `batterysrv_client` (`BatterySrvClient`)
- Permission: `total-energy` and `remain-energy` commands require system caller identity (`Permission::IsSystem()`)

## 基本用法

```bash
ohos-batteryManager <command> [options]
```

## 命令列表

| 命令 | 说明 | 参数 | 权限 | 前置依赖 |
|------|------|------|------|----------|
| capacity | Query battery capacity percentage (0-100%) | 无 | 无 | 无 |
| total-energy | Query battery total energy (mAh) | 无 | System caller identity | 无 |
| remain-energy | Query battery remaining energy (mAh) | 无 | System caller identity | 无 |

**前置依赖说明**：
- **无**: The command can be executed directly without prerequisites

## 示例

```bash
# View all command help
ohos-batteryManager --help

# View subcommand help
ohos-batteryManager capacity --help
ohos-batteryManager total-energy --help
ohos-batteryManager remain-energy --help

# Query battery capacity
ohos-batteryManager capacity
# Output: {"type":"result","status":"success","data":{"capacity":85}}

# Query battery total energy
ohos-batteryManager total-energy
# Output: {"type":"result","status":"success","data":{"totalEnergy":4000}}

# Query battery remaining energy
ohos-batteryManager remain-energy
# Output: {"type":"result","status":"success","data":{"remainEnergy":3200}}
```

## 错误处理示例

```bash
# Extra arguments rejected
ohos-batteryManager capacity extra_arg
# stderr: [ERROR] Unexpected argument for 'capacity' command
# stdout: {"type":"result","status":"failed","errCode":"ERR_ARG_INVALID","errMsg":"Unexpected argument for 'capacity' command. This command takes no arguments.","suggestion":"Usage: ohos-batteryManager capacity"}

# Unknown command
ohos-batteryManager unknown
# stderr: [ERROR] Unknown command: unknown
# stderr: [ERROR] Usage: ohos-batteryManager <command> [options]
```

## 输出格式

- **stdout**: JSON result
- **stderr**: Logs and debug information

### 成功响应

```json
{"type":"result","status":"success","data":{"capacity":85}}
```

### 失败响应

```json
{"type":"result","status":"failed","errCode":"ERR_BATT_SERVICE_UNAVAILABLE","errMsg":"Failed to get battery capacity. BatterySrvClient returned invalid value.","suggestion":"Check if powermgr process is running: ps -ef | grep powermgr"}
```

## 编译

```bash
# Enable feature flag in product config or gn args
battery_manager_feature_support_battery_cli=true

# Build
./build.sh --product-name rk3568 \
    --build-target //base/powermgr/battery_manager/tools/ohos-batteryManager:ohos-batteryManager \
    --ccache

# Install path
/system/bin/cli_tool/executable/ohos-batteryManager
```

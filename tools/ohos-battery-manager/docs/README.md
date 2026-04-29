# ohos-battery-manager

Battery information query CLI tool for OpenHarmony.

## Overview

Query battery capacity and energy status via system SA (System Ability 3302).

## Commands

| Command | Description | Parameters | Permissions |
|---------|-------------|------------|-------------|
| capacity | Query battery capacity (0-100%) | None | None |
| total-energy | Query battery total energy (mAh) | None | System caller identity |
| remain-energy | Query battery remaining energy (mAh) | None | System caller identity |

## Usage

```bash
ohos-battery-manager <command>
```

### Examples

```bash
# Query battery capacity
ohos-battery-manager capacity
# Output: {"success":true,"data":{"capacity":85}}

# Query battery total energy
ohos-battery-manager total-energy
# Output: {"success":true,"data":{"totalEnergy":4000}}

# Query battery remaining energy
ohos-battery-manager remain-energy
# Output: {"success":true,"data":{"remainEnergy":3200}}

# Show help
ohos-battery-manager help
```

### Error Handling

```bash
# Extra arguments are rejected
ohos-battery-manager capacity extra_arg
# stderr: [ERROR] Unexpected argument for 'capacity' command
# stderr: [INFO] Usage: ohos-battery-manager capacity

# Unknown command
ohos-battery-manager unknown
# stderr: [ERROR] Unknown command: unknown
# stderr: [INFO] Usage: ohos-battery-manager <command> [options]
```

## Output Format

- **stdout**: JSON result
- **stderr**: Log and debug info

### Success Response

```json
{"success":true,"data":{"capacity":85}}
```

### Error Response

```json
{"success":false,"error":{"code":"ERR_CONNECTION_FAIL","message":"Failed to get battery capacity."},"suggestion":"Check if powermgr process is running."}
```

## Dependencies

- System Ability: BatteryService (SA 3302, process: powermgr)
- Inner API: `batterysrv_client` (`BatterySrvClient`)
- Permissions: `total-energy` and `remain-energy` require system caller identity (`Permission::IsSystem()`)

## Build

```bash
# Enable feature flag first (in product config or gn args)
battery_manager_feature_support_battery_cli=true

# Build
./build.sh --product-name rk3568 \
    --build-target //base/powermgr/battery_manager/tools/ohos-battery-manager:ohos-battery-manager \
    --ccache

# Install path
/system/bin/cli_tool/executable/ohos-battery-manager
```

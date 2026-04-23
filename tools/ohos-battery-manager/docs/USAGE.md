# Usage

## Basic Usage

```bash
ohos-battery-manager <command>
```

## Commands

| Command | Description | Parameters | Permissions | Dependencies |
|---------|-------------|------------|-------------|--------------|
| capacity | Query battery capacity (0-100%) | None | None | None |
| total-energy | Query battery total energy (mAh) | None | System caller identity | None |
| remain-energy | Query battery remaining energy (mAh) | None | System caller identity | None |

## Examples

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

## Error Handling

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

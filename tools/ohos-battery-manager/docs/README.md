# ohos-battery-manager

Battery information query CLI tool for OpenHarmony.

## Overview

Query battery capacity and energy status via system SA (System Ability 3302).

## Features

- `capacity` - Query battery capacity percentage (0-100%)
- `total-energy` - Query battery total energy (mAh)
- `remain-energy` - Query battery remaining energy (mAh)

## Dependencies

- System Ability: BatteryService (SA 3302, process: powermgr)
- Inner API: `batterysrv_client` (`BatterySrvClient`)
- Permissions: `total-energy` and `remain-energy` require system caller identity (`Permission::IsSystem()`)

## Build Target

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

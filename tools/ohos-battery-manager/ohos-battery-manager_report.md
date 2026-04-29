# ohos-battery-manager 封装报告

## 1. 概述

| 项目 | 内容 |
|------|------|
| **CLI 名称** | ohos-battery-manager |
| **功能描述** | 电池电量查询与电池容量查询工具，支持查询电池电量百分比、总能量和剩余能量 |
| **仓库类型** | 开源仓（ohos- 前缀） |
| **代码路径** | tools/ohos-battery-manager/ |
| **所属子系统** | powermgr |
| **所属部件** | battery_manager |
| **SA 依赖** | BatteryService (SA 3302, 进程: powermgr) |

## 2. 接口分析

### 2.1 候选接口列表

接口来源：`BatterySrvClient`（`interfaces/inner_api/native/include/battery_srv_client.h`），共 16 个方法。经用户确认，仅封装以下 3 个接口：

| 接口名 | 返回类型 | 功能 | 权限 |
|--------|---------|------|------|
| `GetCapacity()` | `int32_t` | 电池电量百分比 (0-100) | 无（公开读取） |
| `GetTotalEnergy()` | `int32_t` | 电池总能量 (mAh) | `Permission::IsSystem()` |
| `GetRemainEnergy()` | `int32_t` | 电池剩余能量 (mAh) | `Permission::IsSystem()` |

### 2.2 权限需求分析

| 权限标识 | 用途说明 | 涉及命令 |
|---------|---------|----------|
| 无 | 电量百分比为公开信息 | capacity |
| `Permission::IsSystem()` | 系统调用者身份检查（非 ohos.permission） | total-energy, remain-energy |

> 注：`GetTotalEnergy()` 和 `GetRemainEnergy()` 服务端通过 `Permission::IsSystem()` 检查调用者身份，CLI 以 root shell 或 native 进程运行时自动满足此条件。

### 2.3 异步分析结果

| 接口 | 类型 | 判断依据 | 封装决策 |
|------|------|---------|----------|
| `GetCapacity()` | 同步 | 直接返回 `int32_t`，无回调 | ✅ 封装 |
| `GetTotalEnergy()` | 同步 | 直接返回 `int32_t`，同步 IPC | ✅ 封装 |
| `GetRemainEnergy()` | 同步 | 直接返回 `int32_t`，同步 IPC | ✅ 封装 |

## 3. 命令设计

### 3.1 命令列表

| CLI 命令 | 对应接口 | 权限 | 参数 | 说明 |
|---------|---------|------|------|------|
| `capacity` | `GetCapacity()` | 无 | 无 | 查询电池电量百分比 (0-100%) |
| `total-energy` | `GetTotalEnergy()` | 系统调用者 | 无 | 查询电池总能量 (mAh) |
| `remain-energy` | `GetRemainEnergy()` | 系统调用者 | 无 | 查询电池剩余能量 (mAh) |

### 3.2 命令依赖关系

| CLI 命令 | 前置依赖命令 | 依赖条件 | 说明 |
|---------|------------|---------|------|
| capacity | 无 | - | 直接调用 BatterySrvClient |
| total-energy | 无 | - | 直接调用 BatterySrvClient |
| remain-energy | 无 | - | 直接调用 BatterySrvClient |

## 4. 代码实现

### 4.1 文件结构

```text
tools/ohos-battery-manager/
├── src/
│   └── main.cpp              # 主入口，3 个命令（help/capacity/total-energy/remain-energy）
├── tests/
│   ├── BUILD.gn              # 测试构建配置
│   └── test_main.cpp         # GTest 测试，5 个用例
├── docs/
│   ├── README.md             # 项目概述
│   └── USAGE.md              # 使用说明
├── BUILD.gn                  # ohos_executable 构建配置
├── config.json               # CLI 工具描述文件
└── ohos-battery-manager_report.md
```

### 4.2 关键代码说明

**命令分发**：使用 `std::unordered_map` 静态命令表
```cpp
static std::unordered_map<std::string, Command> g_commands;

RegisterCommand("help", "Show help message", CmdHelp);
RegisterCommand("capacity", "Query battery capacity (0-100%)", CmdCapacity);
RegisterCommand("total-energy", "Query battery total energy (mAh)", CmdTotalEnergy);
RegisterCommand("remain-energy", "Query battery remaining energy (mAh)", CmdRemainEnergy);
```

**API 调用**：通过 `OHOS::DelayedRefSingleton<BatterySrvClient>` 单例访问
```cpp
auto& client = OHOS::DelayedRefSingleton<OHOS::PowerMgr::BatterySrvClient>::GetInstance();
int32_t capacity = client.GetCapacity();
```

**哨兵值判断**：使用源码定义的 `INVALID_BATT_INT_VALUE` 检测 API 调用失败
```cpp
if (capacity == OHOS::PowerMgr::INVALID_BATT_INT_VALUE) {
    // API 调用失败，服务不可用
}
```

### 4.3 错误码对齐

| 常量 | 值 | 来源 | 用途 |
|------|-----|------|------|
| `INVALID_BATT_INT_VALUE` | -1 | `battery_info.h` | GetCapacity/GetTotalEnergy/GetRemainEnergy 失败哨兵 |
| `ERR_FAILURE` | 1 | `battery_srv_errors.h` | 一般失败 |
| `ERR_CONNECTION_FAIL` | 5100101 | `battery_srv_errors.h` | 服务连接失败 |

### 4.4 BUILD.gn 配置

| 配置项 | 值 |
|--------|-----|
| target | `ohos_executable("ohos-battery-manager")` |
| sanitize | `cfi = true, cfi_cross_dso = true, debug = false` |
| branch_protector_ret | `"pac_ret"` |
| deps | `batterysrv_client` |
| external_deps | `c_utils:utils, hilog:libhilog, ipc:ipc_core, samgr:samgr_proxy` |
| install_images | `["system"]` |
| relative_install_dir | `"cli_tool/executable"` |

### 4.5 修改的文件

| 文件 | 操作 |
|------|------|
| `tools/ohos-battery-manager/src/main.cpp` | 新建 |
| `tools/ohos-battery-manager/BUILD.gn` | 新建 |
| `tools/ohos-battery-manager/config.json` | 新建 |
| `tools/ohos-battery-manager/tests/BUILD.gn` | 新建 |
| `tools/ohos-battery-manager/tests/test_main.cpp` | 新建 |
| `tools/ohos-battery-manager/docs/README.md` | 新建 |
| `tools/ohos-battery-manager/docs/USAGE.md` | 新建 |
| `bundle.json` | 修改（fwk_group 添加 CLI target，test 添加测试 target） |

## 5. 编译验证

### 5.1 编译配置

| 项目 | 值 |
|------|-----|
| **编译路径** | `/home/wangyantian/workspace/ohos_master` |
| **Product** | rk3568 |
| **Target CPU** | arm |
| **编译命令（CLI）** | `./build.sh --product-name rk3568 --build-target ohos-battery-manager --ccache` |
| **编译命令（Test）** | `./build.sh --product-name rk3568 --build-target test_ohos_battery_manager_cli --ccache` |

### 5.2 编译过程

**尝试 1**：
- **结果**: 失败
- **阶段**: GN
- **错误**: `Assertion failed. Build targets in battery_manager should fully enable cfi`
- **修复**: BUILD.gn 添加 `sanitize = { cfi = true, cfi_cross_dso = true, debug = false }` 和 `branch_protector_ret = "pac_ret"`

**尝试 2**：
- **结果**: 失败
- **阶段**: Ninja (CXX compile)
- **错误**: `no template named 'DelayedRefSingleton' in namespace 'OHOS::PowerMgr'`
- **修复**: main.cpp 中 `OHOS::PowerMgr::DelayedRefSingleton` 改为 `OHOS::DelayedRefSingleton`

**尝试 3**：
- **结果**: 成功
- **产物**: `out/rk3568/powermgr/battery_manager/ohos-battery-manager` (26KB)

**测试编译**：
- **错误**: test_main.cpp 同样的 `DelayedRefSingleton` 命名空间问题
- **修复**: 将 `DelayedRefSingleton` 改为 `OHOS::DelayedRefSingleton`
- **结果**: 成功
- **产物**: `out/rk3568/tests/unittest/battery_manager/battery_manager/test_ohos_battery_manager_cli`

### 5.3 最终结果

✅ **CLI 编译成功**
- **尝试次数**: 3（2 次失败 + 1 次成功）
- **产物路径（stripped）**: `out/rk3568/powermgr/battery_manager/ohos-battery-manager`
- **产物路径（unstripped）**: `out/rk3568/exe.unstripped/powermgr/battery_manager/ohos-battery-manager`
- **文件大小**: 26KB
- **文件类型**: ELF 32-bit LSB shared object, ARM, EABI5

✅ **测试编译成功**
- **产物路径**: `out/rk3568/tests/unittest/battery_manager/battery_manager/test_ohos_battery_manager_cli`

## 6. 使用说明

### 6.1 安装路径

```
/system/bin/cli_tool/executable/ohos-battery-manager
```

### 6.2 命令示例

**查询电池电量**：
```bash
ohos-battery-manager capacity
# 输出: {"success":true,"data":{"capacity":85}}
```

**查询电池总能量**：
```bash
ohos-battery-manager total-energy
# 输出: {"success":true,"data":{"totalEnergy":4000}}
```

**查询电池剩余能量**：
```bash
ohos-battery-manager remain-energy
# 输出: {"success":true,"data":{"remainEnergy":3200}}
```

**帮助信息**：
```bash
ohos-battery-manager help
```

### 6.3 权限配置

- `capacity` 命令：无权限要求，任何调用者可执行
- `total-energy` 和 `remain-energy` 命令：需要系统调用者身份，CLI 以 root shell 或 native 进程运行时自动满足

## 7. 待办事项

### 7.1 未封装接口

| 接口名 | 原因 |
|--------|------|
| `GetChargingStatus()` | 用户未要求 |
| `GetHealthStatus()` | 用户未要求 |
| `GetPluggedType()` | 用户未要求 |
| `GetVoltage()` | 用户未要求 |
| `GetPresent()` | 用户未要求 |
| `GetTechnology()` | 用户未要求 |
| `GetBatteryTemperature()` | 用户未要求 |
| `GetNowCurrent()` | 用户未要求 |
| `GetCapacityLevel()` | 用户未要求 |
| `GetRemainingChargeTime()` | 用户未要求 |
| `SetBatteryConfig()` | 用户未要求 |
| `GetBatteryConfig()` | 用户未要求 |
| `IsBatteryConfigSupported()` | 用户未要求 |

### 7.2 优化建议

1. 添加 `--format` 参数支持 table/text 等多种输出格式
2. 添加 `watch` 命令支持实时监控电量变化（需结合 BatteryInfo 事件订阅）
3. 将剩余未封装的查询类接口（如充电状态、电压、温度等）纳入后续迭代

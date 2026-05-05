# ohos-batteryManager 封装报告

## 1. 概述

| 项目 | 内容 |
|------|------|
| **CLI 名称** | ohos-batteryManager |
| **功能描述** | 电池电量查询与电池容量查询工具，支持查询电池电量百分比、总能量和剩余能量 |
| **仓库类型** | 开源仓（ohos- 前缀） |
| **代码路径** | tools/ohos-batteryManager/ |
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
tools/ohos-batteryManager/
├── src/
│   ├── main.cpp              # 主入口，调用 HandleCommand
│   ├── cli_handler.h         # 命令处理头文件
│   └── cli_handler.cpp       # 命令分发与实现（capacity/total-energy/remain-energy）
├── tests/
│   ├── BUILD.gn              # 测试构建配置
│   └── test_main.cpp         # GTest 测试，22 个用例
├── docs/
│   └── README.md             # 项目概述与使用说明
├── BUILD.gn                  # ohos_executable 构建配置
└── ohos-batteryManager.json  # CLI 工具描述文件
```

### 4.2 关键代码说明

**命令分发**：使用 `REGISTER_CMD` 宏注册到 `std::unordered_map` 静态命令表
```cpp
static std::unordered_map<std::string, Command> gCommands;

REGISTER_CMD("capacity", "Query battery capacity percentage (0-100%)",
    "ohos-batteryManager capacity", ..., CmdCapacity);
REGISTER_CMD("total-energy", "Query battery total energy (mAh)",
    "ohos-batteryManager total-energy", ..., CmdTotalEnergy);
REGISTER_CMD("remain-energy", "Query battery remaining energy (mAh)",
    "ohos-batteryManager remain-energy", ..., CmdRemainEnergy);
```

**JSON 输出**：使用 `cJSON` 构建标准信封格式响应，通过 RAII 封装 (`cJSONPtr`) 管理内存
```cpp
// 成功: {"type":"result","status":"success","data":{...}}
response["type"] = "result";
response["status"] = "success";
response["data"] = data;

// 失败: {"type":"result","status":"error","errCode":"...","errMsg":"...","suggestion":"..."}
response["type"] = "result";
response["status"] = "error";
response["errCode"] = errCode;
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

| CLI 错误码 | 含义 | 触发条件 |
|------------|------|---------|
| `ERR_ARG_INVALID` | 参数无效 | 子命令接收到多余参数 |
| `ERR_BATT_SERVICE_UNAVAILABLE` | 电池服务不可用 | `GetCapacity()`/`GetTotalEnergy()`/`GetRemainEnergy()` 返回 `INVALID_BATT_INT_VALUE` |

| 常量 | 值 | 来源 | 用途 |
|------|-----|------|------|
| `INVALID_BATT_INT_VALUE` | -1 | `battery_info.h` | API 调用失败哨兵值 |

### 4.4 BUILD.gn 配置

| 配置项 | 值 |
|--------|-----|
| target | `ohos_executable("ohos-batteryManager")` |
| sanitize | `cfi = true, cfi_cross_dso = true, debug = false` |
| branch_protector_ret | `"pac_ret"` |
| deps | `batterysrv_client` |
| external_deps | `c_utils:utils, hilog:libhilog, ipc:ipc_core, samgr:samgr_proxy` |
| install_images | `["system"]` |
| relative_install_dir | `"cli_tool/executable"` |

### 4.5 修改的文件

| 文件 | 操作 |
|------|------|
| `tools/ohos-batteryManager/src/main.cpp` | 新建 |
| `tools/ohos-batteryManager/src/cli_handler.h` | 新建 |
| `tools/ohos-batteryManager/src/cli_handler.cpp` | 新建 |
| `tools/ohos-batteryManager/BUILD.gn` | 新建 |
| `tools/ohos-batteryManager/ohos-batteryManager.json` | 新建 |
| `tools/ohos-batteryManager/tests/BUILD.gn` | 新建 |
| `tools/ohos-batteryManager/tests/test_main.cpp` | 新建 |
| `tools/ohos-batteryManager/docs/README.md` | 新建 |
| `bundle.json` | 修改（fwk_group 添加 CLI target，test 添加测试 target） |

## 5. 编译验证

### 5.1 编译配置

| 项目 | 值 |
|------|-----|
| **编译路径** | `/home/wangyantian/workspace/ohos_master` |
| **Product** | rk3568 |
| **Target CPU** | arm |
| **编译命令（CLI）** | `./build.sh --product-name rk3568 --build-target ohos-batteryManager --ccache` |
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
- **产物**: `out/rk3568/powermgr/battery_manager/ohos-batteryManager` (26KB)

**测试编译**：
- **错误**: test_main.cpp 同样的 `DelayedRefSingleton` 命名空间问题
- **修复**: 将 `DelayedRefSingleton` 改为 `OHOS::DelayedRefSingleton`
- **结果**: 成功
- **产物**: `out/rk3568/tests/unittest/battery_manager/battery_manager/test_ohos_battery_manager_cli`

### 5.3 最终结果

✅ **CLI 编译成功**
- **尝试次数**: 3（2 次失败 + 1 次成功）
- **产物路径（stripped）**: `out/rk3568/powermgr/battery_manager/ohos-batteryManager`
- **产物路径（unstripped）**: `out/rk3568/exe.unstripped/powermgr/battery_manager/ohos-batteryManager`
- **文件大小**: 26KB
- **文件类型**: ELF 32-bit LSB shared object, ARM, EABI5

✅ **测试编译成功**
- **产物路径**: `out/rk3568/tests/unittest/battery_manager/battery_manager/test_ohos_battery_manager_cli`

## 6. 使用说明

### 6.1 安装路径

```
/system/bin/cli_tool/executable/ohos-batteryManager
```

### 6.2 命令示例

**查询电池电量**：
```bash
ohos-batteryManager capacity
# 输出: {"type":"result","status":"success","data":{"capacity":85}}
```

**查询电池总能量**：
```bash
ohos-batteryManager total-energy
# 输出: {"type":"result","status":"success","data":{"totalEnergy":4000}}
```

**查询电池剩余能量**：
```bash
ohos-batteryManager remain-energy
# 输出: {"type":"result","status":"success","data":{"remainEnergy":3200}}
```

**帮助信息**：
```bash
ohos-batteryManager --help
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

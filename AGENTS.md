# AGENTS.md - OpenHarmony 电池服务（Battery Manager）

## 1. 代码地图

本仓库实现 OpenHarmony 电池服务（Battery Manager），SA ID 3302，进程名 `powermgr`，库 `libbatteryservice.z.so`。核心职责是电池信息查询（容量/电压/温度/充电状态/健康状态/技术类型）、充放电状态上报、关机充电模式（charger 子模块）。最重要的架构边界是**客户端 `BatterySrvClient` 通过 IPC 调用 `BatteryService`，客户端只做转发，所有电池事件来自 HDI 上报并由服务端分发**。

### 非本项目维护的目录

以下目录属于其他团队或生成产物，不属于本项目维护范围，修改时请跳过：

- `frameworks/cj/`：CJ 语言绑定（由 CJ 团队维护）
- `frameworks/ets/taihe/`：taihe FFI 生成产物（由 `taihe_ffi_gen` 工具生成，不要手改）
- `tools/ohos-batteryManager/`：CLI 工具（独立 npm 包，仅在新增 CLI 命令时进入）

### 嵌套指引

本仓目前无嵌套 AGENTS.md / CLAUDE.md / rules / skills 文件。以下子模块可按需新建嵌套 AGENTS.md 以提供更具体的指引：

- `charger/`：关机充电模式独立子模块（含独立 `sa_profile/`、`include/dev/`、`src/dev/`、`resources/`），与开机服务互斥运行，可新建 `charger/AGENTS.md` 聚焦独立运行约束、设备适配层（`dev/`）、关机充电 UI 时序与 init 拉起条件
- `tools/ohos-batteryManager/`：`battery-shell` CLI 独立 npm 包，可新建 `tools/ohos-batteryManager/AGENTS.md` 聚焦 CLI 命令注册、npm 发布流程
- `utils/native/` + `utils/hookmgr/`：跨仓共享工具层，被 `power_manager` 等下游仓引用，可新建 `utils/AGENTS.md` 聚焦跨仓工具变更影响评估
- `interfaces/kits/c/` + `frameworks/capi/battery_info/`：NDK 公共 C 接口，ABI 稳定性要求极高，可新建嵌套 AGENTS.md 聚焦 NDK ABI 兼容性约束与 `*.map` 版本脚本维护
- `services/native/notification/`：电池事件通知模块，可新建嵌套 AGENTS.md 聚焦 CommonEvent 发布与 `usual.event.BATTERY_*` 事件契约
- `test/`：测试组织目录，可新建 `test/AGENTS.md` 聚焦 apitest/benchmarktest/fuzztest/systemtest/unittest 五类测试的运行约定

### 关键区域

- `interfaces/inner_api/native/include/`：公共 C++ API 头文件，含 `battery_srv_client.h` 入口、`battery_info.h` 数据模型、`battery_srv_errors.h` 错误码、`napi_utils.h` NAPI 工具。
- `interfaces/kits/c/`：NDK 纯 C 接口，供原生 C 应用使用。
- `frameworks/capi/battery_info/`：C API 实现（NDK 接口实现层）。
- `frameworks/cj/`：CJ 语言绑定。
- `frameworks/ets/taihe/`：taihe FFI **生成产物**。
- `frameworks/napi/`：ArkTS NAPI 绑定（含 `include/` 头文件 + `src/` 实现）。
- `frameworks/native/`：客户端 SDK 基础设施。
- `services/zidl/IBatterySrv.idl`：IPC 接口定义（**本项目使用 `.idl` 文件，proxy/stub 由工具生成**）。
- `services/native/include/`：服务端头文件，关键类 `BatteryService` / `BatteryConfig` / `BatteryLight` / `BatteryNotify` / `BatteryDump` / `ChargingSound` / `HDIServiceStatusListener`。
- `services/native/src/`：服务端实现。
- `services/native/notification/`：电池事件通知模块。
- `services/native/profile/`：电池配置默认值。
- `services/native/resources/`：资源文件（充电音等）。
- `charger/`：**关机充电模式独立子模块**，含独立 `sa_profile/`、`include/dev/`、`src/dev/`、`resources/`，是开机外的另一个执行入口。
- `sa_profile/3302.json`：SA 3302 注册配置，`min_hdi_proxy_version: ["libbattery_proxy_2.0.z.so"]`。
- `batterymgr.gni`：6 个特性开关。
- `batterymgr.yaml`：HiSysEvent 配置。
- `utils/native/`：跨仓共享工具（部分被 `power_manager` 等仓引用）。
- `utils/hookmgr/`：钩子管理器（跨仓共享，charger 模块依赖）。
- `tools/ohos-batteryManager/`：`battery-shell` CLI 命令行工具。
- `test/`：`apitest/` / `benchmarktest/` / `fuzztest/`（17 个）/ `systemtest/` / `unittest/`。

### Where to look

| 任务类型 | 先看哪里 |
|---|---|
| 公共 API 变更 | `interfaces/inner_api/native/include/` 头文件 -> `frameworks/native/` 实现 -> `frameworks/capi/` C 接口 -> `frameworks/napi/` ArkTS 绑定 -> `frameworks/cj/` CJ 绑定 -> `*.map` 版本脚本 |
| IPC 接口变更 | `services/zidl/IBatterySrv.idl` -> 构建生成 `IBatterySrv_proxy.h/.cpp` + `IBatterySrv_stub.h/.cpp` -> `interfaces/inner_api/native/include/battery_srv_client.h` 调用方 |
| 电池信息查询 | `interfaces/inner_api/native/include/battery_srv_client.h`（客户端入口）-> `services/native/include/battery_service.h`（服务端实现）-> `services/zidl/IBatterySrv.idl`（IPC 定义） |
| 充电状态变更上报 | `services/native/include/battery_callback.h` + `services/native/src/battery_callback.cpp`（HDI 上报入口）+ `services/native/include/battery_notify.h`（事件分发） |
| 关机充电模式 | `charger/include/` + `charger/src/` + `charger/include/dev/`（设备适配）+ `charger/src/dev/` + `charger/resources/` + `charger/sa_profile/` |
| 充电音效 | `services/native/include/charging_sound.h` + `batterymgr.gni` 中 `battery_manager_feature_enable_charging_sound` |
| 低电量提醒 | `services/native/include/battery_notify.h` + `batterymgr.gni` 中 `battery_manager_feature_set_low_capacity_threshold` |
| 电池指示灯 | `services/native/include/battery_light.h` + HDI `drivers_interface_light` |
| 电池配置（场景化） | `services/native/include/battery_config.h` + `services/native/profile/` + `IBatterySrv.idl` 中 `SetBatteryConfig`/`GetBatteryConfig`/`IsBatteryConfigSupported` |
| 无线充电 | `batterymgr.gni` 中 `battery_manager_feature_enable_wireless_charge` |
| 权限校验 | grep `AccessTokenKit` / `VerifyAccessToken` 调用点 |
| 特性开关 | `batterymgr.gni` `declare_args()` 段 + `bundle.json` 的 `features` 列表 |
| 新增/修改测试 | `test/apitest/capi/` + `test/apitest/cj/`（C/CJ API 测试）+ `test/unittest/`（单元）+ `test/systemtest/`（系统）+ `test/fuzztest/`（17 个 fuzzer）+ `test/benchmarktest/`（性能） |
| 构建配置 | `bundle.json` + `batterymgr.gni` + 子目录 `BUILD.gn` |

### 架构分层

```
应用层
  ├─ ArkTS 应用 -> frameworks/napi (NAPI 绑定)
  │                └─ frameworks/ets/taihe (taihe FFI 生成)
  ├─ CJ 应用 -> frameworks/cj (CJ 绑定)
  ├─ C 应用 -> interfaces/kits/c + frameworks/capi (NDK C 接口)
  └─ C++ 应用/系统组件 -> interfaces/inner_api/native (完整 C++ API)
          ↓
客户端 SDK
  BatterySrvClient (interfaces/inner_api/native/include/battery_srv_client.h)
    -> IBatterySrv proxy (services/zidl，IDL 生成)
        ↓ IPC (SystemAbility 3302)
服务端
  BatteryService (services/native/include/battery_service.h, 继承 SystemAbility)
    ├─ BatteryCallback (HDI 上报入口，监听 battery_proxy 事件)
    │    └─ BatteryNotify (事件分发：CommonEvent、HiSysEvent、回调)
    ├─ BatteryConfig (场景化电池配置)
    ├─ BatteryLight (指示灯控制)
    ├─ ChargingSound (充电音效，特性开关)
    ├─ BatteryDump (hidumper 入口)
    └─ HDIServiceStatusListener (HDI 上下线监听)

关机充电模式（独立子模块）
  charger/ (含独立 sa_profile、dev 适配、resources)
    └─ 系统启动时由 init 根据 `power_manager_feature_poweroff_charge` 决定是否拉起
```

## 2. 知识路由

在规划或编辑前，先对任务分类，读取对应的代码路径和文档。

### Task-based routing

| 任务类型 | 读取 |
|---|---|
| 公共 API 新增/修改 | `interfaces/inner_api/native/include/*.h` + `frameworks/native/` + `frameworks/capi/` + `frameworks/napi/` + `frameworks/cj/` + `*.map` 版本脚本 |
| IPC 接口变更 | `services/zidl/IBatterySrv.idl`（IDL 源文件，改完由构建生成 proxy/stub）+ `interfaces/inner_api/native/include/battery_srv_client.h` 调用方 |
| 电池信息查询逻辑 | `interfaces/inner_api/native/include/battery_srv_client.h` + `services/native/include/battery_service.h` + `services/native/src/battery_service.cpp` |
| HDI 上报流程 | `services/native/include/battery_callback.h` + `services/native/src/battery_callback.cpp` + HDI `drivers_interface_battery` |
| 电池事件分发 | `services/native/include/battery_notify.h` + `services/native/src/battery_notify.cpp`（CommonEvent 发布 + 回调通知） |
| 关机充电模式 | `charger/include/` + `charger/src/` + `charger/include/dev/` + `charger/src/dev/` + `charger/resources/` + `charger/sa_profile/` + `batterymgr.gni` 中 `battery_manager_feature_enable_charger` |
| 电池配置（场景化） | `services/native/include/battery_config.h` + `services/native/src/battery_config.cpp` + `services/native/profile/` + `IBatterySrv.idl` 中 `SetBatteryConfig`/`GetBatteryConfig`/`IsBatteryConfigSupported` |
| 充电音效 | `services/native/include/charging_sound.h` + `batterymgr.gni` 中 `battery_manager_feature_enable_charging_sound` |
| 无线充电 | `batterymgr.gni` 中 `battery_manager_feature_enable_wireless_charge` + `services/native/src/battery_service.cpp` 无线充电处理 |
| 低电量阈值 | `batterymgr.gni` 中 `battery_manager_feature_set_low_capacity_threshold` + `services/native/src/battery_notify.cpp` 低电量事件 |
| 电池指示灯 | `services/native/include/battery_light.h` + HDI `drivers_interface_light` |
| 通知集成 | `services/native/notification/` + `batterymgr.gni` 中 `battery_manager_feature_support_notification` |
| CLI 工具 | `tools/ohos-batteryManager/` + `batterymgr.gni` 中 `battery_manager_feature_support_battery_cli` |
| 权限校验 | grep `AccessTokenKit` 调用点 + `utils/native/` 中权限工具 |
| 特性开关 | `batterymgr.gni` `declare_args()` + `bundle.json` 的 `features` 列表 |
| 新增/修改测试 | `test/apitest/{capi,cj}/` + `test/unittest/` + `test/systemtest/` + `test/fuzztest/` + `test/benchmarktest/` |
| 构建配置 | `bundle.json`（依赖、syscap、rom/ram）+ `batterymgr.gni`（特性开关、part 检测）+ 各 `BUILD.gn` |
| DFX（日志/事件） | `batterymgr.yaml`（HiSysEvent 配置）+ grep `HiSysEvent` 调用点 |

### Path-based routing

| 修改路径 | 需了解的上下文 |
|---|---|
| `interfaces/inner_api/native/` | 所有 C++ 消费者的公共 API，变更需同步 `frameworks/capi/`、`frameworks/napi/`、`frameworks/cj/`、`frameworks/ets/taihe/` 四套绑定并检查 `*.map` 版本脚本 |
| `interfaces/kits/c/` | NDK 公共 C 接口，ABI 稳定性要求极高，签名变更需评估 NDK 兼容性 |
| `services/zidl/IBatterySrv.idl` | IDL 源文件，修改后构建系统生成 proxy/stub，**不要直接改生成产物** |
| `services/native/include/battery_service.h` | 服务端核心类，所有电池信息查询和事件分发的入口 |
| `services/native/include/battery_callback.h` | HDI 上报入口，HDI 接口变更会影响此处 |
| `services/native/include/battery_notify.h` | 事件分发核心，影响所有订阅电池事件的应用 |
| `services/native/include/battery_config.h` | 场景化电池配置，`SetBatteryConfig`/`GetBatteryConfig` 的服务端实现 |
| `charger/` | **独立子模块**，关机充电模式，有自己的 SA 入口和资源，与开机服务互斥运行 |
| `charger/include/dev/` + `charger/src/dev/` | 设备适配层，不同硬件的关机充电 UI 差异 |
| `frameworks/ets/taihe/` | taihe FFI **生成产物**，修改 `.taihe` 源文件后由构建系统重生成 |
| `batterymgr.gni` | 6 个特性开关 + 多个 part 检测（`has_hiviewdfx_hisysevent_part` / `has_sensors_miscdevice_part` / `has_drivers_interface_display_part` / `has_drivers_interface_light_part` / `has_graphic_surface_part` / `has_ui_lite_part` / `has_graphic_utils_lite_part`），开关变更需同步 `bundle.json` |
| `utils/native/` + `utils/hookmgr/` | 跨仓共享工具，修改影响 `power_manager` 等下游仓 |
| `batterymgr.yaml` | HiSysEvent 事件定义，新增事件需在此声明 |

### Vocabulary-based routing

当任务、issue、日志、API 名称中出现以下术语时，先理解其含义和风险再动手：

| 术语 | 含义与风险 | 读取 |
|---|---|---|
| BatterySrv | Battery Service，本服务简称，SA 3302 | 本文件 + `sa_profile/3302.json` |
| BatteryInfo | 电池信息数据模型（容量、电压、温度、健康、技术类型等），跨进程传输 | `interfaces/inner_api/native/include/battery_info.h` |
| PluggedType | 充电类型（USB/AC/WIRELESS/NONE），变更影响所有充电逻辑分支 | `battery_info.h` 中 `PlugType` 枚举 |
| BatteryCapacityLevel | 容量档位（CRITICAL/LOW/NORMAL/HIGH/FULL），变更影响 UI 提示与省电策略 | `battery_info.h` 中 `BatteryCapacityLevel` 枚举 |
| HealthStatus | 电池健康状态（UNKNOWN/GOOD/OVERHEAT/DEAD/OVERVOLTAGE/UNSPECIFIED_FAILURE/COLD），变更影响诊断 | `battery_info.h` 中 `BatteryHealthState` 枚举 |
| ChargingSound | 充电音效，特性开关默认关闭 | `services/native/include/charging_sound.h` + `batterymgr.gni` |
| Charger | 关机充电子模块，独立 SA 入口，与开机服务互斥 | `charger/` |
| HookMgr | 钩子管理器，允许在固定执行点插入预处理逻辑（跨仓共享） | `utils/hookmgr/` |
| HDIServiceStatusListener | HDI 上下线监听器，HDI 服务重启时需重新订阅 | `services/native/include/hdi_service_status_listener.h` |
| BatteryConfig | 场景化电池配置，支持按场景读写配置项 | `services/native/include/battery_config.h` + `IBatterySrv.idl` 中 `SetBatteryConfig`/`GetBatteryConfig` |
| CommonEvent | OpenHarmony 公共事件机制，电池事件通过 `usual.event.BATTERY_*` 广播 | `services/native/include/battery_notify.h` |
| IBatterySrv | IPC 接口（IDL 定义），含 17 个方法 | `services/zidl/IBatterySrv.idl` |
| IDL | 接口定义语言，本项目使用 `.idl` 文件生成 IPC proxy/stub | `services/zidl/IBatterySrv.idl` |
| SystemAbility / SA | OpenHarmony 系统服务框架，本服务 SA ID 3302 | `sa_profile/3302.json` |
| sptr | OpenHarmony 共享指针（`refbase.h`），非 `std::shared_ptr` | 全项目 |
| ErrCode | 错误码返回类型 | `interfaces/inner_api/native/include/battery_srv_errors.h` |
| HWTEST_F | OpenHarmony 测试用例宏 | 全项目测试代码 |
| 特性开关 | `batterymgr.gni` 中 `declare_args()` 控制条件编译 | `batterymgr.gni` |
| min_hdi_proxy_version | `sa_profile/3302.json` 中声明的 HDI 代理版本依赖（当前 `libbattery_proxy_2.0.z.so`），降级会破坏兼容性 | `sa_profile/3302.json` |
| powermgr 进程 | 本仓与 power_manager/thermal_manager/display_manager 共享同一进程，多 SA 共进程 | `sa_profile/3302.json` |
| 关机充电 | 关机状态下进入充电模式，UI 由 `charger/` 子模块负责 | `batterymgr.gni` 中 `battery_manager_feature_enable_charger` |

在计划阶段，必须声明：
- **任务分类**（如：公共 API 变更 / IPC 接口变更 / HDI 上报流程变更 / 关机充电模式变更 / 场景化配置变更 / 新增特性 / 测试修改）
- **已读取的代码路径和文档**（具体到文件路径）
- **发现的约束**（架构不变量、禁止事项、特性开关依赖、HDI 版本依赖、跨仓依赖）
- **是否需要同步修改其他层**：
  - 公共 API 变更 -> 同步 C / NAPI / CJ / taihe 四套绑定 + `*.map` 版本脚本
  - `IBatterySrv.idl` 变更 -> 由构建系统重生成 proxy/stub + 同步 `battery_srv_client.h` 调用方
  - 特性开关变更 -> 同步 `bundle.json` 的 `features` 列表
  - `utils/` 变更 -> 检查 `power_manager` 是否受影响
  - `batterymgr.yaml` 变更 -> 同步 HiSysEvent 事件定义

## 3. 约束边界

### 架构不变量

- **客户端不持有业务状态**：`BatterySrvClient` 仅做 IPC 转发，电池状态由 `BatteryService` 持有并从 HDI 上报刷新
- **HDI 是电池事件唯一真相源**：所有电池信息变更必须经 `BatteryCallback` 上报，禁止业务直接构造 `BatteryInfo`
- **公共 API 表达稳定能力意图，不暴露 HDI 句柄或服务端内部字段**
- **权限校验在服务端入口完成**：`SetBatteryConfig` 等写操作必须有权限校验，不能依赖客户端
- **关机充电模式与开机服务互斥**：`charger/` 子模块在关机充电场景下独立运行，不能依赖开机服务的能力
- **IPC 调用必须设置死亡通知**，防止客户端异常导致服务端泄露
- **跨进程回调必须用 Parcelable 序列化**，所有 `BatteryInfo` 跨进程传输必须实现 Marshalling
- **DFX（日志、HiSysEvent、HiCollie、错误码）必须观测所有电池状态变更和 HDI 调用**
- **HDI 上下线必须能自动重连**：`HDIServiceStatusListener` 监听 HDI 重启，自动重新订阅
- **`charger/` 子模块的代码必须能脱离开机服务独立运行**，不能引用 `services/native/` 中的开机服务类

### 禁止事项

- **不要修改公共 API 签名、错误码、权限行为或 `BatteryInfo` 字段语义**，除非任务明确要求；修改 `*.map` 中已有符号的可见性属于破坏性变更
- **不要直接编辑 `frameworks/ets/taihe/` 下的生成文件**，应修改 `.taihe` 源文件后由构建系统重生成
- **不要直接修改 `services/zidl/` 下 IDL 生成的 proxy/stub 代码**，应修改 `IBatterySrv.idl` 后重新生成
- **不要为通过测试而删除日志、HiSysEvent 事件、错误码或诊断信息**
- **不要绕过现有的 DFX、安全、兼容性检查**
- **不要直接修改 HDI 接口**：本仓是 HDI 消费方，HDI 接口由 `drivers_interface_battery` / `drivers_interface_light` 等仓维护
- **不要降级 `sa_profile/3302.json` 中的 `min_hdi_proxy_version`**：降级 HDI 版本会破坏与旧设备的兼容性
- **不要在 `BatteryCallback` 的 HDI 回调中执行阻塞 IO 或长耗时操作**：HDI 上报频率高，阻塞会导致事件丢失
- **不要在 `charger/` 子模块中引入对开机服务的依赖**：关机充电模式无法访问开机服务
- **不要引入新的生产依赖**而不经过 `bundle.json` 评审
- **不要在 `powermgr` 进程中执行长耗时同步操作**：本进程与 power/thermal/display 共享
- **不要绕过 `BatteryService` 直接调用 HDI 接口**：所有 HDI 访问必须经服务端封装

### 需确认后再修改

- **公共 API 签名变更**（需确认兼容性影响和版本策略，更新 `*.map` 版本脚本）
- **`BatteryInfo` 字段新增/删除**（需评估 NDK ABI 兼容性，C 接口消费者可能因结构体布局变化崩溃）
- **IPC 码值变更**（`IBatterySrv.idl` 中 `[ipccode N]` 码值是 ABI 契约，新增接口追加新码值，不能复用或调整已有码值）
- **`sa_profile/3302.json` 中 `min_hdi_proxy_version` 调整**（需确认 HDI 团队和设备兼容性）
- **`batterymgr.gni` 特性开关默认值翻转**（特别是 `battery_manager_feature_enable_charger` / `battery_manager_feature_enable_wireless_charge` / `battery_manager_feature_set_low_capacity_threshold`）
- **`BatteryInfo` 枚举值变更**（`PluggedType` / `BatteryCapacityLevel` / `BatteryHealthState` 等，影响所有消费者）
- **关机充电 UI 行为变更**（需确认 UX 团队评审，影响关机充电体验）
- **新增外部依赖**（需确认许可证、包大小、`bundle.json` 同步）
- **`utils/` 跨仓工具的接口变更**（需确认 `power_manager` 等下游仓是否受影响）
- **`batterymgr.yaml` 中 HiSysEvent 事件变更**（需确认 DFX 团队评审）

### 项目特定陷阱

- **`IBatterySrv.idl` 的 `[ipccode N]` 码值稳定性**：IDL 中每个方法的 `ipccode` 是 ABI 契约，新增接口必须追加新码值，复用或调整已有码值会让旧客户端调用错误接口
- **`BatteryInfo` 字段顺序**：跨进程传输的 Parcelable 字段顺序是 ABI 一部分，新增字段必须追加在末尾，不能插入中间
- **HDI 事件上报的时序保证**：`BatteryCallback` 收到 HDI 上报后会同步分发，订阅者期望按上报顺序收到事件，不要在分发中引入异步乱序
- **关机充电模式的资源隔离**：`charger/` 子模块的资源（`charger/resources/`）独立于开机服务，修改时不能交叉引用
- **`charger/include/dev/` 设备适配**：不同硬件的关机充电 UI 差异通过 `dev/` 子目录隔离，新增设备需新增对应 dev 目录
- **特性开关的 `defines` 双写**：`batterymgr.gni` 中开关既要改 `declare_args()` 默认值，也要在对应 `if` 块更新 `defines`，漏掉一处会导致特性开关失效
- **`powermgr` 多 SA 共进程**：SA 3301/3302/3303/3308 在同一进程，一个 SA 崩溃会拖死全部
- **HDI 重启时序**：HDI 服务重启后，`HDIServiceStatusListener` 必须重新订阅事件，否则电池上报中断
- **`SetBatteryConfig` 的场景化命名空间**：场景名（`sceneName`）是配置项命名空间，新增场景需在 `battery_config.h` 和 `services/native/profile/` 同步定义
- **`tools/ohos-batteryManager/` 是独立 npm 包**：CLI 命令变更需同步发布 npm 包版本

## 4. 验证闭环

### 最小验证

```bash
# 构建 battery_manager 子系统（从 OpenHarmony 根目录执行）
./build.sh --product-name rk3568 --build-target battery_manager

# 构建全部测试
./build.sh --product-name rk3568 --build-target battery_manager_test
```

### 任务特定验证

| 任务类型 | 验证命令 |
|---|---|
| 公共 API 变更 | `./build.sh --product-name rk3568 --build-target battery_manager` + 同步构建依赖本仓的下游仓（`battery_statistics` / `thermal_manager`） + 跑 `test/apitest/{capi,cj}/` |
| IPC 接口变更 | 重新生成 proxy/stub + 跑 `test/unittest/` + `test/fuzztest/getcapacity_fuzzer` 等所有 17 个 fuzzer |
| HDI 上报流程变更 | 跑 `test/systemtest/` HDI 上报场景 + 验证 `BatteryCallback` 重连逻辑 |
| 关机充电模式 | `./build.sh --build-target charger` + 跑 `charger/test/`（如存在） + 真机关机充电验证 |
| 场景化电池配置 | 跑 `test/fuzztest/{setbatteryconfig,getbatteryconfig,isbatteryconfigsupported}_fuzzer` |
| 充电音效 | 跑 `test/systemtest/` 充电场景 + 验证 `battery_manager_feature_enable_charging_sound` 开关生效 |
| 无线充电 | 验证 `battery_manager_feature_enable_wireless_charge` 开关生效 + 真机无线充电验证 |
| 低电量阈值 | 跑 `test/systemtest/` 低电量场景 + 验证 `battery_manager_feature_set_low_capacity_threshold` 开关生效 |
| 电池指示灯 | 真机验证不同电量下的指示灯颜色 + HDI `drivers_interface_light` 兼容性 |
| 特性开关翻转 | 重新构建 `battery_manager` 全量 + 验证 `bundle.json` 中 `features` 同步更新 |
| HDI 适配层变更 | 验证 `sa_profile/3302.json` 的 `min_hdi_proxy_version` 未被降级 |

### Done 定义

- 构建通过（子系统 + 单元测试 + 模糊测试 + 关机充电 + CLI 工具）
- 无新增编译警告
- 变更范围与任务要求一致，未夹带未关联的重构
- IPC 接口变更已同步 `IBatterySrv.idl` + 重生成 proxy/stub + 调用方
- 特性开关变更已同步 `batterymgr.gni` + `bundle.json`
- 公共 API 变更已同步 C / NAPI / CJ / taihe / `*.map` 版本脚本
- `BatteryInfo` 字段变更已评估 NDK ABI 兼容性
- 涉及关机充电的变更已评估独立运行能力（不依赖开机服务）

### 最终响应期望

完成报告必须包含：
1. 修改的文件清单（按 `file:line` 引用）
2. 任务分类与对应验证命令的执行结果
3. 是否触发跨层同步修改（C / NAPI / CJ / taihe / `*.map` / IDL / `bundle.json` / `utils/` / `batterymgr.yaml`）
4. 是否影响特性开关默认值或 HDI 版本依赖
5. 是否触及架构不变量或需确认事项
6. 涉及关机充电模式的变更需额外说明独立运行性验证

### 无法验证时

如果构建环境不可用，列出应执行的命令并说明预期结果，明确标注「未验证」字样，不能假称已通过。涉及 HDI 上报/关机充电/电池指示灯的变更，必须人工复核代码逻辑并说明无法在沙箱验证的限制。

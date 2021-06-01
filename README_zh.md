# 电池服务组件<a name="ZH-CN_TOPIC_0000001124094823"></a>

-   [简介](#section11660541593)
-   [目录](#section19472752217)
-   [相关仓](#section63151229062)

## 简介<a name="section11660541593"></a>

电池服务组件提供如下功能：

1.  电池信息查询。
2.  充放电状态查询。

**图 1**  电源管理子系统架构图<a name="fig106301571239"></a>  
![](figures/power-management-subsystem-architecture.png "电源管理子系统架构图")

## 目录<a name="section19472752217"></a>

```
base/powermgr/battery_manager
├── hdi                         # HDI层
│   ├── api                     # HDI接口
│   └── client                  # HDI客户端
│   └── service                 # HDI服务端
├── interfaces                  # 接口层
│   ├── innerkits               # 内部接口
│   └── kits                    # 外部接口
├── sa_profile                  # SA配置文件
└── services                    # 服务层
│   ├── native                  # native层
│   └── zidl                    # zidl接口层
└── utils                       # 工具和通用层
```



## 相关仓<a name="section63151229062"></a>

电源管理子系统

**powermgr_battery_manager**

powermgr_power_manager

powermgr_display_manager

# battery\_manager<a name="EN-US_TOPIC_0000001124094823"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section19472752217)
-   [Repositories Involved](#section63151229062)

## Introduction<a name="section11660541593"></a>

Battery Manager provides the following functionalities:

1.  Obtaining battery information
2.  Obtaining the battery charging status and the remaining battery power
3.  Power off charging

**Figure  1**  Battery manager subsystem architecture<a name="fig106301571239"></a> 
![](figures/power-management-subsystem-architecture.png "power-management-subsystem-architecture")

## Directory Structure<a name="section19472752217"></a>

```
base/powermgr/battery_manager
├── figures                  	# Architecture figures
├── frameworks                  # FrameWork Layer
│   ├── dialog                  # Dialog
│   ├── js                  	# NAPI code
│   └── native                  # Native code
├── interfaces                  # APIs
│   ├── innerkits               # Internal APIs
│   └── kits                    # External APIs
├── sa_profile                  # SA profile
├── services                    # Services
│   ├── native                  # Native services
│   └── zidl                    # Zidl APIs
├── test                        # Test code
│   ├── fuzztest                # Fuzz test
│   └── systemtest              # System test
└── utils                       # Utilities and common code
```

## Repositories Involved<a name="section63151229062"></a>

[Power management subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/power-management.md)

[powermgr_power_manager](https://gitee.com/openharmony/powermgr_power_manager)

[powermgr_display_manager](https://gitee.com/openharmony/powermgr_display_manager)

**powermgr_battery_manager**

[powermgr_thermal_manager](https://gitee.com/openharmony/powermgr_thermal_manager)

[powermgr_battery_statistics](https://gitee.com/openharmony/powermgr_battery_statistics)

[powermgr_battery_lite](https://gitee.com/openharmony/powermgr_battery_lite)

[powermgr_powermgr_lite](https://gitee.com/openharmony/powermgr_powermgr_lite)

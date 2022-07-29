# Niobe407开发板OpenHarmony基于HDF驱动框架编程开发——Watchdog
本示例将演示如何在Niobe407开发板上通过HDF驱动框架使用看门狗。

## 接口说明

### WatchdogOpen()

```
int32_t WatchdogOpen(int16_t wdtId,  DevHandle *handle)
```

**描述：**

watchdog Open打开看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| wdtId  | 看门狗ID |
| handle | 设备句柄 |

### WatchdogClose()

```
void WatchdogClose(DevHandle handle)
```

**描述：**

watchdog close关闭看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogFeed()

```
int32_t WatchdogFeed(DevHandle handle)
```

**描述：**

watchdog feed喂狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogGetStatus()

```
int32_t WatchdogGetStatus(DevHandle handle, int32_t *status)
```

**描述：**

watchdog getStatus获取看门狗状态函数。

**参数：**

| 名字   | 描述           |
| :----- | :------------- |
| handle | 设备句柄       |
| status | 看门狗状态指针 |

### WatchdogStart()

```
int32_t WatchdogStart(DevHandle handle)
```

**描述：**

watchdog start启动看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### watchdogStop()

```
int32_t WatchdogStop(DevHandle handle)
```

**描述：**

watchdog stop停止看门狗函数。

**参数：**

| 名字   | 描述     |
| :----- | :------- |
| handle | 设备句柄 |

### WatchdogSetTimeout()

```
int32_t WatchdogSetTimeout(DevHandle handle, uint32_t seconds)
```

**描述：**

watchdog setTimeout设置看门狗超时时间函数。

**参数：**

| 名字    | 描述     |
| :------ | :------- |
| handle  | 设备句柄 |
| seconds | 超时时间 |

### WatchdogGetTimeout()

```
int32_t WatchdogGetTimeout(DevHandle handle, uint32_t *seconds)
```

**描述：**

watchdog getTimeout获取看门狗超时时间函数。

**参数：**

| 名字    | 描述                   |
| :------ | :--------------------- |
| handle  | 设备句柄               |
| seconds | 超时时间指针，单位为秒 |


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `205_hdf_watchdog`

- 在menuconfig的`(Top) → Driver`选项中使能如下配置:

```
    [*] Enable Driver
    [*]     HDF driver framework support
    [*]         Enable HDF platform driver
    [*]             Enable HDF platform watchdog driver
```
- 回到sdk根目录，执行`hb build -f`脚本进行编译。

## 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手可查看到如下日志：
```
[HDF:E/HDF_LOG_TAG]HdfWatchdogTestEntry ----------

[HDF:E/HDF_LOG_TAG]WatchdogGetStatus: 0

[HDF:E/HDF_LOG_TAG]WatchdogGetTimeout: 500

[HDF:E/HDF_LOG_TAG]WatchdogSetTimeout: 1200

[HDF:E/HDF_LOG_TAG]WatchdogGetTimeout: 1200

[HDF:I/HDF_LOG_TAG]WatchdogDevStart: watchdog Started! timeout: 1200ms

[HDF:E/HDF_LOG_TAG]---> WatchdogStart.

[HDF:E/HDF_LOG_TAG]WatchdogGetStatus: 1

[HDF:E/HDF_LOG_TAG]--> WatchdogFeed success!
[HDF:E/HDF_LOG_TAG]--> WatchdogFeed success!
[HDF:E/HDF_LOG_TAG]--> WatchdogFeed success!
[HDF:E/HDF_LOG_TAG]--> WatchdogFeed success!
```
## HDF HCS配置
开发板使用的默认配置(`//device/board/talkweb/niobe407/liteos_m/hdf_config/default.hcs`)中，默认没有配置Watchdog，需要同级目录下的`BUILD.gn`改用`hdf.hcs`文件, 或参考该文件添加watchdog配置：

- 在`default_device_info.hcs`中添加如下配置：
```
root {
    module = "talkweb,stm32f407";
    device_info {
        match_attr = "hdf_manager";
        template host {
            hostName = "";
            priority = 100;
            template device {
                template deviceNode {
                    policy = 0;
                    priority = 100;
                    preload = 0;
                    permission = 0664;
                    moduleName = "";
                    serviceName = "";
                    deviceMatchAttr = "";
                }
            }
        }
        platform :: host {
            hostName = "platform_host";
            priority = 50;
            
            device_watchdog :: device {
                watchdog0 :: deviceNode {
                    policy = 1;             
                    priority = 20;          
                    permission = 0644; 
                    moduleName = "ST_WATCHDOG_MODULE_HDF";
                    serviceName = "HDF_PLATFORM_WATCHDOG_0";
                    deviceMatchAttr = "st_watchdog";
                }
            }
        }
    }
}
```

- 在`default.hcs`中添加如下配置：
```
#include "default_device_info.hcs"
root {
    platform {
        watchdog_config {
	        template watchdog_controller {
	            id = 0;
	            regBase = 0x12050000;
	            regStep = 0x1000;
	            timeout = 1000; // watchdog interval(ms)
	            match_attr = "";
	        }
	        st_watchdog :: watchdog_controller {
	            match_attr = "st_watchdog";
	            timeout = 500;
	        }
        }
    }
}
```

## 约束

STM32F407的看门狗是硬件独立看门狗，不支持关闭功能，使用时需注意看门狗的应用场景。
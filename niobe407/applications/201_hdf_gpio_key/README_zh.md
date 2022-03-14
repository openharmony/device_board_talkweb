# Niobe407开发板OpenHarmony基于HDF驱动框架编程开发——GPIO
本示例将演示如何在Niobe407开发板上通过HDF驱动框架，使用GPIO按键中断。


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `201_hdf_gpio_key`

- 在menuconfig的`(Top) → Driver`选项中使能如下配置:

```
    [*] Enable Driver
    [*]     HDF driver framework support
    [*]         Enable HDF platform driver
    [*]             Enable HDF platform gpio driver
```
- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
Entering scheduler
[HDF:E/HDF_LOG_TAG]DeviceManagerStart in
[HDF:I/devmgr_service]start svcmgr result 0
[HDF:I/hcs_blob_if]CheckHcsBlobLength: the blobLength: 1884, byteAlign: 1
[HDF:I/device_node]launch devnode HDF_PLATFORM_GPIO
GpioDriverBind 
[HDF:I/gpio_manager]GpioManagerAdd: start:0 count:8 added success
[HDF:D/HDF_LOG_TAG]PlatformManagerAddDevice: add dev:(null)(0) to PLATFORM_MODULE_GPIO success
[HDF:D/HDF_LOG_TAG]PlatformDeviceAdd: add dev:(null)(0) success
[HDF:E/HDF_LOG_TAG]HdfDriverManagerGetDriver:driver STM_TW_SPI_MODULE_HDF not found
[HDF:E/devhost_service_clnt]failed to install driver HDF_PLATFORM_SPI_0, ret = -207
hiview init success.
[HDF:E/HDF_LOG_TAG]HdfGpioTestEntry: mode:2
```

当按下开发板上的按钮sw2时，点亮或者点灭， 按下开发板上按钮sw3时， 绿灯点亮或者点灭 ,可通过串口助手查看到如下日志:
```
[HDF:E/HDF_LOG_TAG]TestCaseGpioIrqHandler1: irq triggered! on gpio:2, data=(nil)
[HDF:E/gpio_stm32_c]GpioDevWrite 433 ,write pin num 5
[HDF:E/HDF_LOG_TAG]TestCaseGpioIrqHandler1: irq triggered! on gpio:2, data=(nil)
[HDF:E/gpio_stm32_c]GpioDevWrite 433 ,write pin num 5
[HDF:E/HDF_LOG_TAG]TestCaseGpioIrqHandler2: irq triggered! on gpio:3, data=(nil)
[HDF:E/gpio_stm32_c]GpioDevWrite 433 ,write pin num 6
```
## GPIO HDF hcs配置文件解析
- device_gpio_info 在/device/board/talkweb/niobe407/sdk/hdf_config/device_gpio_info.hcs
```
root {
    module = "talkweb,stm32f407";
    device_info {
        match_attr = "hdf_manager";
        template host {
            hostName = "";
            priority = 100;
            template device {
                template deviceNode { //gpio信息的模板，集成模板可以缺省
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
            device_gpio :: device {
                gpio0 :: deviceNode {
                    policy = 2;
                    priority = 60; //权限决定加载的顺序
                    moduleName = "STM_TW_GPIO_MODULE_HDF";
                    serviceName = "HDF_PLATFORM_GPIO";//驱动名称，该字段的值必须和驱动入口结构的moduleName值一致
                    deviceMatchAttr = "gpio_config"; // 与hdf_gpio_.hcs中的config匹配
                }
            }
        }
    }
}
```
- hdf_gpio.hcs解析 在/device/board/talkweb/niobe407/sdk/hdf_config/目录下，配置具体gpio引脚的信息
```
#include "device_gpio_info.hcs"      // 包含具体的device_info文件
root {
    platform {
        gpio_config {
            match_attr = "gpio_config"; // 与device_gpio_info.hcs中的deviceMatchAttr对应
            pin = [0, 1, 2, 3]; // 框架注册pin号，在使用接口时用作GPIO号来使用
            realPin = [5, 6, 0, 1]; // 与pin对应的真实的在stm32中真实的引脚编号（0-15）开发板，此示例为 led2 pe5, led3 pe6, key1按键终端 pe0,sw2, key2按键终端pe1,sw3
            group = [4, 4, 4, 4]; // gpio引脚所属的group 0到8 分别对应GPIOA~GPIOI,此例4个gpio属于GPIOE
			mode = [1, 1, 0, 0];  // 0: input 1: output 2:alternate 3:analog  
            speed = [0, 0, 0, 0]; // 0: low 1: middle 2:high 3:very_high
            pull = [0, 0, 0, 0]; // 0: nopull 1:up 2:down
            pinNum = 4; //总注册gpio个数 如果发现因为加载的gpio个数过多导致无法申请到锁资源，可修改target_config.h中的LOSCFG_BASE_IPC_MUX_LIMIT 增大锁的数量限制
            output = [0, 0, 0, 0];  // 0:pushpull 1:opendrain
            alternate = [0, 0, 0, 0]; // 对应的stm32的管脚复用功能gpio作为每种外设都对应一种复用功能（0-15，具体参考编程手册）
        }
    }
}
```
## 接口说明

```
 1.读写GPIO管脚， 如果要读取一个GPIO管脚电平，通过以下函数完成：
 int32_t GpioRead(uint16_t gpio, uint16_t *val);
 参数说明: 
            gpio: 对应hcs文件中的pin号
            val:  存储读取的值
 返回值：
            0：   读取成功
            负数：读取失败

2.如果要向GPIO管脚写入电平值，通过以下函数完成：
int32_t GpioWrite(uint16_t gpio, uint16_t val);
参数说明: 
            gpio: 对应hcs文件中的pin号
            val:  需要设置的电平值
返回值：
            0：   设置成功
            负数：设置失败
3.如果要为一个GPIO管脚设置中断响应程序，使用如下函数：
int32_t GpioSetIrq(uint16_t gpio, uint16_t mode, GpioIrqFunc func, void *arg);
参数说明: 
            gpio: 对应hcs文件中的pin号
            mode: 中断触发模式
            func: 中断回调函数
            arg:  回调函数参数 
返回值：
            0：   设置成功
            负数：设置失败
4.当不再需要响应中断服务函数时，使用如下函数取消中断设置：
int32_t GpioUnSetIrq(uint16_t gpio);
参数说明: 
            gpio: 对应hcs文件中的pin号
返回值：
            0：   设置成功
            负数：设置失败
5.在中断服务程序设置完成后，还需要先通过如下函数使能GPIO管脚的中断：
int32_t GpioEnableIrq(uint16_t gpio);
参数说明: 
            gpio: 对应hcs文件中的pin号
返回值：
            0：   使能成功
            负数： 使能失败
6.如果要临时屏蔽此中断，可以通过如下函数禁止GPIO管脚中断
int32_t GpioDisableIrq(uint16_t gpio);       
参数说明: 
            gpio: 对应hcs文件中的pin号
返回值：
            0：   禁止成功
            负数： 禁止失败
```
## 示例代码
``` c
/* 中断服务函数*/
static int32_t TestCaseGpioIrqHandler1(uint16_t gpio, void *data) // pin2 回调函数
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(0, &val); // 读取pin0 的电平值，中断产生则设置相反的电平
    if (val) {
        GpioWrite(0, 0);
    } else {
        GpioWrite(0, 1);
    }
}

static int32_t TestCaseGpioIrqHandler2(uint16_t gpio, void *data) // pin3 回调函数
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(1, &val); // 读取pin1 的电平值，中断产生则设置相反的电平
    if (val) {
        GpioWrite(1, 0);
    } else {
        GpioWrite(1, 1);
    }
}


static void* HdfGpioTestEntry(void* arg)
{
    int32_t ret;
    uint16_t valRead;
    uint16_t mode;
    uint16_t gpio0 = 0; // hcs 中对应的pin值
    uint16_t gpio1 = 1;
    uint16_t gpio2 = 2;
    uint16_t gpio3 = 3;
    uint32_t timeout;
    mode = OSAL_IRQF_TRIGGER_FALLING;
    HDF_LOGE("%s: mode:%0x\n", __func__, mode);
    ret = GpioSetIrq(gpio2, mode, TestCaseGpioIrqHandler1, NULL); // 设置中断函数
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return ret;
    }

    ret = GpioSetIrq(gpio3, mode, TestCaseGpioIrqHandler2, NULL); // 设置中断函数
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return ret;
    }

    ret = GpioEnableIrq(gpio2); // 使能中断
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return ret;
    }

    ret = GpioEnableIrq(gpio3); // 使能中断
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return ret;
    }
}
```

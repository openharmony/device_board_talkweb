# Niobe407开发板OpenHarmony基于HDF驱动框架编程开发——UART
本示例将演示如何在Niobe407开发板上通过HDF驱动框架，使用UART接口对USART2进行读写操作。


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `202_hdf_spi_flash`

- 在menuconfig的`(Top) → Driver`选项中使能如下配置:

```
    [*] Enable Driver
    [*]     HDF driver framework support
    [*]         Enable HDF platform driver
    [*]             Enable HDF platform gpio driver
    [*]             Enable HDF platform uart driver
```
- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
[HDF:I/uartDev]InitUartDevice: Enter
[HDF:E/uartDev]uart 2 device init
[HDF:I/HDF_LOG_TAG]UartSetAttribute: success
[HDF:I/uartDev]uart write block 0x40004400
[HDF:I/HDF_LOG_TAG]UartWrite: success, ret 0
```
用串口线连接串口4，并打开串口4，输入welcome to openharmony，即可在调试串口看到日志
```
[HDF:I/HDF_LOG_TAG]UartRead byte 23 content is welcome to openharmony
[HDF:I/uartDev]UartHostDevDeinit: Enter
[HDF:I/uartDev]UartHostDevDeinit: Enter

```
# OpenHarmony驱动子系统开发—UART收发测试

## 概述
    1.UART是通用异步收发传输器（Universal Asynchronous Receiver/Transmitter）的缩写，是通用串行数据总线，用于异步通信。该总线双向通信，可以实现全双工传输。

    2.UART与其他模块一般用2线或4线相连:
    当使用2线相连时，它们分别是：
        TX：发送数据端，和对端的RX相连；
        RX：接收数据端，和对端的TX相连；

    当使用4线相连时，它们分别是：
        TX：发送数据端，和对端的RX相连；
        RX：接收数据端，和对端的TX相连；    
        RTS：发送请求信号，用于指示本设备是否准备好，可接受数据，和对端CTS相连；
        CTS：允许发送信号，用于判断是否可以向对端发送数据，和对端RTS相连；

    3.UART通信之前，收发双方需要约定好一些参数：波特率、数据格式（起始位、数据位、校验位、停止位）等。通信过程中，UART通过TX发送给对端数据，通过RX接收对端发送的数据。

## 接口说明
    1. uart open初始化函数:DevHandle UartOpen(uint32_t port);
        参数说明: 
            port:     对应的uart号，ESP32U4芯片有三组uart，分别为0、1、2
            return:  不为NULL,表示初始化成功
    2. uart设置波特率函数：int32_t UartSetBaud(DevHandle handle, uint32_t baudRate);
        参数说明：
            handle：   UART设备句柄，
            baudRate:  待设置的波特率
            return:    0:UART设置波特率成功,负数表示设置波特率失败
    2. uart读数据函数:int32_t UartRead(DevHandle handle, uint8_t *data, uint32_t size);
        参数说明: 
            handle:  指向uart设备句柄的指针
            data:    uart读取数据后的缓存地址
            size：   uart本次期望读取的数据长度
            return:  返回负数表示失败

    3. uart写数据函数:int32_t UartWrite(DevHandle handle, uint8_t *data, uint32_t size);
        参数说明: 
            handle:  指向uart设备句柄的指针
            data:    uart待发送的数据
            size：   uart本次期望发送的数据长度
            return:  返回0，表示成功，返回负数，表示失败

    4. uart去初始化函数:void UartClose(DevHandle handle);
        参数说明: 
            handle:  指向uart设备句柄的指针

## UART HDF HCS配置文件解析
- device_uart_info.hcs文件位于/device/board/talkweb/niobe407/sdk/hdf_config/device_uart_info.hcs,本例子使用的是UART4串口
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
            device_gpio :: device {
                gpio2 :: deviceNode { // uart配置的gpio引脚信息
                    policy = 2;
                    priority = 60;
                    moduleName = "STM_TW_GPIO_MODULE_HDF";
                    serviceName = "HDF_PLATFORM_GPIO2";
                    deviceMatchAttr = "gpio_config_uart";
                }         
            }
            device_uart :: device {
				uart4 :: deviceNode { // uart4配置信息
                    policy = 2;//驱动服务发布的策略，policy大于等于1（用户态可见为2，仅内核态可见为1）；
                    priority = 100;//驱动启动优先级
                    moduleName = "STM_TW_UART_MODULE_HDF";//驱动名称，该字段的值必须和驱动入口结构的moduleName值一致
                    serviceName = "HDF_PLATFORM_UART_4"; //驱动对外发布服务的名称，必须唯一，必须要按照HDF_PLATFORM_UART_X的格式，X为UART控制器编号
                    deviceMatchAttr = "uart_config4";
				}
            }
            device_uart1 :: device {
				uart5 :: deviceNode { // uart5配置信息，为485接口
                    policy = 2;
                    priority = 100;
                    moduleName = "STM_TW_UART_MODULE_HDF";
                    serviceName = "HDF_PLATFORM_UART_5";
                    deviceMatchAttr = "uart_config5";
				}
            }
        }
    }
}
```

- hdf_uart.hcs文件位于/device/board/talkweb/niobe407/sdk/hdf_config/hdf_uart.hcs,在此文件中配置串口对应的GPIO引脚信息，串口配置信息
```
#include "device_uart_info.hcs"
root {
    platform {
        gpio_config2 {
            match_attr = "gpio_config_uart";
            pin = [0, 1, 2, 3, 4];   // pin index when register to hdf framework
            realPin = [10, 11, 12, 2, 12];       // uart4 tx pc10, rx pc11  uart5 tx:pc12 rx:pd2 de:pg12 此处将uart4配置为232， 将uart5配置为485
            group = [2, 2, 2, 3, 6];          // group of gpio 0:GPIOA 1:GPIOB 2:GPIOC 3:GPIOD 4:GPIOE 5: GPIOF 6:GPIOG 7:GPIOH 8:GPIOI
			mode = [2, 2, 2, 2, 1];           // 0: input 1: output 2:alternate 3:analog  
            speed = [3, 3, 3, 3, 3];          // 0: low 1: middle 2:high 3:very_high
            pull = [1, 1, 1, 1, 2];           // 0: nopull 1:up 2:down
            pinNum = 5;
            output = [0, 0, 0, 0, 0];         // 0:pushpull 1:opendrain
            alternate = [8, 8, 8, 8, 0];      // uart4,5 复用AF_8 USART1-3 复用AF_7
        }
        uart_config4 {
            match_attr = "uart_config4";
            num = 4;     // 1 :uart1 2: USART2 3:USART3 4:UART4 5:UART5 6:USART6 串口总线号此处为uart4
            baudRate = 115200; // baudrate
            dataWidth =  0; // 0:8bit 1:9bit
            stopBit = 1; // 0:0.5stop bits  1:1 stop bit 2:1.5 stop bits 2:2 stop bits
            parity = 0;   // 0: none 1: event 2:odd
            transDir = 0; // 0: dir none  1: rx  2: tx 3:tx and rx
            flowCtrl = 0; // 0: no flowcontrl  1: flowContorl RTS  2: flowControl CTS 3: flowControl RTS AND CTS
            overSimpling = 0; // 0: overSimpling 16bits  1: overSimpling 8bits
            idleIrq = 1;   // 0: disable idle irq  1: enable idle irq
            transMode = 0; /// 0:block 1:noblock 2:TX DMA RX NORMAL  3:TX NORMAL  RX DMA 4: USART_TRANS_TX_RX_DMA 目前还不支持DMA传输方式，只支持0,1阻塞读写方式和非阻塞读写方式
            阻塞读写是指，写时等可写并且写完才返回，阻塞读是指读到设定读取个数或者产生闲时中断才返回，非阻塞读写是指不管串口可读或者可写，直接调用读写接口进行读写，不一定能正确读写成功，一般建议阻塞读写
            maxBufSize = 1024;
            uartType = 0; // 0 : 232 1: 485
            uartDePin = 0;  // usart 485 pin
            uartDeGroup = 0; // usart 485 control line
        }
        uart_config5 {
            match_attr = "uart_config5";
            num = 5;     // 1 :uart1 2: USART2 3:USART3 4:UART4 5:UART5 6:USART6 串口总线号此处为uart5 485
            baudRate = 115200; // baudrate
            dataWidth =  0; // 0:8bit 1:9bit
            stopBit = 1; // 0:0.5stop bits  1:1 stop bit 2:1.5 stop bits 2:2 stop bits
            parity = 0;   // 0: none 1: event 2:odd
            transDir = 3; // 0: dir none  1: rx  2: tx 3:tx and rx
            flowCtrl = 0; // 0: no flowcontrl  1: flowContorl RTS  2: flowControl CTS 3: flowControl RTS AND CTS
            overSimpling = 0; // 0: overSimpling 16bits  1: overSimpling 8bits
            idleIrq = 1;   // 0: disable idle irq  1: enable idle irq
            transMode = 0; // 0:block 1:noblock 2:TX DMA RX NORMAL  3:TX NORMAL  RX DMA 4: USART_TRANS_TX_RX_DMA 目前还不支持DMA传输方式，只支持0,1阻塞读写方式和非阻塞读写方式
            阻塞读写是指，写时等可写并且写完才返回，阻塞读是指读到设定读取个数或者产生闲时中断才返回，非阻塞读写是指不管串口可读或者可写，直接调用读写接口进行读写，不一定能正确读写成功，一般建议阻塞读写
            uartType = 1; // 0 : 232 1: 485
            uartDePin = 12;  // uart5 485 pin
            uartDeGroup = 6; // uart5 485 control line group
        }
    }
}

```

## 例程原理简介
    NIOBE407开发板有USART1,USART2,USART3,UART4,UART5,USART6总共6路串口，本例程用的是USART2,和USART3，USART2为RS232, USART3为RS485。如需修改UART的硬件配置，可前往device\board\talkweb\niobe407\sdk\hdf_config\hdf_uart.hcs中根据您的开发板原理图进行修改
``` c
static void* HdfUsartTestEntry(void* arg)
{
    DevHandle handle = NULL;    /* UART设备句柄  */
    uint32_t port = 2;                  /* UART设备端口号 */
    handle = UartOpen(port);
    if (handle == NULL) {
        HDF_LOGE("UartOpen: failed!\n");
        return NULL;
    }

    int32_t ret;
    uint32_t baudRate;
    /* 获取UART波特率 */
    ret = UartGetBaud(handle, &baudRate);
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartGetBaud: success, badurate %d\n", baudRate);
    }

    baudRate = 115200;
    ret = UartSetBaud(handle, baudRate); // 设置uart波特率
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartSetBaud: success, badurate %d\n", baudRate);
    }

    struct UartAttribute attribute = {0};

    ret = UartGetAttribute(handle, &attribute); // 获取有hcs文件设置的串口配置信息
    if (ret != 0) {
        HDF_LOGE("UartGetAttribute: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartGetAttribute: success\n");
    }

    attribute.dataBits = UART_ATTR_DATABIT_8; // 8位停止位
    attribute.fifoTxEn = 1;                   // tx使能
    attribute.fifoRxEn = 1;                   // rx使能
    attribute.parity = UART_ATTR_PARITY_NONE; // 无校验
    attribute.stopBits = UART_ATTR_STOPBIT_1; // 1位停止位
    attribute.cts = 0;                        // 无流控
    attribute.rts = 0;                        // 无流控

    ret = UartSetAttribute(handle, &attribute); // 设置uart配置
    if (ret != 0) {
        HDF_LOGE("UartSetAttribute: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartSetAttribute: success\n");
    }

    len = strlen((char *)txbuf);
    ret = UartWrite(handle, txbuf, len); // 写串口
    if (ret != 0) {
        HDF_LOGE("UartWrite: failed, ret %d\n", ret);
        goto _ERR;
    } else {
        HDF_LOGI("UartWrite: success, ret %d\n", ret);
    }

    ret = UartRead(handle, rxbuf, len); // 读串口输入
    if (ret < 0) {
        HDF_LOGE("UartRead: failed, ret %d\n", ret);
        goto _ERR;
    } else {
        HDF_LOGI("UartRead byte %d content is %s\n", ret, rxbuf);
    }

    UartClose(handle);
_ERR:
    /* 销毁UART设备句柄 */
    UartClose(handle); 

}
    ```
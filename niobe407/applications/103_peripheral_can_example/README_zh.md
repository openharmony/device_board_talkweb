# Niobe407开发板OpenHarmony外设编程开发——CAN
CAN是控制器局域网络(Controller Area Network, CAN)的简称，是由以研发和生产汽车电子产品著称的德国BOSCH公司开发的，并最终成为国际标准（ISO 11898），是国际上应用最广泛的现场总线之一。 在北美和西欧，CAN总线协议已经成为汽车计算机控制系统和嵌入式工业控制局域网的标准总线，并且拥有以CAN为底层协议专为大型货车和重工机械车辆设计的J1939协议。

本示例将演示如何在Niobe407开发板上通过CAN接口收发数据。


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `103_peripheral_can_example`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

- 准备两块开发板，将固件烧录进去，然后将两块开发板的CAN口对接。（有条件可以直接通过USB转CAN接到电脑上，然后通过上位机调试CAN数据收发）

- 若测试时若无法收发数据，建议检查CAN口是否有终端电阻(使用万用表测量CAN_L与CAN_H直接电阻是否约为120Ω)

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
HAL_CAN_Start success!
send can data [0x1234 : 88 77 66 55 44 33 22 11] success! Tx_Mail:1
Recv CANID:0x00000000 Data:88 77 66 55 44 33 22 11 
send can data [0x1234 : 88 77 66 55 44 33 22 11] success! Tx_Mail:1
Recv CANID:0x00000000 Data:88 77 66 55 44 33 22 11 
send can data [0x1234 : 88 77 66 55 44 33 22 11] success! Tx_Mail:1
Recv CANID:0x00000000 Data:88 77 66 55 44 33 22 11 
```
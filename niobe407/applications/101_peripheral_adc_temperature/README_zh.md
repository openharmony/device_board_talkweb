# Niobe407开发板OpenHarmony外设编程开发——ADC
本示例将演示如何在Niobe407开发板上通过ADC采集CPU温度


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `101_peripheral_adc_temperature`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
The IC current temperature is 24.25
The IC current temperature is 22.96
The IC current temperature is 22.64
The IC current temperature is 23.29
The IC current temperature is 23.29
The IC current temperature is 23.61
The IC current temperature is 23.61
The IC current temperature is 22.96
```

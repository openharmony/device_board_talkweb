# Niobe407内存优化-CCMRAM内存使用示例
由于STM32F407芯片的内存是两块不连续的内存，一块RAM大小为128KB,另一块RAM大小为64KB，但是系统当前默认只用到了128KB那块内存。在niobe407程序开发过程中，难免会遇到内存不够的情况，我们可以将比较大的buffer定义道ccmram内存中去，留出更大的动态内存空间，本示例用于演示如何在niobe407上使用CCRAM内存。

## 注意事项
   ccmram内存不能被DMA访问。

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `501_optimization_ccmram_use`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

## 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，显示内容如下则表示执行成功:
```
---use CCMRAM buff success!
```

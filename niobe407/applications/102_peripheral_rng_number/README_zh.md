# Niobe407开发板OpenHarmony外设编程开发——RNG
随机数发生器（RNG）是一个以连续模拟噪声为基础的随机数发生器，在主机读数时提供一个 32 位的随机数。
本示例将演示如何在Niobe407开发板上通过rng获取随机数。


## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `102_peripheral_rng_number`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
Generate 32bit Random number is 3939425217
Generate 32bit Random number is 65643907
Generate 32bit Random number is 1996657443
Generate 32bit Random number is 105432700
Generate 32bit Random number is 3489997754
Generate 32bit Random number is 4219107597
Generate 32bit Random number is 2745823547
Generate 32bit Random number is 1101818015
Generate 32bit Random number is 2122724804
Generate 32bit Random number is 2652032502
Generate 32bit Random number is 2661532219
Generate 32bit Random number is 3839864595
Generate 32bit Random number is 2992609050
Generate 32bit Random number is 2483297994
Generate 32bit Random number is 2550058723
```
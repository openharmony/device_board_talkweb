# Key-Value存储示例
在程序开发过程中，经常会以键值对的方式进行参数存储，本示例用于演示如何在niobe407上存储和读取键值对参数。

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `401_file_kv_store`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

## 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志，显示内容如下则表示执行成功:
```
littlefs mount on spiflash 0x800000[ /talkweb ] successfully!
hiview init success.
[wifi manage] set key = talkweb0, value = 12345678
[wifi manage] set key = talkweb1, value = 3456789
[wifi manage] get talkweb0 passwd = 12345678
[wifi manage] get talkweb1 passwd = 3456789
```

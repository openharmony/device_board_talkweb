# Niobe407开发板OpenHarmony内核编程开发——atomic
本示例将演示如何在Niobe407开发板上使用原子操作

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `008_system_atomic`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，通过串口助手查看日志
```
###start atomic test1
####start atomic test1
###end atomic test1
####start atomic test2
###end atomic test2 
####start atomic test3
###end atomic test3 
####start atomic test4
###end atomic test4 
### start atomic test5
###end atomic test5 
### start atomic test6
###end atomic test6 
### start atomic test7
### end atomic test7
### start atomic test8
### end atomic test8
### start atomic test9
max task is 25 free thread num is 21
### end of atomic test9
### start of atomic test10
### end of atomic test10
### start of atomic test11
### end of atomic test11
```




# device_board_talkweb

## 简介
该仓用于放置talkweb开发板相关内容，各开发板的安装教程和使用说明可点击下表开发板名称查看。

|开发板名称|SoC型号|应用领域|设备互联类型|
|:-:|:-:|:-:|:-:|
|[Niobe407](niobe407/README_zh.md)|`STM32F407IGT6`|交通、工业|有线以太网|

## 目录结构

```
device/board/talkweb
├── niobe407                              # 开发板名称
    ├── applications                      # 示例程序及应用程序开发目录
    ├── docs                              # 文档目录，用于存放开发板相关文档
    ├── liteos_m                          # LiteOS SDK目录
        ├── bootloader                    # 启动引导固件目录
        ├── bsp                           # 板级支持目录
        ├── drivers                       # 驱动目录
        ├── fs                            # 文件系统目录
        ├── hdf_config                    # hdf配置目录
        ├── lwip_adapter                  # lwip适配目录
        ├── tools                         # 工具目录
    ├── pack_tools                        # 工具目录，用于对生成的程序自动进行打包签名等处理

├── xxx                                   # 其它板型，持续开发中...                    
```

## 使用说明

Niobe407参考：

- [Niobe407](niobe407/README_zh.md)

## 相关仓

- [ vendor_talkweb](https://gitee.com/openharmony/vendor_talkweb)
- [device_soc_st](https://gitee.com/openharmony/device_soc_st)
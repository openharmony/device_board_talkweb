# HDF GPIO配置通用文件使用说明

  此文件为GPIO配置通用文件。

  当需要初始化GPIO时，再HDF驱动函数里直接调用 NiobeHdfGpioInit()函数即可。

# 使用步骤
  
  以I2C驱动为例:

    第一步: 根据hdf_base_hal.h中 NIOBE_HDF_GPIO_ATTR结构体规范，填写GPIO配置参数到hdf_i2c.hcs中, 如下:

        gpio{
            // 要配置的引脚个数，接下来的引脚名必须定义成gpio_num_1, gpio_num_2, gpio_num_3...
            gpio_num_max = 3; 

            // port, pin, mode, speed, outputType, pull, alternate
            gpio_num_1 = [7, 9, 1, 2, 0, 0, 0];
            gpio_num_2 = [4, 5, 1, 2, 0, 0, 0]; 
            gpio_num_3 = [4, 6, 1, 2, 0, 0, 0];                   
        }

    第二步: 在获取hcs dir属性后，调用NiobeHdfGpioInit()函数，初始化GPIO配置，I2C驱动具体位置如下，其它驱动自行参考:

    struct DeviceResourceIface *dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE); 

    if(NiobeHdfGpioInit(resourceNode, dri) == false){ //在这里调用GPIO初始化函数
        HDF_LOGE("i2c config NiobeHdfGpioInit fail\r\n");
        return HDF_FAILURE;        
    }


/*
 * Copyright (c) 2021-2022 Talkweb Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _HDF_BASE_HAL_H
#define _HDF_BASE_HAL_H
#include "device_resource_if.h"
#include "hdf_log.h"
#include "stdbool.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"

/**
 * @brief 外设复用功能映射表
 *
 */
enum PIN_ALTERNATE_MAP {
    ALTERNATE_SYSTEM = 0,
    ALTERNATE_TIM1_2 = 1,
    ALTERNATE_TIM3_4_5 = 2,
    ALTERNATE_TIM8_9_10_11 = 3,
    ALTERNATE_I2C1_2_3 = 4,
    ALTERNATE_SPI1_2 = 5,
    ALTERNATE_SPI3 = 6,
    ALTERNATE_USART1_2_3 = 7,
    ALTERNATE_USART4_5_6 = 8,
    ALTERNATE_CAN1_2 = 9,
    ALTERNATE_TIM12_13_14 = 9,
    ALTERNATE_OTG_FS = 10,
    ALTERNATE_OTG_HS = 10,
    ALTERNATE_ETH = 11,
    ALTERNATE_FSMC = 12,
    ALTERNATE_SDIO = 12,
    ALTERNATE_OTG_HS_2 = 12,
    ALTERNATE_DCMI = 13,
    ALTERNATE_DEFAULT = 14,
    ALTERNATE_EVENTOUT = 15,
};

/**
 * @brief HDF GPIO初始化入参
 *
 */
typedef struct {

    /*端口号
     GPIOA ~ GPIOH对应0~8
    */
    unsigned int port;

    /*引脚号
     GPIO_PIN_0 ~ GPIO_PIN_15对应0~15
    */
    unsigned int pin;

    /*模式：
    LL_GPIO_MODE_INPUT = 0
    LL_GPIO_MODE_OUTPUT = 1
    LL_GPIO_MODE_ALTERNATE = 2
    LL_GPIO_MODE_ANALOG = 3 */
    unsigned int mode;

    /* 速度:
    LL_GPIO_SPEED_FREQ_LOW = 0
    LL_GPIO_SPEED_FREQ_MEDIUM = 1
    LL_GPIO_SPEED_FREQ_HIGH = 2
    LL_GPIO_SPEED_FREQ_VERY_HIGH = 3*/
    unsigned int speed;

    /* 输出类型:
    LL_GPIO_OUTPUT_PUSHPULL = 0
    LL_GPIO_OUTPUT_OPENDRAIN = 1*/
    unsigned int outputType;

    /* 上下拉:
    LL_GPIO_PULL_NO = 0
    LL_GPIO_PULL_UP = 1
    LL_GPIO_PULL_DOWN = 2*/
    unsigned int pull;

    /* 复用: 当mode = LL_GPIO_MODE_ALTERNATE时有效*/
    enum PIN_ALTERNATE_MAP alternate;

} NIOBE_HDF_GPIO_ATTR;


bool NiobeHdfGpioInit(const struct DeviceResourceNode *resourceNode, struct DeviceResourceIface *dir);

#endif
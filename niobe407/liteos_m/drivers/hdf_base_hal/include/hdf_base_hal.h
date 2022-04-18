/*
 * Copyright (c) 2022 Talkweb Co., Ltd.
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

#define GPIO_NUM_CONFIG_MAX 20
#define GPIO_HCS_NUM 7

typedef enum {
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
    ALTERNATE_MAX = 16,
} PIN_ALTERNATE_MAP;

typedef enum {
    NIOBE_GPIO_PORT_A = 0,
    NIOBE_GPIO_PORT_B,
    NIOBE_GPIO_PORT_C,
    NIOBE_GPIO_PORT_D,
    NIOBE_GPIO_PORT_E,
    NIOBE_GPIO_PORT_F,
    NIOBE_GPIO_PORT_G,
    NIOBE_GPIO_PORT_H,
    NIOBE_GPIO_PORT_I,
    NIOBE_GPIO_PORT_MAX,
} GPIO_PORT_MAP;

typedef enum {
    NIOBE_GPIO_PIN_0 = 0, /* Pin 0 selected    */
    NIOBE_GPIO_PIN_1,     /* Pin 1 selected    */
    NIOBE_GPIO_PIN_2,     /* Pin 2 selected    */
    NIOBE_GPIO_PIN_3,     /* Pin 3 selected    */
    NIOBE_GPIO_PIN_4,     /* Pin 4 selected    */
    NIOBE_GPIO_PIN_5,     /* Pin 5 selected    */
    NIOBE_GPIO_PIN_6,     /* Pin 6 selected    */
    NIOBE_GPIO_PIN_7,     /* Pin 7 selected    */
    NIOBE_GPIO_PIN_8,     /* Pin 8 selected    */
    NIOBE_GPIO_PIN_9,     /* Pin 9 selected    */
    NIOBE_GPIO_PIN_10,    /* Pin 10 selected   */
    NIOBE_GPIO_PIN_11,    /* Pin 11 selected   */
    NIOBE_GPIO_PIN_12,    /* Pin 12 selected   */
    NIOBE_GPIO_PIN_13,    /* Pin 13 selected   */
    NIOBE_GPIO_PIN_14,    /* Pin 14 selected   */
    NIOBE_GPIO_PIN_15,    /* Pin 15 selected   */
    NIOBE_GPIO_PIN_MAX,   /* Pin Max */
} GPIO_PIN_MAP;

typedef enum {
    NIOBE_GPIO_MODE_INPUT = 0,
    NIOBE_GPIO_MODE_OUTPUT,
    NIOBE_GPIO_MODE_ALTERNATE,
    NIOBE_GPIO_MODE_ANALOG,
    NIOBE_GPIO_MODE_MAX,
} GPIO_MODE_MAP;

typedef enum {
    NIOBE_GPIO_SPEED_LOW = 0,
    NIOBE_GPIO_SPEED_MID,
    NIOBE_GPIO_SPEED_HIGH,
    NIOBE_GPIO_SPEED_VERY_HIGH,
    NIOBE_GPIO_SPEED_MAX,
} GPIO_SPEED_MAP;

typedef enum {
    NIOBE_GPIO_OUTPUTTYPE_PUSHPULL = 0,
    NIOBE_GPIO_OUTPUTTYPE_OPENDRAIN,
    NIOBE_GPIO_OUTPUTTYPE_MAX,
} GPIO_OUTPUTTYPE_MAP;

typedef enum {
    NIOBE_GPIO_PULL_NO = 0,
    NIOBE_GPIO_PULL_UP,
    NIOBE_GPIO_PULL_DOWN,
    NIOBE_GPIO_PULL_MAX,
} GPIO_PULL_MAP;

typedef struct {
    /* gpio group num */
    GPIO_PORT_MAP port;

    /* gpio pin num */
    GPIO_PIN_MAP pin;

    /* gpio mode */
    GPIO_MODE_MAP mode;

    /* gpio speed */
    GPIO_SPEED_MAP speed;

    /* gpio outout type */
    GPIO_OUTPUTTYPE_MAP outputType;

    /* pull up or pull down */
    GPIO_PULL_MAP pull;

    /* multiplexing: Valid when mode = LL_GPIO_MODE_ALTERNATE */
    PIN_ALTERNATE_MAP alternate;
} HDF_GPIO_ATTR;

bool NiobeHdfGpioInit(const struct DeviceResourceNode *resourceNode, struct DeviceResourceIface *dir);
bool NiobeInitGpioInit(const HDF_GPIO_ATTR* attr);
#endif
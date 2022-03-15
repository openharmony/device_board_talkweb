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

#ifndef __NIOBE407_LL_GPIO_H
#define __NIOBE407_LL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

#if defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH) || defined (GPIOI) || defined (GPIOJ) || defined (GPIOK)

#if defined(USE_FULL_LL_DRIVER)

typedef enum {
    STM32_GPIO_PIN_0 = 0, /* Pin 0 selected    */
    STM32_GPIO_PIN_1,     /* Pin 1 selected    */
    STM32_GPIO_PIN_2,     /* Pin 2 selected    */
    STM32_GPIO_PIN_3,     /* Pin 3 selected    */
    STM32_GPIO_PIN_4,     /* Pin 4 selected    */
    STM32_GPIO_PIN_5,     /* Pin 5 selected    */
    STM32_GPIO_PIN_6,     /* Pin 6 selected    */
    STM32_GPIO_PIN_7,     /* Pin 7 selected    */
    STM32_GPIO_PIN_8,     /* Pin 8 selected    */
    STM32_GPIO_PIN_9,     /* Pin 9 selected    */
    STM32_GPIO_PIN_10,    /* Pin 10 selected   */
    STM32_GPIO_PIN_11,    /* Pin 11 selected   */
    STM32_GPIO_PIN_12,    /* Pin 12 selected   */
    STM32_GPIO_PIN_13,    /* Pin 13 selected   */
    STM32_GPIO_PIN_14,    /* Pin 14 selected   */
    STM32_GPIO_PIN_15,    /* Pin 15 selected   */
    STM32_GPIO_PIN_MAX,   /* Pin Max */
} STM32_GPIO_PIN;

typedef enum {
    STM32_GPIO_GROUP_A = 0,
    STM32_GPIO_GROUP_B,
    STM32_GPIO_GROUP_C,
    STM32_GPIO_GROUP_D,
    STM32_GPIO_GROUP_E,
    STM32_GPIO_GROUP_F,
    STM32_GPIO_GROUP_G,
    STM32_GPIO_GROUP_H,
    STM32_GPIO_GROUP_I,
    STM32_GPIO_GROUP_MAX,
}STM32_GPIO_GROUP;

typedef enum {
    TW_HAL_GPIO_RESET = 0,
    TW_HAL_GPIO_SET,
} LL_PIN_STATE;


__STATIC_INLINE uint16_t LL_GPIO_ReadInputPin(GPIO_TypeDef *GPIOx, uint32_t pinMask)
{
    return (uint32_t)(((READ_REG(GPIOx->IDR)) & pinMask) >> (POSITION_VAL(pinMask)));
}

GPIO_TypeDef* LL_GET_GPIOX(STM32_GPIO_GROUP group);
uint32_t    LL_GET_HAL_PIN(STM32_GPIO_PIN pin);

#endif /* USE_FULL_LL_DRIVER */

#endif /* defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH) || defined (GPIOI) || defined (GPIOJ) || defined (GPIOK) */

#ifdef __cplusplus
}
#endif

#endif /* __NIOBE407_LL_GPIO_H */

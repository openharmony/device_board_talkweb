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

#ifndef _NIOBE407_LL_EXTI_H_
#define _NIOBE407_LL_EXTI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_ll_exti.h"

#if defined(USE_FULL_LL_DRIVER)

#if defined (EXTI)

typedef void (*TW_HAL_GPIO_PIN_EXIT_HANDLER)(uint16_t pin);

typedef struct
{
    LL_EXTI_InitTypeDef initType;
    TW_HAL_GPIO_PIN_EXIT_HANDLER Exithandler;
    uint32_t PinReg;
    GPIO_TypeDef* Gpiox;
} LL_EXTI_InitConfig;

typedef struct {
    uint16_t pin;
    uint16_t localPin;
    uint32_t exitLine;
    uint8_t setup;
    uint8_t group;
    uint8_t trigger;
    GPIO_TypeDef* gpiox;
    uint32_t pinReg;
    TW_HAL_GPIO_PIN_EXIT_HANDLER handler;
} Pin_ST;

typedef struct {
    uint32_t trigger;
} HAL_GPIO_EXIT_CFG_T;

uint32_t LL_SETUP_EXTI(LL_EXTI_InitConfig* cfg, uint16_t pin, uint16_t local, uint8_t group);

#endif /* EXTI */

#endif /* USE_FULL_LL_DRIVER */

#ifdef __cplusplus
}
#endif

#endif /* _NIOBE407_LL_EXTI_H_ */




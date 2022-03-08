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

#if defined(USE_FULL_LL_DRIVER)

#include "niobe407_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"

#if defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH) || defined (GPIOI) || defined (GPIOJ) || defined (GPIOK)

GPIO_TypeDef* LL_GET_GPIOX(STM32_GPIO_GROUP group)
{
    GPIO_TypeDef *GroupAddr = NULL;
    switch(group) {
        case STM32_GPIO_GROUP_A:
            GroupAddr = ((GPIO_TypeDef *)GPIOA);
            break;
        case STM32_GPIO_GROUP_B:
            GroupAddr = ((GPIO_TypeDef *)GPIOB);
            break;
        case STM32_GPIO_GROUP_C:
            GroupAddr = ((GPIO_TypeDef *)GPIOC);
            break;
        case STM32_GPIO_GROUP_D:
            GroupAddr = ((GPIO_TypeDef *)GPIOD);
            break;
        case STM32_GPIO_GROUP_E:
            GroupAddr = ((GPIO_TypeDef *)GPIOE);
            break;
        case STM32_GPIO_GROUP_F:
            GroupAddr = ((GPIO_TypeDef *)GPIOF);
            break;
        case STM32_GPIO_GROUP_G:
            GroupAddr = ((GPIO_TypeDef *)GPIOG);
            break;
        case STM32_GPIO_GROUP_H:
            GroupAddr = ((GPIO_TypeDef *)GPIOH);
            break;
        case STM32_GPIO_GROUP_I:
            GroupAddr = ((GPIO_TypeDef *)GPIOI);
            break;
        default:
            break;
    }
    return GroupAddr;
}
uint32_t LL_GET_HAL_PIN(STM32_GPIO_PIN pin)
{
    return (LL_GPIO_PIN_0 << pin);
}

#endif /* defined (GPIOA) || defined (GPIOB) || defined (GPIOC) || defined (GPIOD) || defined (GPIOE) || defined (GPIOF) || defined (GPIOG) || defined (GPIOH) || defined (GPIOI) || defined (GPIOJ) || defined (GPIOK) */

#endif /* USE_FULL_LL_DRIVER */

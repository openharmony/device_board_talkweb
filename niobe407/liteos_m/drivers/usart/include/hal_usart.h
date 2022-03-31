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

#ifndef _HAL_USART_H_
#define _HAL_USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_ll_usart.h"

#if defined (USART1) || defined (USART2) || defined (USART3) || defined (USART6) \
    || defined (UART4) || defined (UART5)

#if defined(USE_FULL_LL_DRIVER)

uint32_t USART_TxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size);
uint32_t USART_RxData(uint8_t num, uint8_t *p_data, uint32_t size, BOOL isBlock);

void UART_IRQ_INIT(USART_TypeDef * UART, uint8_t num, uint32_t irqNum, BOOL isBlock);
void UART_IRQ_DEINIT(USART_TypeDef * UART, uint32_t irqNum);

#endif /* USE_FULL_LL_DRIVER */

#endif /* USART1 || USART2 || USART3 || USART6 || UART4 || UART5 || UART7 || UART8 || UART9 || UART10 */

#ifdef __cplusplus
}
#endif

#endif /* _HAL_USART_H_ */

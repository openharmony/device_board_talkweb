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

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"

#if defined (USART1) || defined (USART2) || defined (USART3) || defined (USART6) || defined (UART4) || defined (UART5) || defined (UART7) || defined (UART8) || defined (UART9) || defined (UART10)

uint32_t USART_Block_TxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size)
{
    while(size)
    {
        while(!LL_USART_IsActiveFlag_TXE(UART)){}
        LL_USART_TransmitData8(UART, *p_data);
        size--;
        p_data++;
    }
    while(LL_USART_IsActiveFlag_TC(UART)==RESET){}
    return size;
}

uint32_t USART_Block_RxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size, volatile uint32_t* exitFlag)
{
    uint32_t readLen = size;
    while(size && (*exitFlag != 1)) {
        while(!LL_USART_IsActiveFlag_RXNE(UART)){
            if (*exitFlag) {
                return readLen - size;
            }
        }
        if (size == readLen) {
            LL_USART_EnableIT_IDLE(UART);
        }
        *p_data = LL_USART_ReceiveData8(UART);
        size--;
        p_data++;
    }

    return readLen - size;
}

uint32_t USART_NoBlock_TxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size)
{
    while(size)
    {
        LL_USART_TransmitData8(UART, *p_data);
        size--;
        p_data++;
    }

    return size;
}
uint32_t USART_NoBlock_RxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size)
{
    while(size)
    {
        *p_data = LL_USART_ReceiveData8(UART);
        size--;
        p_data++;
    }

    return size;
}

#endif /* USART1 || USART2 || USART3 || USART6 || UART4 || UART5 || UART7 || UART8 || UART9 || UART10 */

#endif /* USE_FULL_LL_DRIVER */

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

#if defined(USE_FULL_LL_DRIVER)

#include "niobe407_ll_spi.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"

#if defined (SPI1) || defined (SPI2) || defined (SPI3) || defined (SPI4) || defined (SPI5) || defined(SPI6)

uint8_t LL_SPI_Transmit(SPI_TypeDef* SPIx ,uint8_t byte)
{
    uint8_t read,send = byte;
    while (LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
    LL_SPI_TransmitData8(SPIx, send);
    while (LL_SPI_IsActiveFlag_RXNE(SPIx) == 0);
    read = LL_SPI_ReceiveData8(SPIx);
    return read;
}

#endif /* defined (SPI1) || defined (SPI2) || defined (SPI3) || defined (SPI4) || defined (SPI5) || defined(SPI6) */

#endif /* USE_FULL_LL_DRIVER */

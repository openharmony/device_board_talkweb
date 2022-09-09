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

#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "uart.h"
#include "los_task.h"

#if defined (USART1) || defined (USART2) || defined (USART3) || \
    defined (UART4) || defined (UART5) || defined (USART6)

#define UART_NUM_MAX 6
#define UART_IRQ_NUM 0
#define UART_NUM1 1
#define UART_NUM2 2
#define UART_NUM3 3
#define UART_NUM4 4
#define UART_NUM5 5
#define UART_NUM6 6

static EVENT_CB_S g_uartInputEvent;
static BOOL g_eventInited = FALSE;
static RingBuffer *g_uartRingBuf[UART_NUM_MAX] = {NULL};
#define RING_BUFFER_SIZE 128

typedef void (*UART_FUNC_CB)(void);
static void USART1_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART1);
        LL_USART_ClearFlag_RXNE(USART1);
        RingBufWrite(g_uartRingBuf[UART_NUM1], value);
    }

    return;
}

static void USART1_BLOCK_IRQ_Func(void)
{
    uint8_t value = 0;
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        value = LL_USART_ReceiveData8(USART1);
        LL_USART_ClearFlag_RXNE(USART1);
        RingBufWrite(g_uartRingBuf[UART_NUM1 - 1], value);
    }

    if (LL_USART_IsActiveFlag_IDLE(USART1)) {
        LL_USART_ClearFlag_IDLE(USART1);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM1) != 0) {
            return;
        }
    }

    return;
}

static void USART2_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART2);
        LL_USART_ClearFlag_RXNE(USART2);
        RingBufWrite(g_uartRingBuf[UART_NUM2 - 1], value);
    }

    return;
}

static void USART2_BLOCK_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART2)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART2);
        LL_USART_ClearFlag_RXNE(USART2);
        RingBufWrite(g_uartRingBuf[UART_NUM2 - 1], value);
    }

    LL_USART_EnableIT_IDLE(USART2);
    if (LL_USART_IsActiveFlag_IDLE(USART2)) {
        LL_USART_DisableIT_IDLE(USART2);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM2) != 0) {
            return;
        }
    }

    return;
}

static void USART3_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART3)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART3);
        LL_USART_ClearFlag_RXNE(USART3);
        RingBufWrite(g_uartRingBuf[UART_NUM3], value);
    }

    return;
}

static void USART3_BLOCK_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART3)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART3);
        LL_USART_ClearFlag_RXNE(USART3);
        RingBufWrite(g_uartRingBuf[UART_NUM3 - 1], value);
    }

    LL_USART_EnableIT_IDLE(USART3);
    if (LL_USART_IsActiveFlag_IDLE(USART3)) {
        LL_USART_DisableIT_IDLE(USART3);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM3) != 0) {
            return;
        }
    }

    return;
}

static void USART4_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(UART4)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(UART4);
        LL_USART_ClearFlag_RXNE(UART4);
        RingBufWrite(g_uartRingBuf[UART_NUM4], value);
    }

    return;
}

static void USART4_BLOCK_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(UART4)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(UART4);
        LL_USART_ClearFlag_RXNE(UART4);
        RingBufWrite(g_uartRingBuf[UART_NUM4 - 1], value);
    }

    LL_USART_EnableIT_IDLE(UART4);
    if (LL_USART_IsActiveFlag_IDLE(UART4)) {
        LL_USART_DisableIT_IDLE(UART4);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM4) != 0) {
            return;
        }
    }

    return;
}

static void USART5_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(UART5)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(UART5);
        LL_USART_ClearFlag_RXNE(UART5);
        RingBufWrite(g_uartRingBuf[UART_NUM5], value);
    }

    return;
}

static void USART5_BLOCK_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(UART5)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(UART5);
        LL_USART_ClearFlag_RXNE(UART5);
        RingBufWrite(g_uartRingBuf[UART_NUM5 - 1], value);
    }

    LL_USART_EnableIT_IDLE(UART5);
    if (LL_USART_IsActiveFlag_IDLE(UART5)) {
        LL_USART_DisableIT_IDLE(UART5);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM5) != 0) {
            return;
        }
    }

    return;
}

static void USART6_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART6)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART6);
        RingBufWrite(g_uartRingBuf[UART_NUM6], value);
    }

    return;
}

static void USART6_BLOCK_IRQ_Func(void)
{
    if (LL_USART_IsActiveFlag_RXNE(USART6)) {
        uint8_t value;
        value = LL_USART_ReceiveData8(USART6);
        LL_USART_ClearFlag_RXNE(USART6);
        RingBufWrite(g_uartRingBuf[UART_NUM6 - 1], value);
    }

    LL_USART_EnableIT_IDLE(USART6);
    if (LL_USART_IsActiveFlag_IDLE(USART6)) {
        LL_USART_DisableIT_IDLE(USART6);
        if (LOS_EventWrite(&g_uartInputEvent, UART_NUM6) != 0) {
            return;
        }
    }

    return;
}

static UART_FUNC_CB g_funcMap[UART_NUM_MAX] = {
    USART1_IRQ_Func,
    USART2_IRQ_Func,
    USART3_IRQ_Func,
    USART4_IRQ_Func,
    USART5_IRQ_Func,
    USART6_IRQ_Func,
};

static UART_FUNC_CB g_funcBlockMap[UART_NUM_MAX] = {
    USART1_BLOCK_IRQ_Func,
    USART2_BLOCK_IRQ_Func,
    USART3_BLOCK_IRQ_Func,
    USART4_BLOCK_IRQ_Func,
    USART5_BLOCK_IRQ_Func,
    USART6_BLOCK_IRQ_Func,
};

uint32_t USART_TxData(USART_TypeDef * UART, uint8_t *p_data, uint32_t size)
{
    while (size) {
        while (!LL_USART_IsActiveFlag_TXE(UART));
        LL_USART_TransmitData8(UART, *p_data);
        size--;
        p_data++;
    }
    while (LL_USART_IsActiveFlag_TC(UART) == RESET);
    return size;
}

void UART_IRQ_INIT(USART_TypeDef * UART, uint8_t num, uint32_t irqNum, BOOL isBlock)
{
    g_uartRingBuf[num - 1] = RingBufInit(RING_BUFFER_SIZE);
    if (g_uartRingBuf[num - 1] == NULL) {
        printf("RingBufInit fail!\n");
        return;
    }
    if (isBlock) {
        if (!g_eventInited) {
            int32_t ret = LOS_EventInit(&g_uartInputEvent);
            if (ret != LOS_OK) {
                printf("Init uartInputEvent failed! ERROR: 0x%x\n", ret);
                return;
            } else {
                g_eventInited = TRUE;
            }
        }

        LL_USART_EnableIT_RXNE(UART);
        LL_USART_EnableIT_IDLE(UART);
        ArchHwiCreate(irqNum, UART_IRQ_NUM, 1, g_funcBlockMap[num - 1], NULL);
    } else {
        LL_USART_EnableIT_RXNE(UART);
        ArchHwiCreate(irqNum, UART_IRQ_NUM, 1, g_funcMap[num - 1], NULL);
    }

    return;
}

void UART_IRQ_DEINIT(USART_TypeDef * UART, uint32_t irqNum)
{
    LL_USART_DisableIT_RXNE(UART);
    ArchHwiDelete(irqNum, NULL);

    return;
}

uint32_t USART_RxData(uint8_t num, uint8_t *p_data, uint32_t size, BOOL isBlock)
{
    uint32_t readLen = 0;
    unsigned char data;
    if (isBlock) {
        (VOID)LOS_EventRead(&g_uartInputEvent, num, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
    }

    while (size--) {
        if (RingBufRead(g_uartRingBuf[num - 1], &data) == 0) {
            *p_data = data;
            readLen++;
            p_data++;
        } else {
            break;
        }
    }

    return readLen;
}
#endif /* USART1 || USART2 || USART3 || USART6 || UART4 || UART5 */

#endif /* USE_FULL_LL_DRIVER */

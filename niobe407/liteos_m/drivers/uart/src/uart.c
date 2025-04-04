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
#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_UART
#include <hdf_log.h>
#include <uart_if.h>
#include "los_task.h"
#define HDF_SHELL_STACK_SIZE 0x1000
#define HDF_SHELL_TASK_NAME "hdf_shell_task"
#define HDF_SHELL_TASK_PRIORITY 26
#define MAX_BUF_SIZE 1024
uint8_t rbuf[MAX_BUF_SIZE] = {0};
DevHandle handle = NULL;
#define UART_DEBUG_SHELL_PORT 1
#endif

#include "stdio.h"
#include "los_config.h"
#include "los_reg.h"
#include "los_interrupt.h"
#include "los_event.h"
#include "stm32f4xx_hal_uart.h"
#include "uart.h"

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_UART
INT32 UartPutc(INT32 ch, VOID *file)
{
    char RL = '\r';
    if (handle == NULL) {
        if (ch =='\n') {
            HAL_UART_Transmit(&huart1, &RL, 1, 0xFFFF);
        }
        return HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    } else {
        if (ch == '\n') {
            UartWrite(handle, &RL, 1);
        }
        return UartWrite(handle, (uint8_t *)&ch, 1);
    }
}
#else
INT32 UartPutc(INT32 ch, VOID *file)
{
    char RL = '\r';
    if (ch =='\n') {
        HAL_UART_Transmit(&huart1, &RL, 1, 0xFFFF);
    }

    return HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}
#endif

#define CN_RCV_RING_BUFLEN  128
static RingBuffer *g_debugRingBuf;
/**
 * @brief: use this function to read data from ring buffer
 * @input: ringBuf, the ring buf to be read
 * @input: data, data to be read storaged
 * @return: 0 success while -1 failed
 */
int RingBufRead(RingBuffer *buf, unsigned char *data)
{
    if (buf->dataLen == 0) {
        return -1;
    } else {
        *data = buf->buf[buf->posR];
        buf->posR = (buf->posR + 1) % buf->size;
        buf->dataLen--;
        return 0;
    }
}

/**
 * @brief: use this function to write data to ring buffer
 * @input: ringBuf, the ring buf to be read
 * @input: data, data to be written
 * @return: 0 success while -1 failed
 */
int RingBufWrite(RingBuffer *buf, unsigned char data)
{
    if (buf->dataLen == buf->size) {
        return -1;
    } else {
        buf->buf[buf->posW] = data;
        buf->posW = (buf->posW + 1) % buf->size;
        buf->dataLen++;
        return 0;
    }
}

int RingBufWriteMore(RingBuffer *buf, unsigned char* data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        if (RingBufWrite(buf, data[i]) !=0) {
            break;
        }
    }

    return 0;
}

/**
 * @brief: use this function to make a ringbuffer
 * @return: the ring buffer maked, NULL failed while others success
 */
RingBuffer* RingBufInit(int size)
{
    RingBuffer *buf;
    if (size <= 0) {
        return NULL;
    }
    buf = (RingBuffer*)malloc(size + sizeof(RingBuffer));
    if (buf != NULL) {
        buf->buf = (unsigned char*) buf + sizeof(RingBuffer);
        buf->dataLen = 0;
        buf->posR = 0;
        buf->posW = 0;
        buf->size = size;
    }
    return buf;
}

uint8_t UartGetc(void)
{
    unsigned char data;
    if (RingBufRead(g_debugRingBuf, &data) == 0) {
        return (int) data;
    } else {
        return 0;
    }
}

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_UART
static INT32 InitDebugShellUart(uint32_t port)
{
    handle = UartOpen(port);
    if (handle == NULL) {
        return 1;
    }

    return 0;
}
static void HdfShellTaskEntry(void)
{
    while (1) {
        memset_s(rbuf, MAX_BUF_SIZE, 0, MAX_BUF_SIZE);
        int32_t readLen = UartRead(handle, rbuf, MAX_BUF_SIZE);
        if (readLen < 0) {
            return;
        } else {
            for (int i = 0; i < readLen; i++) {
                (void)RingBufWrite(g_debugRingBuf, rbuf[i]);
                (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
            }
        }
    }

    UartClose(handle);

    return;
}

void StartUartShell(void)
{
    UINT32 uwRet;
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)HdfShellTaskEntry;
    stTask.uwStackSize = HDF_SHELL_STACK_SIZE;
    stTask.pcName = HDF_SHELL_TASK_NAME;
    stTask.usTaskPrio = HDF_SHELL_TASK_PRIORITY; /* Os task priority is 26 */
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        HDF_LOGE("Task1 create failed\n");
    }
}

VOID ShellUartInit(VOID)
{
    LosShellInit();
    g_debugRingBuf = RingBufInit(CN_RCV_RING_BUFLEN);
    if (g_debugRingBuf == NULL) {
        return;
    }

    handle = UartOpen(UART_DEBUG_SHELL_PORT);
    if (handle == NULL) {
        HDF_LOGE("UartOpen: failed!\n");
        return;
    }
    StartUartShell();
}
#else
static void huart1_irq(void)
{
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) {
        unsigned char value;
        value = (uint8_t) (huart1.Instance->DR & 0x00FF);
        RingBufWrite(g_debugRingBuf, value);
    } else if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
    }
}

VOID ShellUartInit(VOID)
{
    LosShellInit();
    g_debugRingBuf = RingBufInit(CN_RCV_RING_BUFLEN);
    if (g_debugRingBuf == NULL) {
        printf("RingBufInit fail!\n");
        return;
    }
    LOS_HwiCreate(USART1_IRQn, 0, 1, (HWI_PROC_FUNC)huart1_irq, 0);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}
#endif

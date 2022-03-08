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

#include <hdf_log.h>
#include <uart_if.h>
#include "los_task.h"
#include "los_compiler.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "samgr_lite.h"
#include "ohos_run.h"
#define USE_FULL_LL_DRIVER

#define HDF_USART_STACK_SIZE 0x1000
#define HDF_USART_TASK_NAME "hdf_usart_test_task"
#define HDF_USART_TASK_PRIORITY 2

uint8_t txbuf[80] = "this is usart test function\nthe data will echo later\n";
uint8_t rxbuf[80] = {0};
uint8_t len = 0;
uint8_t tmp;

static void* HdfUsartTestEntry(void* arg)
{
    uint32_t port = 4;                  /* UART设备端口号 */
    DevHandle handle = UartOpen(port);
    if (handle == NULL) {
        HDF_LOGE("UartOpen: failed!\n");
        return NULL;
    }

    int32_t ret;
    uint32_t baudRate;
    /* 获取UART波特率 */
    ret = UartGetBaud(handle, &baudRate);
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartGetBaud: success, badurate %d\n", baudRate);
    }

    baudRate = 115200;
    ret = UartSetBaud(handle, baudRate);
    if (ret != 0) {
        HDF_LOGE("UartGetBaud: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartSetBaud: success, badurate %d\n", baudRate);
    }

    struct UartAttribute attribute = {0};

    ret = UartGetAttribute(handle, &attribute);
    if (ret != 0) {
        HDF_LOGE("UartGetAttribute: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartGetAttribute: success\n");
    }

    attribute.dataBits = UART_ATTR_DATABIT_8;
    attribute.fifoTxEn = 1;
    attribute.fifoRxEn = 1;
    attribute.parity = UART_ATTR_PARITY_NONE;
    attribute.stopBits = UART_ATTR_STOPBIT_1;
    attribute.cts = 0;
    attribute.rts = 0;

    ret = UartSetAttribute(handle, &attribute);
    if (ret != 0) {
        HDF_LOGE("UartSetAttribute: failed, ret %d\n", ret);
    } else {
        HDF_LOGI("UartSetAttribute: success\n");
    }

    len = strlen((char *)txbuf);
    ret = UartWrite(handle, txbuf, len);
    if (ret != 0) {
        HDF_LOGE("UartWrite: failed, ret %d\n", ret);
        goto _ERR;
    } else {
        HDF_LOGI("UartWrite: success, ret %d\n", ret);
    }

    ret = UartRead(handle, rxbuf, len);
    if (ret < 0) {
        HDF_LOGE("UartRead: failed, ret %d\n", ret);
        goto _ERR;
    } else {
        HDF_LOGI("UartRead byte %d content is %s\n", ret, rxbuf);
    }

    UartClose(handle);
_ERR:
    /* 销毁UART设备句柄 */
    UartClose(handle); 

}

void StartHdfUsartTest(void)
{
    UINT32 uwRet;
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)HdfUsartTestEntry;
    stTask.uwStackSize = HDF_USART_STACK_SIZE;
    stTask.pcName = HDF_USART_TASK_NAME;
    stTask.usTaskPrio = HDF_USART_TASK_PRIORITY; /* Os task priority is 2 */
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        printf("Task1 create failed\n");
    }
}

OHOS_APP_RUN(StartHdfUsartTest);


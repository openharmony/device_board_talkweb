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
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_run.h"
#include "cmsis_os2.h"

typedef struct
{
    char *buffer;
} MSG_BUF;

MSG_BUF g_msg_buf;
osMessageQueueId_t g_msg_queue;

void ThreadMsgQueue1(void *arg)
{
    (void)arg;
    g_msg_buf.buffer = "hello niobe407";
    while (1) {
        osMessageQueuePut(g_msg_queue, &g_msg_buf.buffer, 0U, 0U);
        osThreadYield();
        osDelay(100);
    }
}

void ThreadMsgQueue2(void *arg)
{
    (void)arg;
    osStatus_t status;

    while (1) {
        //wait for message
        status = osMessageQueueGet(g_msg_queue, &g_msg_buf.buffer, NULL, 0U);
        if (status == osOK) {
            printf("ThreadMsgQueue2 Get msg: %s\n", g_msg_buf.buffer);
        }
        else {
            osDelay(100);
        }
    }
}

static void MsgExample(void)
{
    g_msg_queue = osMessageQueueNew(10, 50, NULL);
    if (g_msg_queue == NULL) {
        printf("create Message Queue failed!\n");
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 10;
    attr.priority = 25;

    attr.name = "ThreadMsgQueue1";
    if (osThreadNew(ThreadMsgQueue1, NULL, &attr) == NULL) {
        printf("create ThreadMsgQueue1 failed!\n");
    }

    attr.name = "ThreadMsgQueue2";
    if (osThreadNew(ThreadMsgQueue2, NULL, &attr) == NULL) {
        printf("create ThreadMsgQueue2 failed!\n");
    }
}

OHOS_APP_RUN(MsgExample);

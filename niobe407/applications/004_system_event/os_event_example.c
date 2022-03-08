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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_run.h"
#include "cmsis_os2.h"

#define FLAGS_MSK1 0x00000001U

osEventFlagsId_t g_event_flags_id; // event flags id

/**
 *发送事件
 *\param[in] argument 发送参数
 */
void OS_Thread_EventSender(void *argument)
{
    osEventFlagsId_t flags;
    (void)argument;
    printf("Start OS_Thread_EventSender.\n");
    while (1)
    {
        /// Set the specified Event Flags.
        /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
        /// \param[in]     flags         specifies the flags that shall be set.
        /// \return event flags after setting or error code if highest bit set.
        flags = osEventFlagsSet(g_event_flags_id, FLAGS_MSK1);

        printf("Send Flags is %d\n", flags);
        //挂起线程，让位其他线程调度
        osThreadYield();
        osDelay(1000);
    }
}

/**
 *  接收事件
 * \param[in] argument 发送参数
*/
void OS_Thread_EventReceiver(void *argument)
{
    (void)argument;
    printf("Start OS_Thread_EventSender.\n");
    while (1)
    {
        uint32_t flags;
        /// Wait for one or more Event Flags to become signaled.
        /// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
        /// \param[in]     flags         specifies the flags to wait for.
        /// \param[in]     options       specifies flags options (osFlagsXxxx).
        /// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
        /// \return event flags before clearing or error code if highest bit set.
        flags = osEventFlagsWait(g_event_flags_id, FLAGS_MSK1, osFlagsWaitAny, osWaitForever);
        printf("Receive Flags is %u\n", flags);
    }
}

/**
 * 事件测试Example
*/
void OS_Event_example(void)
{
    printf("Start OS_Event_example.\n");
    //  ==== Event Flags Management Functions ====
    /// Create and Initialize an Event Flags object.
    /// \param[in]     attr          event flags attributes; NULL: default values.
    /// \return event flags ID for reference by other functions or NULL in case of error.
    g_event_flags_id = osEventFlagsNew(NULL);
    if (g_event_flags_id == NULL)
    {
        printf("Falied to create EventFlags!\n");
        return;
    }

    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 25;

    attr.name = "Thread_EventSender";
    /// Create a thread and add it to Active Threads.
    /// \param[in]     func          thread function.
    /// \param[in]     argument      pointer that is passed to the thread function as start argument.
    /// \param[in]     attr          thread attributes; NULL: default values.
    /// \return thread ID for reference by other functions or NULL in case of error
    //osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);
    if (osThreadNew(OS_Thread_EventSender, NULL, &attr) == NULL)
    {
        printf("Falied to create Thread_EventSender!\n");
        return;
    }
    
    attr.name = "Thread_EventReceiver";
    if (osThreadNew(OS_Thread_EventReceiver, NULL, &attr) == NULL)
    {
        printf("Falied to create Thread_EventReceiver!\n");
        return;
    }
}

OHOS_APP_RUN(OS_Event_example);

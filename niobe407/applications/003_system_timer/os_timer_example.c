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

unsigned int timer1Exec=1, timer2Exec=100;

void OS_Timer1_Callback(void *arg)
{
    printf("This is Niobe407 Timer1_Callback:%u!\r\n", *(unsigned int*)arg);
}

void OS_Timer2_Callback(void *arg)
{
    printf("This is Niobe407 Timer2_Callback:%u!\r\n", *(unsigned int*)arg);
}

static void OS_Timer_example(void)
{
    osTimerId_t timerId1, timerId2;
    unsigned int delay;
    osStatus_t status;

    timer1Exec = 1U;
    timerId1 = osTimerNew((osTimerFunc_t)OS_Timer1_Callback, osTimerPeriodic, &timer1Exec, NULL);
    if (timerId1 != NULL)
    {
        delay = 100U;
        status = osTimerStart(timerId1, delay);
        if (status != osOK)
        {
            printf("Falied to start timer1!\n");
        }
    }

    timer2Exec = 100U;
    timerId2 = osTimerNew((osTimerFunc_t)OS_Timer2_Callback, osTimerPeriodic, &timer2Exec, NULL);
    if (timerId2 != NULL)
    {
        delay = 300U;
        status = osTimerStart(timerId2, delay);
        if (status != osOK)
        {
            printf("Falied to start timer2!\n");
        }
    }
}

OHOS_APP_RUN(OS_Timer_example);

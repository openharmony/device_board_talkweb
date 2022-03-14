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

osSemaphoreId_t sem1;

void Thread_Semp1(void)
{
	while(1)
	{
		osSemaphoreRelease(sem1);
		printf("Thread_Semp1 Release  Semap \n");
		osDelay(200U);
	}
}

void Thread_Semp2(void)
{
	while(1)
	{
		osSemaphoreAcquire(sem1, osWaitForever);
		printf("Thread_Semp2 get Semap \n");
		osDelay(100U);
	}
}

void semp_example(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;
    attr.priority = 10;

    attr.name = "Thread_Semp1";
    if (osThreadNew((osThreadFunc_t)Thread_Semp1, NULL, &attr) == NULL) {
        printf("create Thread_Semp1 failed!\n");
    }

    attr.name = "Thread_Semp2";
    if (osThreadNew((osThreadFunc_t)Thread_Semp2, NULL, &attr) == NULL) {
        printf("create Thread_Semp2 failed!\n");
    }
	
    sem1 = osSemaphoreNew(3, 0, NULL);
    if (sem1 == NULL) {
        printf("Semp1 create failed!\n");
    }
}

OHOS_APP_RUN(semp_example);
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

osMutexId_t mutex_id;

void firstThread(void)
{
	osDelay(100U);
	while(1)
	{
		osMutexAcquire(mutex_id, osWaitForever);
    	printf("firstThread is Acquire.\r\n");
    	osDelay(1000U);
		osMutexRelease(mutex_id);
	}
}

void twoThread(void)
{
	osDelay(1000U);
	while(1) 
	{
		printf("twoThread is Acquire.\r\n");
    	osDelay(1000U);
	}
}

void threeThread(void)
{
    while(1)
	{
		osMutexAcquire(mutex_id, osWaitForever);
    	printf("threeThread is Acquire.\r\n");
    	osDelay(3000U);
		osMutexRelease(mutex_id);
	}
}

void os_mutex_example(void)
{
    osThreadAttr_t attr;

    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 4;

    attr.name = "firstThread";
    attr.priority = 26;
    if (osThreadNew((osThreadFunc_t)firstThread, NULL, &attr) == NULL)
	{
    	printf("create firstThread failed!\n");
    }

	attr.name = "twoThread";
    attr.priority = 25;
    if (osThreadNew((osThreadFunc_t)twoThread, NULL, &attr) == NULL)
    {
      	printf("create twoThread failed!\n");
    }

    attr.name = "threeThread";
    attr.priority = 24;
    if (osThreadNew((osThreadFunc_t)threeThread, NULL, &attr) == NULL)
    {
      	printf("create threeThread failed!\n");
    }

	mutex_id = osMutexNew(NULL);
    if (mutex_id == NULL)
    {
      	printf("create Mutex failed!\n");
    }
}

OHOS_APP_RUN(os_mutex_example);

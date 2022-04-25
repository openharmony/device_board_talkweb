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
#include "adc_temperature.h"
#include "cmsis_os2.h"
#include "ohos_run.h"

#define TASK_DELAY 1000
#define STACK_SIZE 4096

void thread_entry(void)
{
    while (1) {
        double Current_Temperature = Temperature_Get();
        printf("The IC current temperature is %.2f\r\n", Current_Temperature);
        osDelay(TASK_DELAY);
    }
}

static void adc_temperature_example(void)
{
    osThreadAttr_t attr;

    attr.name = "thread";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = STACK_SIZE;
    attr.priority = osPriorityNormal;

    Temperature_ADC_Init();

    if (osThreadNew((osThreadFunc_t)thread_entry, NULL, &attr) == NULL) {
        printf("Falied to create thread!\n");
    }
}

OHOS_APP_RUN(adc_temperature_example);

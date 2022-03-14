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

#include "ohos_run.h"
#include "osal_irq.h"
#include "osal_time.h"

#include "ohos_run.h"
#include "cmsis_os2.h"
#include <stdio.h>

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_GPIO
#include "hdf_log.h"
#include "gpio_if.h"
#endif

#define HDF_GPIO_STACK_SIZE 0x1000
#define HDF_GPIO_TASK_NAME "hdf_gpio_test_task"
#define HDF_GPIO_TASK_PRIORITY 25

#ifdef LOSCFG_DRIVERS_HDF_PLATFORM_GPIO

static int32_t TestCaseGpioIrqHandler1(uint16_t gpio, void *data)
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(0, &val);
    if (val) {
        GpioWrite(0, 0);
    } else {
        GpioWrite(0, 1);
    }
}

static int32_t TestCaseGpioIrqHandler2(uint16_t gpio, void *data)
{
    HDF_LOGE("%s: irq triggered! on gpio:%u, data=%p", __func__, gpio, data);
    uint16_t val = 0;
    GpioRead(1, &val);
    if (val) {
        GpioWrite(1, 0);
    } else {
        GpioWrite(1, 1);
    }
}


static void* HdfGpioTestEntry(void* arg)
{
    (void)arg;
    int32_t ret;
    uint16_t mode;
    uint16_t gpio2 = 2;
    uint16_t gpio3 = 3;

    mode = OSAL_IRQF_TRIGGER_FALLING;
    HDF_LOGE("%s: mode:%0x\n", __func__, mode);
    ret = GpioSetIrq(gpio2, mode, TestCaseGpioIrqHandler1, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioSetIrq(gpio3, mode, TestCaseGpioIrqHandler2, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioEnableIrq(gpio2);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }

    ret = GpioEnableIrq(gpio3);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq fail! ret:%d\n", __func__, ret);
        return NULL;
    }
}

void StartHdfGpioTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_GPIO_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_GPIO_STACK_SIZE;
    attr.priority = HDF_GPIO_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfGpioTestEntry, NULL, &attr) == NULL) {
        printf("Falied to create thread1!\n");
    }
}
#else
void StartHdfGpioTest(void)
{
    printf("StartHdfGpioTest fail!\n");
    printf("You should open LOSCFG_DRIVERS_HDF_PLATFORM_GPIO support in kernel/liteos_m menuconfig.\n");
}
#endif

OHOS_APP_RUN(StartHdfGpioTest);
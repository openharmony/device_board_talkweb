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
#include <hdf_log.h>
#include <pwm_if.h>
#include "cmsis_os2.h"
#include "samgr_lite.h"
#include "ohos_run.h"
#define USE_FULL_LL_DRIVER

#define HDF_PWM_DUTY 10000000
#define HDF_PWM_PERIOD 50000000
#define HDF_PWM_DUTY1 10000000
#define HDF_PWM_PERIOD1 100000000
#define HDF_PWM_DUTY2 200000
#define HDF_PWM_PERIOD2 400000

#define HDF_PWM_STACK_SIZE 0x1000
#define HDF_PWM_TASK_NAME "hdf_pwm_test_task"
#define HDF_PWM_TASK_PRIORITY 25

#define HDF_PWM_DEALY_MS 1000

#define PWM2 1
#define PWM3 2
#define PWM8 7

#define BEEP_ON 0
static void* HdfPwmTestEntry(void* arg)
{
    int32_t ret;

    DevHandle handle = NULL;

    struct PwmConfig pcfg;
    pcfg.duty = HDF_PWM_DUTY;
    pcfg.period = HDF_PWM_PERIOD;
    pcfg.number = 0;
    pcfg.polarity = PWM_NORMAL_POLARITY;
    pcfg.status = PWM_ENABLE_STATUS;

    handle = PwmOpen(PWM2);
    if (handle == NULL) {
        HDF_LOGE("PwmOpen: failed!\n");
        return;
    } else {
        HDF_LOGI("open pwm %u success!\n", PWM2 + 1);
    }

    ret = PwmSetConfig(handle, &pcfg);
    if (ret != 0) {
        HDF_LOGE("PwmSetConfig: failed, ret %d\n", ret);
        goto _ERR2;
    } else {
        HDF_LOGI("PwmSetConfig success ! duty is %u\n", pcfg.duty);
    }

    DevHandle handle1 = NULL;

    struct PwmConfig pcfg1;
    pcfg1.duty =  HDF_PWM_DUTY1;
    pcfg1.period = HDF_PWM_PERIOD1;
    pcfg1.number = 0;
    pcfg1.polarity = PWM_NORMAL_POLARITY;
    pcfg1.status = PWM_ENABLE_STATUS;

    handle1 = PwmOpen(PWM3);
    if (handle1 == NULL) {
        HDF_LOGE("PwmOpen: failed!\n");
        return;
    } else {
        HDF_LOGI("open pwm %u success!\n", PWM3 + 1);
    }

    ret = PwmSetConfig(handle1, &pcfg1);
    if (ret != 0) {
        HDF_LOGE("PwmSetConfig: failed, ret %d\n", ret);
        goto _ERR1;
    } else {
        HDF_LOGI("PwmSetConfig success ! duty is %u\n", pcfg1.duty);
    }
#if (BEEP_ON == 1)
    DevHandle handle2 = NULL;
    struct PwmConfig pcfg2;
    pcfg2.duty = HDF_PWM_DUTY2;
    pcfg2.period = HDF_PWM_PERIOD2;
    pcfg2.number = 0;
    pcfg2.polarity = PWM_NORMAL_POLARITY;
    pcfg2.status = PWM_ENABLE_STATUS;

    handle2 = PwmOpen(PWM8);
    if (handle2 == NULL) {
        HDF_LOGE("PwmOpen: failed!\n");
        return;
    } else {
        HDF_LOGI("open pwm %u success!\n", PWM8 + 1);
    }

    ret = PwmSetConfig(handle2, &pcfg2);
    if (ret != 0) {
        HDF_LOGE("PwmSetConfig: failed, ret %d\n", ret);
        goto _ERR3;
    } else {
        HDF_LOGI("PwmSetConfig success ! duty is %u\n", pcfg2.duty);
    }
#endif
    while (1) {
        LOS_TaskDelay(HDF_PWM_DEALY_MS);
    }
_ERR1:
    PwmClose(handle);
_ERR2:
    PwmClose(handle1);
#if (BEEP_ON == 1)
_ERR3:
    PwmClose(handle2);
#endif
}

void StartHdfPwmTest(void)
{
    osThreadAttr_t attr;

    attr.name = HDF_PWM_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = HDF_PWM_STACK_SIZE;
    attr.priority = HDF_PWM_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)HdfPwmTestEntry, NULL, &attr) == NULL) {
        printf("Falied to create thread1!\n");
    }
}

OHOS_APP_RUN(StartHdfPwmTest);


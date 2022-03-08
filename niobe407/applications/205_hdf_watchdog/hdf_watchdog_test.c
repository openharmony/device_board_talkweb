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

#define HDF_WATCHDOG_STACK_SIZE 0x1000
#define HDF_WATCHDOG_TASK_NAME "hdf_watchdog_test_task"
#define HDF_WATCHDOG_TASK_PRIORITY 25


static void* HdfWatchdogTestEntry(void* arg)
{
    int32_t ret;
    DevHandle handle = NULL;
    int32_t status;
    uint32_t timeOut = 0;
    
    HDF_LOGE("HdfWatchdogTestEntry ----------\n");

    ret = WatchdogOpen(0, &handle);
    if (HDF_SUCCESS != ret || handle == NULL) {
        HDF_LOGE("WatchdogOpen: failed, ret %d\n", ret);
        return NULL;
    }

    /* 获取Watchdog启动状态 */
    ret = WatchdogGetStatus(handle, &status);
    if (ret != 0) {
        HDF_LOGE("WatchdogGetStatus: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("WatchdogGetStatus: %d\n", status);

    ret = WatchdogGetTimeout(handle, &timeOut);
    if (ret != 0) {
        HDF_LOGE("WatchdogGetTimeout: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("WatchdogGetTimeout: %d\n", timeOut);
    
    ret = WatchdogSetTimeout(handle, 1200);
    if (ret != 0) {
        HDF_LOGE("WatchdogSetTimeout: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("WatchdogSetTimeout: 1200\n");

    ret = WatchdogGetTimeout(handle, &timeOut);
    if (ret != 0) {
        HDF_LOGE("WatchdogGetTimeout: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("WatchdogGetTimeout: %d\n", timeOut);

    /* 启动看门狗 */
    ret = WatchdogStart(handle);
    if (ret != 0) {
        HDF_LOGE("WatchdogStart: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("---> WatchdogStart.\n");

    ret = WatchdogGetStatus(handle, &status);
    if (ret != 0) {
        HDF_LOGE("WatchdogGetStatus: failed, ret %d\n", ret);
        return NULL;
    }else
        HDF_LOGE("WatchdogGetStatus: %d\n", status);

    while (1)
    {
        ret = WatchdogFeed(handle);  //喂狗操作
        if (ret != 0) {
            HDF_LOGE("WatchdogFeed: failed, ret %d\n", ret);
            return NULL;
        }else
            HDF_LOGE("--> WatchdogFeed success!");
        osDelay(timeOut);
    }
}

void StartHdfWatchdogTest(void)
{
    UINT32 uwRet;
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)HdfWatchdogTestEntry;
    stTask.uwStackSize = HDF_WATCHDOG_STACK_SIZE;
    stTask.pcName = HDF_WATCHDOG_TASK_NAME;
    stTask.usTaskPrio = HDF_WATCHDOG_TASK_PRIORITY;
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        printf("Task1 create failed\n");
    }
}

OHOS_APP_RUN(StartHdfWatchdogTest);


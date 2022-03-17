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
#include "los_task.h"
#include "los_compiler.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "samgr_lite.h"
#include "ohos_init.h"
#include "ohos_types.h"
#include "watch_dog.h"

extern void OHOS_SystemInit(void); // this is the service loader function for OHOS

#define TALKWEB_SERVICE_STACKSIZE       (4096)
#define TALKWEB_SERVICE_TASK_PRIOR      25
#define TALKWEB_SERVICE_TASK_NAME       "talkweb_sys_service"

__attribute__((weak)) void ohos_app_main(){
    printf("No application run, Maybe you should config your application in BUILD.gn!\n");
    return;
}

__attribute__((weak)) void before_ohos_run(){
    return;
}

static void talkweb_sys_service()
{
    /* 系统服务配置初始化 */
    sys_service_config();

    /* 启动应用程序 */
    ohos_app_main();

    while(1) {
        feed_dog();           //喂狗
        LOS_TaskDelay(1000);
    }
}

static void OHOS_Main(void)
{
    UINT32 uwRet = -1;
    UINT32 taskID = 0;
    TSK_INIT_PARAM_S stTask = {0};
    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)talkweb_sys_service;
    stTask.uwStackSize = TALKWEB_SERVICE_STACKSIZE;
    stTask.pcName = TALKWEB_SERVICE_TASK_NAME;
    stTask.usTaskPrio = TALKWEB_SERVICE_TASK_PRIOR; /* Os task priority is 6 */
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        printf("Task1 create failed\n");
    }
}

/***
 * @brief This is the ohos entry, and you could call this in your main funciton after the
 *        necessary hardware has been initialized.
 */
void OHOS_Boot(void)
{
    UINT32 ret;

    before_ohos_run();  //系统运行之前的程序入口函数

    ret = LOS_KernelInit();
    if (ret == LOS_OK) {
        OHOS_SystemInit();
        LOS_Start();
    }
    return;  // and should never come here
}

SYS_RUN(OHOS_Main);
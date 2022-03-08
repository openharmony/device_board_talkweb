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

#include "uart.h"
#include "watch_dog.h"
#include "devmgr_service_start.h"

extern int DeviceManagerStart(void); // hdf init

/**
 * @brief 根据内核宏配置在此开启相应功能
 * 注意该函数不能被阻塞
 */
void sys_service_config()
{
#ifdef LOSCFG_WATCH_DOG
    watch_dog_init(1100);     //看门狗初始化
#endif

#ifdef LOSCFG_DRIVERS_HDF
    DeviceManagerStart();     //HDF驱动初始化
#endif

#ifdef LOSCFG_SHELL
    ShellUartInit();          //shell初始化
#endif
}
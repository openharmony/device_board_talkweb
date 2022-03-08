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

#include "watch_dog.h"

IWDG_HandleTypeDef hiwdg;

/**
 * @brief 看门狗初始化
 * 
 * @param timeout 看门狗喂狗超时时间, 取值范围[1,4096], 单位ms
 * @return int  成功返回0, 失败返回-1
 */
int watch_dog_init(unsigned int timeout)
{
    if(timeout < 1 || timeout > 4096)
        return -1;
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = timeout - 1;   //重装初值，超时周期（取值范围0-4095, 32K晶振在32预分频下对应1-4096ms）
    HAL_IWDG_Init(&hiwdg);  //初始化看门狗参数
    /* 启动独立看门狗 */
    __HAL_IWDG_START(&hiwdg);
    return 0;
}

/**
 * @brief 喂狗操作，定时器重装初值
 * 
 */
void feed_dog()
{
#ifdef LOSCFG_WATCH_DOG
    HAL_IWDG_Refresh(&hiwdg);
#endif
}
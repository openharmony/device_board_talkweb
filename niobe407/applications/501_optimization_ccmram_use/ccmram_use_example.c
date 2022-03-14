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
#include "ohos_run.h"
#include "cmsis_os2.h"

#define TEST_BUFF_LEN  (1024*4)

__attribute__((section(".ccmram"))) unsigned int test_buff [TEST_BUFF_LEN] = {0};

__attribute__((section(".ccmram_func"))) void test_ccmram(void)
{
    for(int i=0; i<TEST_BUFF_LEN; i++) {
        test_buff[i] = i;   
    }
    
    if (test_buff[0] == 0 && test_buff[4095] == 4095)
        printf("---use CCMRAM buff success!\n");
    else
        printf("---use CCMRAM buff fail!\n");
}

void test_ccmram_example()
{
    test_ccmram();
}

OHOS_APP_RUN(test_ccmram_example);

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
#include "ohos_run.h"
#include "program_version.h"

void welcome_info(void)
{
    printf("*************************************************************\n");
    printf("\n");
    printf("████████╗ █████╗ ██╗     ██╗  ██╗██╗    ██╗███████╗██████╗\n");
    printf("╚══██╔══╝██╔══██╗██║     ██║ ██╔╝██║    ██║██╔════╝██╔══██╗\n");
    printf("   ██║   ███████║██║     █████╔╝ ██║ █╗ ██║█████╗  ██████╔╝\n");
    printf("   ██║   ██╔══██║██║     ██╔═██╗ ██║███╗██║██╔══╝  ██╔══██╗\n");
    printf("   ██║   ██║  ██║███████╗██║  ██╗╚███╔███╔╝███████╗██████╔╝\n");
    printf("   ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚══╝╚══╝ ╚══════╝╚═════╝\n");
    printf("\n");
    printf("         Welcome to use Niobe407 development board!\n");
    printf("             Hardware version: "APP_VERSION"\n");
    printf("             Software version: "HW_VERSION"\n");
    printf("*************************************************************\n");
}

void run_sys_before(void)
{
    welcome_info();

    /* Run code before openharmony run */
}

BEFORE_OHOS_RUN(run_sys_before);
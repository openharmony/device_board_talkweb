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

#ifndef _BSP_UART_H
#define _BSP_UART_H

#include "los_event.h"
#include "los_compiler.h"
#include <stdio.h>


extern UART_HandleTypeDef huart1;
extern EVENT_CB_S g_shellInputEvent;

extern VOID ShellUartInit(VOID);
extern INT32 UartPutc(INT32 c, VOID *file);
extern uint8_t UartGetc(VOID);

#endif

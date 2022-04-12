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

#ifndef _TARGET_CONFIG_H
#define _TARGET_CONFIG_H

#include "los_compiler.h"
#include "stm32f4xx.h"
/*=============================================================================
                                        System clock module configuration
=============================================================================*/
#define OS_SYS_CLOCK                                        168000000
#define LOSCFG_BASE_CORE_TICK_PER_SECOND                    (1000UL)
#define LOSCFG_BASE_CORE_TICK_HW_TIME                       0
#define LOSCFG_BASE_CORE_TICK_WTIMER                        0
#define LOSCFG_BASE_CORE_TICK_RESPONSE_MAX                  0xFFFFFFUL

/*=============================================================================
                                        Hardware interrupt module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_HWI                                 1
#define LOSCFG_USE_SYSTEM_DEFINED_INTERRUPT                 1
#define LOSCFG_PLATFORM_HWI_LIMIT                           128
#define LOSCFG_ARCH_HWI_VECTOR_ALIGN                        0x200
/*=============================================================================
                                       Task module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_TSK_LIMIT                          63
#define LOSCFG_BASE_CORE_TSK_IDLE_STACK_SIZE                (0x1000U)
#define LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE             (0x1000U)
#define LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE                 (ALIGN(0x80, 4))
#define LOSCFG_BASE_CORE_TIMESLICE                          1
#define LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT                  10
#define LOSCFG_BASE_CORE_TSK_MONITOR                        1
#define LOSCFG_BASE_CORE_EXC_TSK_SWITCH                     0

/*=============================================================================
                                       Semaphore module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_SEM                                 1
#define LOSCFG_BASE_IPC_SEM_LIMIT                           48
/*=============================================================================
                                       Mutex module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_MUX                                 1
#define LOSCFG_BASE_IPC_MUX_LIMIT                           32
/*=============================================================================
                                       Queue module configuration
=============================================================================*/
#define LOSCFG_BASE_IPC_QUEUE                               1
#define LOSCFG_BASE_IPC_QUEUE_LIMIT                         24
/*=============================================================================
                                       Software timer module configuration
=============================================================================*/
#define LOSCFG_BASE_CORE_SWTMR                              1
#define LOSCFG_BASE_CORE_SWTMR_ALIGN                        1
#define LOSCFG_BASE_CORE_SWTMR_LIMIT                        48
/*=============================================================================
                                       Memory module configuration
=============================================================================*/
extern unsigned int __los_heap_addr_start__;
extern unsigned int __los_heap_addr_end__;
#define LOSCFG_SYS_EXTERNAL_HEAP                            1
#define LOSCFG_SYS_HEAP_ADDR                                ((void *)&__los_heap_addr_start__)
#define LOSCFG_SYS_HEAP_SIZE (((unsigned long)&__los_heap_addr_end__) - ((unsigned long)&__los_heap_addr_start__))
#define LOSCFG_MEM_MUL_POOL                                 1
#define OS_SYS_MEM_NUM                                      20
#define LOSCFG_MEM_FREE_BY_TASKID                           1
#define LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK                1
#define LOSCFG_MEM_LEAKCHECK                                0

/*=============================================================================
                                       Exception module configuration
=============================================================================*/
#define LOSCFG_PLATFORM_EXC                                 0

#endif /* _TARGET_CONFIG_H */

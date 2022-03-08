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
#include "los_atomic.h"
#include "ohos_run.h"

#define ATOMIC_STACK_SIZE 0x1000
#define ATOMIC_TASK_NAME "atomic_test_task"
#define ATOMIC_TASK_PRIORITY 2
#define ATOMIC_TEST_LOOP 10
Atomic g_testAtomicID03 = 0;
Atomic64 g_testAtomicID05 = 0;
UINT32 volatile g_testCount = 0;
#define ATOMIC_TASK_NUM 3
#define DB_ATOMIC_TASK_NUM ATOMIC_TASK_NUM*2

uint32_t count = 0;

extern LosTaskCB *g_taskCBArray;
UINT32 AtomicTaskUsedCountGet(VOID)
{
    UINT32 intSave;
    UINT32 count = 0;

    intSave = LOS_IntLock();
    for (UINT32 index = 0; index < LOSCFG_BASE_CORE_TSK_LIMIT; index++) {
        LosTaskCB *taskCB = ((LosTaskCB *)g_taskCBArray) + index;
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            count++;
        }
    }
    LOS_IntRestore(intSave);

    return (LOSCFG_BASE_CORE_TSK_LIMIT - count);
}


#define STM32_ASSERT_EQUAL(param, value, retcode)      \
    do {                                                \
        if ((param) != (value)) {                       \
            return 1;                                   \
        }                                               \
    } while (0)

#define STM32_ASSERT_NOT_EQUAL(param, value, retcode)  \
    do {                                                \
        if ((param) == (value)) {                       \
            return 1;                                   \
        }                                               \
    } while (0)
#define STM32_ASSERT_EQUAL_VOID(param, value, retcode) \
    do {                                                \
        if ((param) != (value)) {                       \
            return;                                     \
        }                                               \
    } while (0)
#define STM32_GOTO_EQUAL(param, value, retcode, label) \
    do {                                                \
        if ((param) != (value)) {                       \
            goto label;                                 \
        }                                               \
    } while (0)

void AtomicTest1()
{
    printf("###start atomic test1\n");
    volatile INT32 value = 0;
    UINTPTR ret;

    printf("####start atomic test1\n");
    ret = LOS_AtomicDecRet(&value);
    STM32_ASSERT_EQUAL(value, ret, value);
    STM32_ASSERT_EQUAL(value, -1, value);

    ret = LOS_AtomicDecRet(&value);
    STM32_ASSERT_EQUAL(value, ret, value);
    STM32_ASSERT_EQUAL(value, -0x02, value);

    value = 0x7fffffff;
    ret = LOS_AtomicDecRet(&value);
    STM32_ASSERT_EQUAL(value, ret, value);
    STM32_ASSERT_EQUAL(value, 0x7ffffffe, value);

    value = -1;
    LOS_AtomicInc(&value);
    STM32_ASSERT_EQUAL(value, 0, value);

    value = 0x7ffffffe;
    LOS_AtomicInc(&value);
    STM32_ASSERT_EQUAL(value, 0x7fffffff, value);

    value = 0;
    LOS_AtomicDec(&value);
    STM32_ASSERT_EQUAL(value, -1, value);

    value = 0x7fffffff;
    LOS_AtomicDec(&value);
    STM32_ASSERT_EQUAL(value, 0x7ffffffe, value);

    printf("###end atomic test1\n" );

}

void AtomicTest2()
{
    printf("####start atomic test2\n");
    UINT32 newVal;
    volatile INT32 value = 0;
    UINTPTR ret;

    value = 0;
    newVal = 0xff;
    ret = LOS_AtomicXchg32bits(&value, newVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    newVal = 0xffff;
    ret = LOS_AtomicXchg32bits(&value, newVal);
    STM32_ASSERT_EQUAL(ret, 0xff, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    newVal = 0xffffff;
    ret = LOS_AtomicXchg32bits(&value, newVal);
    STM32_ASSERT_EQUAL(ret, 0xffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffff, value);

    newVal = 0xffffffff;
    ret = LOS_AtomicXchg32bits(&value, newVal);
    STM32_ASSERT_EQUAL(ret, 0xffffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffff, value);
    printf("###end atomic test2 \n" );

}

void AtomicTest3()
{
    printf("####start atomic test3\n");
    UINT32 oldVal;
    UINT32 newVal;
    volatile INT32 value;
    UINTPTR ret;
    newVal = 0xff;
    oldVal = 1;
    value = 0;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal); // 如果value != oldvalue 返回1，但是不会更新value 到 new value
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0, value);

    oldVal = 0;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    newVal = 0xffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    oldVal = 0xff;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    newVal = 0xffffffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    oldVal = 0xffff;
    ret = LOS_AtomicCmpXchg32bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffff, value);

    printf("###end atomic test3 \n");

}

void AtomicTest4()
{
    printf("####start atomic test4\n");
    g_testAtomicID03 = 0;
    g_testCount = 0;
    
    UINTPTR ret;
    INT32 i;
    ret = 0;
    UINTPTR count;
    for (i = 0; i < 100; ++i) { // run 100 times.
        count = g_testAtomicID03;
        ret = LOS_AtomicIncRet(&g_testAtomicID03);
        STM32_ASSERT_EQUAL_VOID(ret, g_testAtomicID03, ret);
        STM32_ASSERT_EQUAL_VOID((count + 1), g_testAtomicID03, (count + 1));
    }
    STM32_ASSERT_EQUAL_VOID(g_testAtomicID03, i, g_testAtomicID03);
    STM32_ASSERT_EQUAL_VOID(ret, g_testAtomicID03, ret);

    LOS_AtomicAdd(&g_testCount, 1);
    STM32_ASSERT_EQUAL(g_testCount, 1, g_testCount);
    printf("###end atomic test4 \n");

}

void AtomicTest5()
{
    printf("### start atomic test5\n");
    volatile INT64 value = 0;
    UINT64 ret;
    UINT64 uwNewVal;

    uwNewVal = 0xff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    uwNewVal = 0xffff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0xff, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    uwNewVal = 0xffffff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0xffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffff, value);

    uwNewVal = 0xffffffff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0xffffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffff, value);

    uwNewVal = 0xffffffffffff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0xffffffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffffffff, value);

    uwNewVal = 0xffffffffffffffff;
    ret = LOS_AtomicXchg64bits(&value, uwNewVal);
    STM32_ASSERT_EQUAL(ret, 0xffffffffffff, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffffffffffff, value);
    printf("###end atomic test5 \n");

}

void AtomicTest6()
{
    printf("### start atomic test6\n");
    volatile INT64 value = 0;
    BOOL ret;
    UINT64 newVal = 0xff;
    UINT64 oldVal = 1;

    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0, value);

    oldVal = 0;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    newVal = 0xffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xff, value);

    oldVal = 0xff;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    newVal = 0xffffffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xffff, value);

    oldVal = 0xffff;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffff, value);

    newVal = 0xffffffffffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffff, value);

    oldVal = 0xffffffff;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffffffff, value);

    newVal = 0xffffffffffffffff;
    oldVal = 1;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 1, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffffffff, value);

    oldVal = 0xffffffffffff;
    ret = LOS_AtomicCmpXchg64bits(&value, newVal, oldVal);
    STM32_ASSERT_EQUAL(ret, 0, ret);
    STM32_ASSERT_EQUAL(value, 0xffffffffffffffff, value);
    printf("###end atomic test6 \n");

}
static VOID Task7Entry(VOID)
{
    INT64 i;

    for (i = 0x7fffffffffff0000; i < 0x7fffffffffff000f; ++i) {
        LOS_AtomicXchg64bits(&g_testAtomicID05, i);
    }

    ++g_testCount;
}

void AtomicTest7()
{
    printf("### start atomic test7\n");
    UINT32 ret, i;
    UINT32 taskId[ATOMIC_TASK_NUM];
    TSK_INIT_PARAM_S task[ATOMIC_TASK_NUM] = {0, };
    CHAR taskName[ATOMIC_TASK_NUM][20] = {"", }; // max taskName size is 20.
    CHAR buf[10] = ""; // max buf size is 10.

    g_testCount = 0;
    g_testAtomicID05 = 0;

    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        memset(buf, 0, 10); // max buf size is 10.
        memset(taskName[i], 0, 20); // max taskName size is 20.

        task[i].pfnTaskEntry = (TSK_ENTRY_FUNC)Task7Entry;
        task[i].pcName       = taskName[i];
        task[i].uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
        task[i].usTaskPrio   = 23; // TASK_PRIO_TEST - 2 has higher priority than TASK_PRIO_TEST
        task[i].uwResved     = 0x0100;
    }
    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        ret = LOS_TaskCreate(&taskId[i], &task[i]);
        STM32_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }
    LOS_TaskDelay(20); // delay 20 ticks.

    STM32_GOTO_EQUAL(g_testCount, ATOMIC_TASK_NUM, g_testCount, EXIT);
    STM32_GOTO_EQUAL(g_testAtomicID05, 0x7fffffffffff000e, g_testAtomicID05, EXIT);
    printf("### end atomic test7\n");
    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        (VOID)LOS_TaskDelete(taskId[i]);
    }
EXIT:
    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        (VOID)LOS_TaskDelete(taskId[i]);
    }
}

static void Task8Entry(VOID)
{
    INT64 i;
    INT64 count;
    for (i = 0x7fffffffffff0000; i < 0x7fffffffffffffff; ++i) {
        count = g_testAtomicID05;
        LOS_AtomicCmpXchg64bits(&g_testAtomicID05, i, count);
    }

    ++g_testCount;

}

void AtomicTest8()
{
    printf("### start atomic test8\n");
    UINT32 ret, i;
    UINT32 taskId[ATOMIC_TASK_NUM];
    TSK_INIT_PARAM_S task[ATOMIC_TASK_NUM] = {0, };
    CHAR taskName[ATOMIC_TASK_NUM][20] = {"", }; // max taskName size is 20.
    CHAR buf[10] = ""; // max buf size is 10.

    g_testCount = 0;
    g_testAtomicID05 = 0;

    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        memset(buf, 0, 10); // max buf size is 10.
        memset(taskName[i], 0, 20); // max taskName size is 20.

        task[i].pfnTaskEntry = (TSK_ENTRY_FUNC)Task8Entry;
        task[i].pcName       = taskName[i];
        task[i].uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
        task[i].usTaskPrio   = 23; // TASK_PRIO_TEST - 2 has higher priority than TASK_PRIO_TEST
        task[i].uwResved     = 0x0100;
    }

    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        ret = LOS_TaskCreate(&taskId[i], &task[i]);
        STM32_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    LOS_TaskDelay(80); // delay 80 ticks.
    STM32_GOTO_EQUAL(g_testCount, ATOMIC_TASK_NUM, g_testCount, EXIT);
    STM32_GOTO_EQUAL(g_testAtomicID05, (0x7fffffffffffffff - 1), g_testAtomicID05, EXIT);
    printf("### end atomic test8\n");
    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        (VOID)LOS_TaskDelete(taskId[i]);
    }
EXIT:
    for (i = 0; i < ATOMIC_TASK_NUM; i++) {
        (VOID)LOS_TaskDelete(taskId[i]);
    }

}
static VOID TaskTest901(VOID)
{
    UINT32 i;
    for (i = 0; i < 10; ++i) { // run 10 times.
        LOS_Atomic64Dec(&g_testAtomicID05);
    }

    ++g_testCount;
}

static VOID TaskTest902(VOID)
{
    UINT64 i;
    for (i = 0; i < 10; ++i) { // run 10 times.
        LOS_AtomicCmpXchg64bits(&g_testAtomicID05, g_testAtomicID05, g_testAtomicID05);
    }

    ++g_testCount;
}

void AtomicTest9()
{
    printf("### start atomic test9\n");
    UINT32 ret, i;
    UINT32 taskId[DB_ATOMIC_TASK_NUM];
    TSK_INIT_PARAM_S task[DB_ATOMIC_TASK_NUM] = {0, };
    CHAR taskName[DB_ATOMIC_TASK_NUM][20] = {"", }; // max taskName size is 20.
    CHAR buf[10] = ""; // max buf size is 10.
    Atomic uCount = 0;
    g_testCount = 0;
    g_testAtomicID05 = 0xffffffffff;
    UINT32 uLoop = g_taskMaxNum - AtomicTaskUsedCountGet();
    printf("max task is %d free thread num is %d\n",g_taskMaxNum, uLoop);
    if (uLoop > DB_ATOMIC_TASK_NUM) {
        uLoop = DB_ATOMIC_TASK_NUM;
    }

    for (i = 0; i < uLoop; i++) {
        memset(buf, 0, 10); // max buf size is 10.
        memset(taskName[i], 0, 20); // max taskName size is 20.

        if (i % 2 == 0) { // 2 is index.
            uCount++;
            task[i].pfnTaskEntry = (TSK_ENTRY_FUNC)TaskTest901;
        } else {
            task[i].pfnTaskEntry = (TSK_ENTRY_FUNC)TaskTest902;
        }
        task[i].pcName       = taskName[i];
        task[i].uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
        task[i].usTaskPrio   = 23; // TASK_PRIO_TEST - 2 has higher priority than TASK_PRIO_TEST
        task[i].uwResved     = 0x0100;
    }
    for (i = 0; i < uLoop; i++) {
        ret = LOS_TaskCreate(&taskId[i], &task[i]);
        STM32_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    LOS_TaskDelay(20); // delay 20 ticks.
    STM32_GOTO_EQUAL(g_testCount, uLoop, g_testCount, EXIT);
    STM32_GOTO_EQUAL(g_testAtomicID05, (0xffffffffff - 10 * uCount), g_testAtomicID05, EXIT);  // run 10 times.
    printf("### end of atomic test9\n");
EXIT:
    for (i = 0; i < uLoop; i++) {
        (VOID)LOS_TaskDelete(taskId[i]);
    }
}
static void* AtomicTaskF10()
{
    INT64 i;

    for (i = 0; i < ATOMIC_TEST_LOOP; ++i) {
        LOS_Atomic64Inc(&g_testAtomicID05);
    }

    ++g_testCount;
}

UINT32 AtomicTest10()
{
    printf("### start of atomic test10\n");
    UINT32 ret;
    g_testAtomicID05 = 0;
    g_testCount = 0;
    UINT32 task10 = 0;

    TSK_INIT_PARAM_S stTask1 = {0};
    stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)AtomicTaskF10;
    stTask1.pcName       = "AtomicTaskF10";
    stTask1.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask1.usTaskPrio   = 23; // TASK_PRIO_TEST - 2 has higher priority than TASK_PRIO_TEST
    stTask1.uwResved     = 0x0100;

    ret = LOS_TaskCreate(&task10, &stTask1);
    STM32_ASSERT_EQUAL(ret, LOS_OK, ret);
    LOS_TaskDelay(20); // delay 20 ticks.

    STM32_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);
    STM32_GOTO_EQUAL(g_testAtomicID05, ATOMIC_TEST_LOOP, g_testAtomicID05, EXIT);
    printf("### end of atomic test10\n");
EXIT:
    LOS_TaskDelete(task10);
    return LOS_OK;

}

static void* AtomicTest11Entry(void* arg)
{
    printf("### start of atomic test11\n");
    Atomic atomicTestCounter = 0;
    INT32 readCounter = 0;
    INT32 atomicRet = 0;
    
    Atomic64 atomic64TestCounter = 0;
    INT64 read64Counter = 0;
    INT64 atomic64Ret = 0;

    STM32_ASSERT_EQUAL(g_testCount, 0, g_testCount);
    
    atomicTestCounter = 0x88888888;
    LOS_AtomicSet(&atomicTestCounter, 0x88888888);
    atomicRet = LOS_AtomicAdd(&atomicTestCounter, 0x33333333);
    STM32_ASSERT_EQUAL(atomicRet, 0xbbbbbbbb, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0xbbbbbbbb, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0xaaaaaaaa);
    atomicRet = LOS_AtomicAdd(&atomicTestCounter, 0x77777777);
    STM32_ASSERT_EQUAL(atomicRet, 0x22222221, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x22222221, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0xdddddddd);
    atomicRet = LOS_AtomicSub(&atomicTestCounter, 0x99999999);
    STM32_ASSERT_EQUAL(atomicRet, 0x44444444, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x44444444, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x88888888);
    atomicRet = LOS_AtomicSub(&atomicTestCounter, 0xdddddddd);
    STM32_ASSERT_EQUAL(atomicRet, 0xaaaaaaab, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0xaaaaaaab, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x11111111);
    LOS_AtomicInc(&atomicTestCounter);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x11111112, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0xffffffff);
    LOS_AtomicInc(&atomicTestCounter);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x00000000, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x88888888);
    atomicRet = LOS_AtomicIncRet(&atomicTestCounter);
    STM32_ASSERT_EQUAL(atomicRet, 0x88888889, atomicRet);

    LOS_AtomicSet(&atomicTestCounter, 0xffffffff);
    atomicRet = LOS_AtomicIncRet(&atomicTestCounter);
    STM32_ASSERT_EQUAL(atomicRet, 0x00000000, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x00000000, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x66666666);
    LOS_AtomicDec(&atomicTestCounter);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x66666665, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x00000000);
    LOS_AtomicDec(&atomicTestCounter);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0xffffffff, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x88888888);
    atomicRet = LOS_AtomicDecRet(&atomicTestCounter);
    STM32_ASSERT_EQUAL(atomicRet, 0x88888887, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0x88888887, readCounter);

    LOS_AtomicSet(&atomicTestCounter, 0x00000000);
    atomicRet = LOS_AtomicDecRet(&atomicTestCounter);
    STM32_ASSERT_EQUAL(atomicRet, 0xffffffff, atomicRet);
    readCounter = LOS_AtomicRead(&atomicTestCounter);
    STM32_ASSERT_EQUAL(readCounter, 0xffffffff, readCounter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Add(&atomic64TestCounter, 0x0000000033333333);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaaabbbbbbbb, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaabbbbbbbb, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Add(&atomic64TestCounter, 0x0000123456789abc);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaabcdedf012344, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaabcdedf012344, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0xaaaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Add(&atomic64TestCounter, 0x88888888aaaaaaaa);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x3333333333333332, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x3333333333333332, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Sub(&atomic64TestCounter, 0x0000000033333333);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaaa55555555, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaa55555555, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Sub(&atomic64TestCounter, 0x00001234cba98765);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaa9875bcdf0123, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaa9875bcdf0123, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64Sub(&atomic64TestCounter, 0x60001234cba98765);
    STM32_ASSERT_EQUAL(atomic64Ret, 0xfaaa9875bcdf0123, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0xfaaa9875bcdf0123, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    LOS_Atomic64Inc(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaa88888889, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaaffffffff);
    LOS_Atomic64Inc(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaab00000000, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0xffffffffffffffff);
    LOS_Atomic64Inc(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x0000000000000000, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64IncRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaaa88888889, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaa88888889, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaaffffffff);
    atomic64Ret = LOS_Atomic64IncRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaab00000000, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaab00000000, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0xffffffffffffffff);
    atomic64Ret = LOS_Atomic64IncRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x0000000000000000, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x0000000000000000, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    LOS_Atomic64Dec(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaa88888887, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa00000000);
    LOS_Atomic64Dec(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaa9ffffffff, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x0000000000000000);
    LOS_Atomic64Dec(&atomic64TestCounter);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0xffffffffffffffff, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa88888888);
    atomic64Ret = LOS_Atomic64DecRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaaa88888887, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaaa88888887, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x5aaaaaaa00000000);
    atomic64Ret = LOS_Atomic64DecRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0x5aaaaaa9ffffffff, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0x5aaaaaa9ffffffff, read64Counter);

    LOS_Atomic64Set(&atomic64TestCounter, 0x0000000000000000);
    atomic64Ret = LOS_Atomic64DecRet(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(atomic64Ret, 0xffffffffffffffff, atomic64Ret);
    read64Counter = LOS_Atomic64Read(&atomic64TestCounter);
    STM32_ASSERT_EQUAL(read64Counter, 0xffffffffffffffff, read64Counter);

    ++g_testCount;
    printf("### end of atomic test11\n");

}
void AtomicTest11()
{
    UINT32 ret;
    g_testCount = 0;
    UINT32 task11 = 0;

    TSK_INIT_PARAM_S stTask1 = {0};
    stTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)AtomicTest11Entry;
    stTask1.pcName       = "Atomic_011";
    stTask1.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    stTask1.usTaskPrio   = 23; // TASK_PRIO_TEST - 2 has higher priority than TASK_PRIO_TEST
    stTask1.uwResved     = 0x0100;

    ret = LOS_TaskCreate(&task11, &stTask1);
    STM32_ASSERT_EQUAL(ret, LOS_OK, ret);
    LOS_TaskDelay(20); // delay 20 ticks.

    STM32_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);
EXIT:
    LOS_TaskDelete(task11);
    return LOS_OK;

}
static void* AtomicTestEntryAll(void* arg)
{
    AtomicTest1();
    AtomicTest2();
    AtomicTest3();
    AtomicTest4();
    AtomicTest5();
    AtomicTest6();
    AtomicTest7();
    AtomicTest8();
    AtomicTest9();
    AtomicTest10();
    AtomicTest11();
}

static void KernelAtomicAllAssert(void)
{
    UINT32 uwRet;
    UINT32 taskID;
    TSK_INIT_PARAM_S stTask = {0};

    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)AtomicTestEntryAll;
    stTask.uwStackSize = ATOMIC_STACK_SIZE;
    stTask.pcName = ATOMIC_TASK_NAME;
    stTask.usTaskPrio = ATOMIC_TASK_PRIORITY; /* Os task priority is 6 */
    uwRet = LOS_TaskCreate(&taskID, &stTask);
    if (uwRet != LOS_OK) {
        printf("Task1 create failed\n");
    }
}

OHOS_APP_RUN(KernelAtomicAllAssert);
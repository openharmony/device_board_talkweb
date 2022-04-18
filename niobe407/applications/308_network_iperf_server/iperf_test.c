/*
* Copyright (c) 2022 Talkweb Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include <stdio.h>
#include "cmsis_os2.h"
#include "app_ethernet.h"
#include "ohos_run.h"

osThreadId_t iperf_test_id = NULL;
static unsigned char iperf_run_flag = 0;

#define LOCAL_PORT        1234

EthLinkInfo gEthLinkInfo;

void iperf_test(void *arg)
{
    (void *)arg;
    get_ethernet_link_info(&gEthLinkInfo);
    printf("lwiperf_start_tcp_server IP:%s port:%d\n", ipaddr_ntoa(&gEthLinkInfo.ipaddr), LOCAL_PORT);
    lwiperf_start_tcp_server(&gEthLinkInfo.ipaddr, LOCAL_PORT, NULL, NULL);
    printf("lwiperf tcp_server is running...\n");
    iperf_run_flag = 1;
    while (iperf_run_flag) {
        osDelay(1000);
    }
}

static void eth_enable_state_callBack(EthLinkState state)
{
    static int net_init_finish = 0;
    if (state == STATE_UPDATE_LINK_DOWN) {
        iperf_run_flag = 0;
        osThreadTerminate(iperf_test_id);
        iperf_test_id = NULL;
        printf("ETH LINK STATE: DisConnected!\r\n");
    } else if (state == STATE_UPDATE_LINK_UP) {
        printf("ETH LINK STATE: Connected!\r\n");
        if (net_init_finish == 0) {
            osThreadAttr_t attr;
            attr.name = "iperf_test";
            attr.attr_bits = 0U;
            attr.cb_mem = NULL;
            attr.cb_size = 0U;
            attr.stack_mem = NULL;
            attr.stack_size = 1024 * 4;
            attr.priority = 25;
            iperf_test_id = osThreadNew((osThreadFunc_t)iperf_test, NULL, &attr);
            if (iperf_test_id == NULL) {
                printf("Falied to create iperf_test thread!\n");
            }
            net_init_finish = 1;
        }
    }
}

void iperf_test_example(void)
{
    ethernet_enable(eth_enable_state_callBack);
}

OHOS_APP_RUN(iperf_test_example);
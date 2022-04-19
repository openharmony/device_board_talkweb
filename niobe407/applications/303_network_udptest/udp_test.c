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

osThreadId_t udp_test_id = NULL;
static struct udp_pcb *g_upcb = NULL;
static unsigned char udp_rcv_flag = 0;

#define LOCAL_PORT        1234
#define DESTINATION_PORT  5678
#define DESTINATION_IP    "192.168.8.119"

void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    (void *)arg;
    if (p != NULL) {
        char *recdata = (char *)malloc(p->len*sizeof(char)+1);
        if (recdata != NULL) {
            if (udp_rcv_flag != 1) {
                udp_rcv_flag = 1;
            }
            memcpy_s(recdata, p->len, p->payload, p->len);
            recdata[p->len] = '\0';
            printf("udp recv from[%d.%d.%d.%d:%d]: %s\n", *((uint8_t *)&addr->addr), *((uint8_t *)&addr->addr + 1),
                *((uint8_t *)&addr->addr + 2), *((uint8_t *)&addr->addr + 3), port, recdata);
            free(recdata);
            udp_send(upcb, p);
        }
        pbuf_free(p);
    }
}

void udp_client_send(char *pData)
{
    struct pbuf *p;
    p = pbuf_alloc(PBUF_TRANSPORT, strlen(pData), PBUF_POOL);
    if (p != NULL) {
        pbuf_take(p, pData, strlen(pData));
        udp_send(g_upcb, p);
        pbuf_free(p);
    }
}

void udp_test(void *arg)
{
    (void)arg;
    ip_addr_t serverIP;
    err_t err;
    ipaddr_aton(DESTINATION_IP, &serverIP);
    g_upcb = udp_new();
    if (g_upcb != NULL) {
        g_upcb->local_port = LOCAL_PORT;
        err = udp_connect(g_upcb, &serverIP, DESTINATION_PORT);
        if (err == ERR_OK) {
            udp_recv(g_upcb, udp_receive_callback, NULL);
            printf("udp client connected\r\n");
        } else {
            udp_remove(g_upcb);
            printf("can not connect udp pcb.\r\n");
        }
    }
    while (!udp_rcv_flag) {
        printf("wait recv udp data...\n");
        osDelay(1000);
    }
}

static void eth_enable_state_callBack(EthLinkState state)
{
    static int net_init_finish = 0;
    if (state == STATE_UPDATE_LINK_DOWN) {
        printf("ETH LINK STATE: DisConnected!\r\n");
    } else if (state == STATE_UPDATE_LINK_UP) {
        printf("ETH LINK STATE: Connected!\r\n");
        if (net_init_finish == 0) {
            osThreadAttr_t attr;
            attr.name = "udp_test";
            attr.attr_bits = 0U;
            attr.cb_mem = NULL;
            attr.cb_size = 0U;
            attr.stack_mem = NULL;
            attr.stack_size = 1024 * 4;
            attr.priority = 25;
            udp_test_id = osThreadNew((osThreadFunc_t)udp_test, NULL, &attr);
            if (udp_test_id == NULL) {
                printf("Falied to create udp_test thread!\n");
            }
            net_init_finish = 1;
        }
    }
}

void udp_test_example(void)
{
    ethernet_enable(eth_enable_state_callBack);
}

OHOS_APP_RUN(udp_test_example);
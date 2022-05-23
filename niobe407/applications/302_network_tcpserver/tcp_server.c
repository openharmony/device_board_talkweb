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
#include "app_ethernet.h"
#include "cmsis_os2.h"
#include "ohos_run.h"


#define TCP_SERVER_PRIORITY 25
#define TCP_SERVER_STACK_SIZE 4096
#define TCP_SERVER_DELAY 200
#define TCP_SERVER_RCV_DATA_SIZE 512

#define SERVER_LISTEN_PORT 8080
#define TCP_BACKLOG 5
#define RECV_BUFFER_MAX 511

osThreadId_t tcp_server_id = NULL;

void tcp_server(void *argument)
{
    (void *)argument;
    int sock, connected;
    char recv_data[TCP_SERVER_RCV_DATA_SIZE] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    int recv_data_len;

    printf("start tcp_server test\r\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket error\n");
        goto __exit;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_LISTEN_PORT);
    memset_s(&(server_addr.sin_zero), sizeof(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Unable to bind\n");
        goto __exit;
    }

    if (listen(sock, TCP_BACKLOG) == -1) {
        printf("Listen error\n");
        goto __exit;
    }

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        printf("new client connected from (%s, %d)\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        int flag = 1;
        setsockopt(connected, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));
        while (1) {
            recv_data_len = recv(connected, recv_data, RECV_BUFFER_MAX, 0);
            if (recv_data_len <= 0) {
                break;
            } else {
                recv_data[recv_data_len] = '\0';
            }

            printf("recv %s\n", recv_data);
            write(connected, recv_data, recv_data_len);

            osDelay(TCP_SERVER_DELAY);
        }
        if (connected >= 0) {
            closesocket(connected);
        }

        connected = -1;
    }
__exit:
    if (sock >= 0) {
        closesocket(sock);
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
            attr.name = "tcp_server";
            attr.attr_bits = 0U;
            attr.cb_mem = NULL;
            attr.cb_size = 0U;
            attr.stack_mem = NULL;
            attr.stack_size = TCP_SERVER_STACK_SIZE;
            attr.priority = TCP_SERVER_PRIORITY;
            tcp_server_id = osThreadNew((osThreadFunc_t)tcp_server, NULL, &attr);
            if (tcp_server_id == NULL) {
                printf("Failed to create tcp_server thread!\n");
            }
            net_init_finish = 1;
        }
    }
}

void tcp_server_example(void)
{
    ethernet_enable(eth_enable_state_callBack);
}

OHOS_APP_RUN(tcp_server_example);
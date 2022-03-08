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
#include <stdio.h>
#include "cmsis_os2.h"
#include "ohos_run.h"
#include "app_ethernet.h"

#define SERVER_IP "192.168.8.119"
#define SERVER_PORT 8080

osThreadId_t tcp_client_id = NULL;

void tcp_client(void *thread_param)
{
    struct sockaddr_in client_addr;
    char recv_data[512] = {0};
    int recv_data_len;

    while (1)
    {
        /* 申请套接字，本质是netconn_new函数的封装 */
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            printf("Socket error\n");
            osDelay(100);
            continue;
        }

        /* 清空sockaddr_in结构体内存空间 */
        memset(&(client_addr), 0, sizeof(client_addr));

        /* 为sockaddr_in结构体成员赋值，用于以下的connect绑定 */
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(SERVER_PORT);
        client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        printf("try connect to server " SERVER_IP ":%d\n", SERVER_PORT);
        /* 将远端server的ip地址与端口进行绑定 */
        if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1)
        {
            closesocket(sock);
            osDelay(1000);
            continue;
        }

        printf("Connect to tcp server successful!\n");
        while (1)
        {
            /* 成功接收到数据，返回接收的数据长度 */
            recv_data_len = recv(sock, recv_data, 511, 0);
            if (recv_data_len <= 0)
                break;
            else
                recv_data[recv_data_len] = '\0';

            /* 串口打印接收的数据内容 */
            printf("recv:%s\n", recv_data);
            /* 发送数据内容 */
            write(sock, recv_data, recv_data_len);
        }
    }
}

static void eth_enable_state_callBack(EthLinkState state)
{
    /* ETH连接断开*/
    if (state == STATE_UPDATE_LINK_DOWN)
    {
        printf("ETH LINK STATE: DisConnected!\r\n");
        osThreadTerminate(tcp_client_id);
        tcp_client_id = NULL;
    }
    /* ETH连接成功*/
    else if (state == STATE_UPDATE_LINK_UP)
    {
        printf("ETH LINK STATE: Connected!\r\n");

        osThreadAttr_t attr;
        attr.name = "tcp_client";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = 1024 * 4;
        attr.priority = 25;
        tcp_client_id = osThreadNew((osThreadFunc_t)tcp_client, NULL, &attr);
        if (tcp_client_id == NULL)
        {
            printf("Falied to create tcp_client thread!\n");
        }
    }
}

/**
 *当需要使用静态IP时， 将useStaticIp设置成1，补充相应的IP参数。
 *当需要使用静态MAC时，将useStaticMac设置成1，补充MAC参数。
 *当使用DHCP和随机MAC地址时，无需设置参数，直接调用ethernet_enable()即可。
 *此例程使用的是静态IP和静态MAC地址的模式
 **/
void tcp_client_example(void)
{
    EthLinkInfo info = {
        .useStaticIp = 1,
        .ipaddr.u8_addr = {192, 168, 8, 200},
        .netmask.u8_addr = {255, 255, 255, 0},
        .gw.u8_addr = {192, 168, 8, 1},

        .useStaticMac = 1,
        .macAddr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01}
    };

    set_ethernet_link_info(&info);
    ethernet_enable(eth_enable_state_callBack); //有线网络使能
}

OHOS_APP_RUN(tcp_client_example);
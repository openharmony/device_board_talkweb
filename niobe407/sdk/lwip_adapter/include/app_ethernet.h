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

#ifndef _APP_ETHERNET_H
#define _APP_ETHERNET_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"
#include "lwip/udp.h"

/* DHCP连接超时(单位:秒)*/
#define DHCP_TIMEOUT_S 30

/* ETH配置检测线程优先级*/
#define ETH_THREAD_PRIORITY 5

typedef union
{
    ip_addr_t u32_addr;
    unsigned char u8_addr[4];
} IPUnion;

/* ETH状态回调枚举*/
typedef enum
{
    STATE_UPDATE_LINK_DOWN = 0, // 网络连接状态更新(连接->断开)
    STATE_UPDATE_LINK_UP = 1,   // 网络连接状态更新(断开->连接)
} EthLinkState;

/* ETH PHY LINK相关参数*/
typedef struct
{
    unsigned char useStaticIp; // 1= 使用静态IP，0= 使用DHCP功能
    IPUnion ipaddr;            //当使用静态IP时有效，为静态IP地址
    IPUnion netmask;           //当使用静态IP时有效，为静态子网掩码
    IPUnion gw;                //当使用静态IP时有效，为静态默认网关

    unsigned char useStaticMac; // 1=使用静态MAC地址， 0=使用库随机生成的MAC地址
    unsigned char macAddr[6];   //当使用静态MAC地址时有效
} EthLinkInfo;

// 获取ETH PHY LINK相关参数
void get_ethernet_link_info(EthLinkInfo *info);

// 设置ETH PHY LINK相关参数
void set_ethernet_link_info(EthLinkInfo *info);

/* ETH PHY状态更新回调函数*/
typedef void (*eth_state_callBack)(EthLinkState state);

/* ETH使能函数*/
void ethernet_enable(eth_state_callBack callback);

#endif

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

#ifndef _APP_ETHERNET_H
#define _APP_ETHERNET_H

#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/dhcp.h"
#include "lwip/udp.h"


#define DHCP_TIMEOUT_S 30

#define ETH_THREAD_PRIORITY 5

typedef union {
    ip_addr_t u32_addr;
    unsigned char u8_addr[4];
} IPUnion;

typedef enum {
    STATE_UPDATE_LINK_DOWN = 0,
    STATE_UPDATE_LINK_UP = 1,
} EthLinkState;

typedef struct {
    unsigned char useStaticIp;
    IPUnion ipaddr;
    IPUnion netmask;
    IPUnion gw;
    unsigned char useStaticMac;
    unsigned char macAddr[6];
} EthLinkInfo;

void get_ethernet_link_info(EthLinkInfo *info);

void set_ethernet_link_info(EthLinkInfo *info);

typedef void (*eth_state_callBack)(EthLinkState state);

void ethernet_enable(eth_state_callBack callback);

#endif

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

#include "app_ethernet.h"

#define DO_PHY_STATUS_CALLBACK(func, val) \
    {                                     \
        do {                              \
            if (func != NULL) {           \
                func(val);                \
            }                             \
        } while (0);                      \
    }

static EthLinkInfo gEthLinkInfo = {
    .useStaticIp = 0,
    .useStaticMac = 0};

void get_ethernet_link_info(EthLinkInfo *info)
{
    if (info == NULL) {
        ETH_DEBUG("get eth info fail, param is null.\r\n");
        return;
    }
    memcpy(info, &gEthLinkInfo, sizeof(gEthLinkInfo));
}

void set_ethernet_link_info(EthLinkInfo *info)
{
    if (info == NULL) {
        ETH_DEBUG("set eth info fail, param is null.\r\n");
        return;
    }
    memcpy(&gEthLinkInfo, info, sizeof(gEthLinkInfo));
}

static void ethernet_staticIP_config(void)
{
    ETH_DEBUG("use static IP:\r\n");
    ETH_DEBUG("ip........................%d.%d.%d.%d\r\n", gEthLinkInfo.ipaddr.u8_addr[0],gEthLinkInfo.ipaddr.u8_addr[1], gEthLinkInfo.ipaddr.u8_addr[2], gEthLinkInfo.ipaddr.u8_addr[3]);
    ETH_DEBUG("netmask...................%d.%d.%d.%d\r\n", gEthLinkInfo.netmask.u8_addr[0], gEthLinkInfo.netmask.u8_addr[1], gEthLinkInfo.netmask.u8_addr[2], gEthLinkInfo.netmask.u8_addr[3]);
    ETH_DEBUG("gw........................%d.%d.%d.%d\r\n", gEthLinkInfo.gw.u8_addr[0], gEthLinkInfo.gw.u8_addr[1], gEthLinkInfo.gw.u8_addr[2], gEthLinkInfo.gw.u8_addr[3]);
}

static void ethernet_dhcpReslut_get(void)
{
    ETH_DEBUG("ip........................%u.%u.%u.%u\r\n", (gNetif.ip_addr.addr >> 0) & 0XFF, (gNetif.ip_addr.addr >> 8) & 0XFF, (gNetif.ip_addr.addr >> 16) & 0XFF, (gNetif.ip_addr.addr >> 24) & 0XFF);
    ETH_DEBUG("netmask...................%u.%u.%u.%u\r\n", (gNetif.netmask.addr >> 0) & 0XFF, (gNetif.netmask.addr >> 8) & 0XFF, (gNetif.netmask.addr >> 16) & 0XFF, (gNetif.netmask.addr >> 24) & 0XFF);
    ETH_DEBUG("gw........................%u.%u.%u.%u\r\n", (gNetif.gw.addr >> 0) & 0XFF, (gNetif.gw.addr >> 8) & 0XFF, (gNetif.gw.addr >> 16) & 0XFF, (gNetif.gw.addr >> 24) & 0XFF);
    gEthLinkInfo.ipaddr.u32_addr = gNetif.ip_addr;
    gEthLinkInfo.netmask.u32_addr = gNetif.netmask;
    gEthLinkInfo.gw.u32_addr = gNetif.gw;
}

static void user_dhcp_wait(void)
{
    dhcp_start(&gNetif);
    ETH_DEBUG("dhcp start...\r\n");

    unsigned int timeout = DHCP_TIMEOUT_S;
    gNetif.ip_addr.addr = 0;
    do {
        osDelay(1000);
        if (--timeout <= 0) {
            break;
        }
    } while (gNetif.ip_addr.addr == 0);

    if (gNetif.ip_addr.addr != 0) {
        ETH_DEBUG("dhcp success:\r\n");
        ethernet_dhcpReslut_get();
    } else {
        dhcp_stop(&gNetif);
        ETH_DEBUG("dhcp fail!\r\n");
        ethernet_staticIP_config();
        netif_set_addr(&gNetif, &gEthLinkInfo.ipaddr.u32_addr, &gEthLinkInfo.netmask.u32_addr, &gEthLinkInfo.gw.u32_addr);
    }
}

static void ethernet_thread_task(void *param)
{
    ETH_DEBUG("start ethernet config task...\r\n");
    tcpip_init(NULL, NULL);

    if (gEthLinkInfo.useStaticIp == 1) {
        ethernet_staticIP_config();
    }

    netif_add(&gNetif, &gEthLinkInfo.ipaddr.u32_addr, &gEthLinkInfo.netmask.u32_addr, &gEthLinkInfo.gw.u32_addr, NULL, &ethernetif_init, &ethernet_input);

    while ((gNetif.flags & NETIF_FLAG_LINK_UP) != NETIF_FLAG_LINK_UP) {
        ETH_DEBUG("Please check the network port!\r\n");
        ethernetif_init(&gNetif);
        osDelay(2000);
    }
    netif_set_default(&gNetif);

    if (netif_is_link_up(&gNetif)) {
        netif_set_up(&gNetif);
    } else {
        netif_set_down(&gNetif);
    }

    if (gEthLinkInfo.useStaticIp == 0) {
        user_dhcp_wait();
    }

    unsigned int regval = 0;
    unsigned char status = 0;
    eth_state_callBack callback = (eth_state_callBack)param;

    for (;;) {
        /* get phy link status */
        HAL_ETH_ReadPHYRegister(&gEthHandle, PHY_BSR, &regval);
        if (status != (regval & PHY_LINKED_STATUS)) {
            status = regval & PHY_LINKED_STATUS;
            if (status == 0) {
                netif_set_down(&gNetif);
                DO_PHY_STATUS_CALLBACK(callback, STATE_UPDATE_LINK_DOWN);
            } else {
                netif_set_up(&gNetif);
                DO_PHY_STATUS_CALLBACK(callback, STATE_UPDATE_LINK_UP);
            }
        }
        osDelay(1000);
    }
}

void ethernet_enable(eth_state_callBack callback)
{
    osThreadAttr_t attr = {.name = "ethernet_thread_task", .attr_bits = 0U, .cb_mem = NULL, .stack_mem = NULL, .stack_size = 4096, .priority = ETH_THREAD_PRIORITY};
    if (osThreadNew((osThreadFunc_t)ethernet_thread_task, (void *)callback, &attr) == NULL) {
        ETH_DEBUG("Create ethernet_thread_task failed!\r\n");
    }
}

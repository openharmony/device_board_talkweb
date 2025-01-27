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

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include <string.h>
#include "cmsis_os2.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_eth.h"

#define ETH_DEBUG(format, ...) printf("[%s] "format"", __FUNCTION__, ##__VA_ARGS__)

extern struct netif gNetif;
extern ETH_HandleTypeDef gEthHandle;

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(void const *argument);
struct pbuf *low_level_input(struct netif *netif);
void USER_IRQHandler(void);
void getmac(uint8_t *pMacBuf);

#endif

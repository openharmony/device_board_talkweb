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

#include "ethernetif.h"
#include "app_ethernet.h"
#include "los_interrupt.h"
#include "los_tick.h"

#define GPIO_AFx_ETH GPIO_AF11_ETH
#define ETH_RCC_CLK_ENABLE() __HAL_RCC_ETH_CLK_ENABLE()
#define ETH_GPIO_ClK_ENABLE()         \
    {                                 \
        __HAL_RCC_GPIOA_CLK_ENABLE(); \
        __HAL_RCC_GPIOC_CLK_ENABLE(); \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
        __HAL_RCC_GPIOG_CLK_ENABLE(); \
    }

#define IFNAME0 't'
#define IFNAME1 'w'

struct netif gNetif;
ETH_HandleTypeDef gEthHandle;
osThreadId_t gEthTaskID = NULL;
osSemaphoreId_t gEthSemaphore = NULL;

__ALIGN_BEGIN ETH_DMADescTypeDef DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;
__ALIGN_BEGIN ETH_DMADescTypeDef DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;
__ALIGN_BEGIN uint8_t DMARx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END;
__ALIGN_BEGIN uint8_t DMATx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END;

void ETH_IPC_Create(void)
{
    if (gEthSemaphore == NULL) {
        gEthSemaphore = osSemaphoreNew(1, 0, NULL);
        if (gEthSemaphore == NULL) {
            ETH_DEBUG("ETH_IPC_Create gEthSemaphore failed!\r\n");
            return;
        }
    }

    if (gEthTaskID == NULL) {
        osThreadAttr_t attr = {.name = "ETH_Input", .attr_bits = 0U, .cb_mem = NULL, .stack_mem = NULL, .stack_size = 3072, .priority = 5};
        gEthTaskID = osThreadNew((osThreadFunc_t)ethernetif_input, NULL, &attr);
        if (gEthTaskID == NULL) {
            ETH_DEBUG("ETH_IPC_Create gEthTaskID failed!\r\n");
        }
    }
}

extern void ETH_IRQHandler(void);
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    ETH_GPIO_ClK_ENABLE();
    ETH_RCC_CLK_ENABLE();

    /**ETH GPIO Configuration
    PA0     ------> ETH_RST
    PC1     ------> ETH_MDC
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PG11     ------> ETH_TX_EN
    PG13     ------> ETH_TXD0
    PG14     ------> ETH_TXD1
    */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF0_TRACE;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AFx_ETH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

    LOS_HwiCreate(ETH_IRQn, 0, 5, (HWI_PROC_FUNC)USER_IRQHandler, 0);
}

void getmac(uint8_t *pMacBuf)
{
    if (pMacBuf == NULL)
        return;

    uint32_t uiMcuId = 0;
    uint8_t pMcuID[15] = {0};
    int i = 0;
    uint32_t CpuID[3] = {0};

    CpuID[0] = *(uint32_t *)(0x1fff7a10);
    CpuID[1] = *(uint32_t *)(0x1fff7a14);
    CpuID[2] = *(uint32_t *)(0x1fff7a18);
    ETH_DEBUG("MCU UID: %08X-%08X-%08X\r\n",CpuID[0],CpuID[1],CpuID[2]);

    pMcuID[0] = (uint8_t)(CpuID[0] & 0x000000FF);
    pMcuID[1] = (uint8_t)((CpuID[0] & 0xFF00) >> 8);
    pMcuID[2] = (uint8_t)((CpuID[0] & 0xFF0000) >> 16);
    pMcuID[3] = (uint8_t)((CpuID[0] & 0xFF000000) >> 24);

    pMcuID[4] = (uint8_t)(CpuID[1] & 0xFF);
    pMcuID[5] = (uint8_t)((CpuID[1] & 0xFF00) >> 8);
    pMcuID[6] = (uint8_t)((CpuID[1] & 0xFF0000) >> 16);
    pMcuID[7] = (uint8_t)((CpuID[1] & 0xFF000000) >> 24);

    pMcuID[8] = (uint8_t)(CpuID[2] & 0xFF);
    pMcuID[9] = (uint8_t)((CpuID[2] & 0xFF00) >> 8);
    pMcuID[10] = (uint8_t)((CpuID[2] & 0xFF0000) >> 16);
    pMcuID[11] = (uint8_t)((CpuID[2] & 0xFF000000) >> 24);

    uiMcuId = (CpuID[0] >> 1) + (CpuID[1] >> 2) + (CpuID[2] >> 3);

    for (i = 0; i < 12; i++) {
        pMcuID[12] += pMcuID[i];
    }
    for (i = 0; i < 12; i++) {
        pMcuID[13] ^= pMcuID[i];
    }

    pMacBuf[0] = pMcuID[12] & 0xFE;
    pMacBuf[1] = (uint8_t)(uiMcuId & 0xFF);
    pMacBuf[2] = (uint8_t)((uiMcuId & 0xFF00) >> 8);
    pMacBuf[3] = (uint8_t)((uiMcuId & 0xFF0000) >> 16);
    pMacBuf[4] = (uint8_t)((uiMcuId & 0xFF000000) >> 24);
    pMacBuf[5] = pMcuID[13];
    return;
}

void low_level_init(struct netif *netif)
{
    uint32_t regvalue = 0;
    uint8_t macaddress[6] = {0};
    EthLinkInfo linkInfo = {0};

    get_ethernet_link_info(&linkInfo);
    if (linkInfo.useStaticMac == 1) {
        memcpy(macaddress, linkInfo.macAddr, 6);
    } else {
        getmac(macaddress);
        memcpy(linkInfo.macAddr, macaddress, 6);
        set_ethernet_link_info(&linkInfo);
    }

    ETH_DEBUG("MacADDR: %02X-%02X-%02X-%02X-%02X-%02X\n", macaddress[0], macaddress[1],
              macaddress[2], macaddress[3],
              macaddress[4], macaddress[5]);

    gEthHandle.Instance = ETH;
    gEthHandle.Init.MACAddr = macaddress;
    gEthHandle.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
    gEthHandle.Init.Speed = ETH_SPEED_100M;
    gEthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
    gEthHandle.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
    gEthHandle.Init.RxMode = ETH_RXINTERRUPT_MODE;
    gEthHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
    gEthHandle.Init.PhyAddress = LAN8720_PHY_ADDRESS;
    if (HAL_ETH_Init(&gEthHandle) == HAL_OK) {
        ETH_DEBUG("ETH Init Success!\r\n");
        netif->flags |= NETIF_FLAG_LINK_UP;
    } else {
        ETH_DEBUG("ETH Init Fail!\r\n");
        netif->flags &= ~NETIF_FLAG_LINK_UP;
    }

    HAL_ETH_DMATxDescListInit(&gEthHandle, DMATxDscrTab, &DMATx_Buff[0][0], ETH_TXBUFNB);
    HAL_ETH_DMARxDescListInit(&gEthHandle, DMARxDscrTab, &DMARx_Buff[0][0], ETH_RXBUFNB);

    netif->hwaddr_len = ETH_HWADDR_LEN;
    netif->hwaddr[0] = macaddress[0];
    netif->hwaddr[1] = macaddress[1];
    netif->hwaddr[2] = macaddress[2];
    netif->hwaddr[3] = macaddress[3];
    netif->hwaddr[4] = macaddress[4];
    netif->hwaddr[5] = macaddress[5];

    netif->mtu = 1500;
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    ETH_IPC_Create();

    HAL_ETH_Start(&gEthHandle);
    HAL_ETH_ReadPHYRegister(&gEthHandle, 0x02, &regvalue);

    /* enable phy interrupt */
    HAL_ETH_ReadPHYRegister(&gEthHandle, PHY_MICR, &regvalue);
    regvalue |= (PHY_MICR_INT_EN | PHY_MICR_INT_OE);
    HAL_ETH_WritePHYRegister(&gEthHandle, PHY_MICR, regvalue);

    HAL_ETH_ReadPHYRegister(&gEthHandle, PHY_MISR, &regvalue);
    regvalue |= PHY_MISR_LINK_INT_EN;
    HAL_ETH_WritePHYRegister(&gEthHandle, PHY_MISR, regvalue);
}

struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL, *q = NULL;
    volatile ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    if (HAL_ETH_GetReceivedFrame_IT(&gEthHandle) != HAL_OK) {
        return NULL;
    }
    uint16_t len = gEthHandle.RxFrameInfos.length;
    uint8_t *buffer = (uint8_t *)gEthHandle.RxFrameInfos.buffer;

    if (len > 0) {
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }

    if (p != NULL) {
        dmarxdesc = gEthHandle.RxFrameInfos.FSRxDesc;
        bufferoffset = 0;
        for (q = p; q != NULL; q = q->next) {
            byteslefttocopy = q->len;
            payloadoffset = 0;
            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE) {
                memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));
                dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }
    dmarxdesc = gEthHandle.RxFrameInfos.FSRxDesc;
    for (uint32_t i = 0; i < gEthHandle.RxFrameInfos.SegCount; i++) {
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    gEthHandle.RxFrameInfos.SegCount = 0;
    if ((gEthHandle.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET) {
        gEthHandle.Instance->DMASR = ETH_DMASR_RBUS;
        gEthHandle.Instance->DMARPDR = 0;
    }
    return p;
}

err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval;
    struct pbuf *q;
    uint8_t *buffer = (uint8_t *)(gEthHandle.TxDesc->Buffer1Addr);
    volatile ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;

    DmaTxDesc = gEthHandle.TxDesc;
    for (q = p; q != NULL; q = q->next) {
        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
            errval = ERR_USE;
            goto error;
        }

        byteslefttocopy = q->len;
        bufferoffset = 0;
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE) {
            memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));
            DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
            if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET) {
                errval = ERR_USE;
                goto error;
            }

            buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);
            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }
    HAL_ETH_TransmitFrame(&gEthHandle, framelength);
    errval = ERR_OK;

error:
    if ((gEthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET) {
        gEthHandle.Instance->DMASR = ETH_DMASR_TUS;
        gEthHandle.Instance->DMATPDR = 0;
    }
    return errval;
}

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    osSemaphoreRelease(gEthSemaphore);
}

void ethernetif_input(void const *argument)
{
    struct pbuf *p;
    struct netif *netif = (struct netif *)&gNetif;

    for (;;) {
        if (osSemaphoreAcquire(gEthSemaphore, osWaitForever) == osOK) {
            do {
                p = low_level_input(netif);
                if (p != NULL) {
                    if (netif->input(p, netif) != ERR_OK) {
                        pbuf_free(p);
                    }
                }
            } while (p != NULL);
        }
    }
}

err_t ethernetif_init(struct netif *netif)
{
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "talkweb_stm32";
#endif

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    low_level_init(netif);

    return ERR_OK;
}

void USER_IRQHandler(void)
{
    osKernelLock();
    HAL_ETH_IRQHandler(&gEthHandle);
    osKernelUnlock();
}

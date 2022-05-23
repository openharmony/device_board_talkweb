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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_run.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "can_init.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_TxHeaderTypeDef RxHeader;
CAN_FilterTypeDef Filter;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t aRxData[8] = {0};
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, aRxData) == HAL_OK) {
        uint32_t canid = (RxHeader.IDE == CAN_ID_STD)?RxHeader.StdId:RxHeader.ExtId;
        printf("\nRecv CANID:0x%08X Data:", canid);
        for (uint8_t i = 0; i<8; i++)
            printf("%02X ", aRxData[i]);
        printf("\n");
    }
}

void thread_entry(void)
{
    uint8_t TxData[8] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
    uint32_t TxMailbox;
    uint32_t send_can_id = 0x1234;

    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.IDE = CAN_ID_EXT; 
    TxHeader.ExtId = send_can_id;
    TxHeader.TransmitGlobalTime = DISABLE;
    TxHeader.DLC = 8;

    Filter.FilterIdHigh             = 0;
    Filter.FilterIdLow              = 0;
    Filter.FilterMaskIdHigh         = 0;
    Filter.FilterMaskIdLow          = 0;
    Filter.FilterFIFOAssignment     = CAN_FILTER_FIFO0;
    Filter.FilterBank               = 0;
    Filter.FilterMode               = CAN_FILTERMODE_IDMASK;
    Filter.FilterScale              = CAN_FILTERSCALE_32BIT;
    Filter.FilterActivation         = ENABLE;
    Filter.SlaveStartFilterBank     = 0;

    HAL_CAN_ConfigFilter(&hcan1, &Filter);

    while (HAL_OK != HAL_CAN_Start(&hcan1)) {
        printf("HAL_CAN_Start fail! try again!\n");
        osDelay(100);
    }

    printf("HAL_CAN_Start success!\n");
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    while (1) {
        uint32_t ret = HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
        if (ret == HAL_OK) {
            printf("send can data [0x%04X : %02X %02X %02X %02X %02X %02X %02X %02X] success! Tx_Mail:%u\n", \
                send_can_id, TxData[0], TxData[1], TxData[2], TxData[3], TxData[4], TxData[5], TxData[6], \
                TxData[7], TxMailbox);
        }
        else {
            printf("send can data fail,ret = %s\n", \
                (ret==HAL_ERROR)?"HAL_ERROR":((ret==HAL_BUSY)?"HAL_BUSY":"HAL_TIMEOUT"));
        }
        osDelay(1000);
    }
}

static void can_send_example(void)
{
    MX_CAN1_Init();

    osThreadAttr_t attr;
    attr.name = "thread1";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 4096;
    attr.priority = 25;

    if (osThreadNew((osThreadFunc_t)thread_entry, NULL, &attr) == NULL) {
        printf("Failed to create thread!\n");
    }
}

OHOS_APP_RUN(can_send_example);

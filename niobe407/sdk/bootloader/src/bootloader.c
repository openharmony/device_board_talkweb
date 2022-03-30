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
#include "stm32f4xx_hal.h"

#define APP_START_ADDR (0x8010000)
#define JUMP_APP_ADDR (APP_START_ADDR + 4)
#define APP_RAM_CMP_VAL 0x2FFE0000
#define APP_RAM_CMP_ADDR 0x20020000
#define RCC_PLL_PLLM 16
#define RCC_PLL_PLLN 192
#define RCC_PLL_PLLQ 4

#define UART_BAUDRATE 115200

UART_HandleTypeDef huart1;

static void SystemClockConfig(void);
static void MX_USART1_UART_Init(void);
static void IapExecuteApp(unsigned int appStartAddr);

int main(void)
{
    HAL_Init();
    SystemClockConfig();
    MX_USART1_UART_Init();
    printf("stm32f4xx bootloader start...\r\n");
    IapExecuteApp(APP_START_ADDR);
    while (1) {
    }
}

static void SystemClockConfig(void)
{
    RCC_OscInitTypeDef RCCOscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLL_PLLM;
    RCC_OscInitStruct.PLL.PLLN = RCC_PLL_PLLN;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLL_PLLQ;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);
}

static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = UART_BAUDRATE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6 | GPIO_PIN_7);
    }
}

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
static void MSR_MSP(unsigned int ulAddr)
{
    __ASM volatile("MSR MSP, r0");
    __ASM volatile("BX r14");
}

static void IapExecuteApp(unsigned int appStartAddr)
{
    unsigned int JumpAddress;

    if (((*(volatile unsigned int *)appStartAddr) & APP_RAM_CMP_VAL) == APP_RAM_CMP_ADDR) {
        JumpAddress = *(volatile unsigned int *)JUMP_APP_ADDR;
        Jump_To_Application = (pFunction)JumpAddress;
        MSR_MSP(*(volatile unsigned int *)appStartAddr);
        HAL_UART_MspDeInit(&huart1);
        HAL_RCC_DeInit();
        Jump_To_Application();
    }

    printf("into app fail! \r\n");
}

int _write(int fd, char *pBuffer, int size)
{
    for (int i = 0; i < size; i++) {
        HAL_UART_Transmit(&huart1, (unsigned char *)&pBuffer[i], 1, 0xFFFF);
    }
    return size;
}

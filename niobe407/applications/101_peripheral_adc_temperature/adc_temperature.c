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

#include "adc_temperature.h"
#include "los_interrupt.h"
#include "los_arch_interrupt.h"

ADC_HandleTypeDef hadcx;
DMA_HandleTypeDef hdma_adcx;

static uint16_t ADC_ConvertedValue;

void MX_ADCx_Init(void)
{
    ADC_ChannelConfTypeDef sConfig;
    hadcx.Instance = ADCx;
    hadcx.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadcx.Init.Resolution = ADC_RESOLUTION_12B;
    hadcx.Init.ScanConvMode = DISABLE;
    hadcx.Init.ContinuousConvMode = ENABLE;
    hadcx.Init.DiscontinuousConvMode = DISABLE;
    hadcx.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadcx.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadcx.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadcx.Init.NbrOfConversion = 1;
    hadcx.Init.DMAContinuousRequests = ENABLE;
    hadcx.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    HAL_ADC_Init(&hadcx);
    sConfig.Channel = ADC_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;  
    HAL_ADC_ConfigChannel(&hadcx, &sConfig);
}

void ADCx_DMA_IRQx_Handler(void)
{
    HAL_DMA_IRQHandler(&hdma_adcx);
}

void MX_DMA_Init(void)
{
    DMAx_RCC_CLK_ENABLE();
    LOS_HwiCreate(OS_SYS_VECTOR_CNT+ADCx_DMA_IRQx, 0, 1,(HWI_PROC_FUNC)ADCx_DMA_IRQx_Handler, 0);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance==ADCx) {
        ADCx_RCC_CLK_ENABLE();
        hdma_adcx.Instance = DMAx__Stream_x;
        hdma_adcx.Init.Channel = DMAx__CHANNEL_x;
        hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adcx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adcx.Init.Mode = DMA_CIRCULAR;
        hdma_adcx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_adcx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        HAL_DMA_Init(&hdma_adcx);
        __HAL_LINKDMA(hadc,DMA_Handle,hdma_adcx);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance==ADCx) {
        ADCx_RCC_CLK_DISABLE();
        HAL_DMA_DeInit(hadc->DMA_Handle);
    }
} 

void Temperature_ADC_Init()
{
    MX_DMA_Init();
    MX_ADCx_Init();
    HAL_ADC_Start_DMA(&hadcx,(uint32_t *)&ADC_ConvertedValue,sizeof(ADC_ConvertedValue));  
}

double Temperature_Get()
{
    double ADC_ConvertedValueLocal;
    __IO double Current_Temperature; 
    ADC_ConvertedValueLocal =(double)(ADC_ConvertedValue&0xFFF)*3.3/4096; 
    Current_Temperature = (ADC_ConvertedValueLocal-0.76)/0.0025+25;
    return Current_Temperature;					 
}
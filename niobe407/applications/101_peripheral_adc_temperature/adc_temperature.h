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

#ifndef __ADC_TEMPERATURE_H__
#define __ADC_TEMPERATURE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_adc_ex.h"

#define ADCx_RCC_CLK_ENABLE()            __HAL_RCC_ADC1_CLK_ENABLE()
#define ADCx_RCC_CLK_DISABLE()           __HAL_RCC_ADC1_CLK_DISABLE()
#define DMAx_RCC_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define ADCx                             ADC1
#define ADCx_DMA_IRQx                    DMA2_Stream0_IRQn

#define DMAx__Stream_x                   DMA2_Stream0
#define DMAx__CHANNEL_x                  DMA_CHANNEL_0

#define ADC_CHANNEL                      ADC_CHANNEL_TEMPSENSOR

extern void Temperature_ADC_Init();

extern double Temperature_Get();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __ADC_TEMPERATURE_H__ */

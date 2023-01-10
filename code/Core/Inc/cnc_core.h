
// Francesco Osti 25/12/2022 //

#ifndef __CNC_CORE_H
#define __CNC_CORE_H

#include "main.h"
#include "stm32l4xx_hal.h"
#include "cnc_info.h"

int32_t ihyp(uint16_t x, uint16_t y);

void CNC_Stepper(HMI_info_t* info, bool runX, bool runY, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_Absolute(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_JOG(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);

void CNC_TIM_Callback_X(HMI_info_t* info, TIM_HandleTypeDef *htimX);
void CNC_TIM_Callback_Y(HMI_info_t* info, TIM_HandleTypeDef *htimY);

void CNC_HL_Control(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY, UART_HandleTypeDef *huart, volatile uint16_t *adc_data);
void CNC_Stop(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_Limit_X(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_Limit_Y(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);

#endif


// Francesco Osti 25/12/2022 //

#ifndef __CNC_CORE_H
#define __CNC_CORE_H

#include "main.h"
#include "stm32l4xx_hal.h"
#include "cnc_info.h"

void CNC_Absolute(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_JOG(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY);
void CNC_HL_Control(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY, UART_HandleTypeDef *huart);

#endif

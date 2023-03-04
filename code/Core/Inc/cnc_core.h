
// Francesco Osti 25/12/2022 //

#ifndef __CNC_CORE_H
#define __CNC_CORE_H

#include "main.h"
#include "stm32l4xx_hal.h"
#include "cnc_info.h"

#define fastPulseX_01mm 4
#define fastPulseY_01mm 4

float __ihyp(float x, float y);
void __swapP12(HMI_info_t* const info);

void CNC_Stepper(HMI_info_t* const info);

void CNC_JogX_Forced(HMI_info_t* const info);
void CNC_JogY_Forced(HMI_info_t* const info);
void CNC_Absolute(HMI_info_t* const info);
bool CNC_AbsoluteX(HMI_info_t* const info);
bool CNC_AbsoluteY(HMI_info_t* const info);
void CNC_JOG(HMI_info_t* const info);

void CNC_TIM_Callback_X(HMI_info_t* const info);
void CNC_TIM_Callback_Y(HMI_info_t* const info);

void CNC_HL_Control(HMI_info_t* const info, UART_HandleTypeDef *huart, volatile uint16_t *adc_data);
void CNC_Stop(HMI_info_t* const info);
void CNC_Limit_X(HMI_info_t* const info);
void CNC_Limit_Y(HMI_info_t* const info);

#endif

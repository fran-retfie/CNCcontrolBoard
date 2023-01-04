
//--- Francesco Osti 29/12/2022 ---//

#include "cnc_core.h"
#include "cnc_config.h"
#include "data_parser.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

void CNC_Absolute(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){
    bool dirX = (info->pos.x > info->commanded.pos.x);
    bool dirY = (info->pos.y > info->commanded.pos.y);

    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, dirX);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, dirY);

    uint16_t pulsesX = (dirX ? (info->pos.x - info->commanded.pos.x) : (info->commanded.pos.x - info->pos.x)) * stepX_01mm;
    uint16_t pulsesY = (dirY ? (info->pos.y - info->commanded.pos.y) : (info->commanded.pos.y - info->pos.y)) * stepY_01mm;

    //start timer 1 ad 15, generate pulses
    if(pulsesX > 0){
        htimX->Instance->RCR = pulsesX-1;
        htimX->Instance->ARR = 10;
        htimX->Instance->CCR3 = 5;
        htimX->Instance->EGR = TIM_EGR_UG;
        htimX->Instance->CR1 |= TIM_CR1_CEN;
        HAL_TIMEx_PWMN_Start(htimX, TIM_CHANNEL_3);
    }
    
    if(pulsesY > 0){
        htimY->Instance->RCR = pulsesY-1;
        htimY->Instance->ARR = 10;
        htimY->Instance->CCR1 = 5;
        htimY->Instance->EGR = TIM_EGR_UG;
        htimY->Instance->CR1 |= TIM_CR1_CEN;
        HAL_TIM_PWM_Start(htimY, TIM_CHANNEL_1);
    }

    while((htimX->Instance->CR1 | htimY->Instance->CR1) & TIM_CR1_CEN);

    info->pos.x = info->commanded.pos.x;
    info->pos.y = info->commanded.pos.y;
}

void CNC_Jog(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){

    bool dirX = (info->commanded.speed.x > 0);
    bool dirY = (info->commanded.speed.y > 0);

    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, dirX);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, dirY);

    uint16_t pulsesX = (info->commanded.speed.x != 0) ? 10 : 0;
    uint16_t pulsesY = (info->commanded.speed.y != 0) ? 10 : 0;
    uint16_t periodX = (uint16_t) div(10000, abs(info->commanded.speed.x)).quot;
    uint16_t periodY = (uint16_t) div(10000, abs(info->commanded.speed.y)).quot;

    //start timer 1 ad 15, generate pulses
    if(pulsesX > 0){
        htimX->Instance->RCR = pulsesX-1;
        htimX->Instance->ARR = periodX;
        htimX->Instance->CCR3 = periodX>>1;
        htimX->Instance->EGR = TIM_EGR_UG;
        htimX->Instance->CR1 |= TIM_CR1_CEN;
        HAL_TIMEx_PWMN_Start(htimX, TIM_CHANNEL_3);
    }
    
    if(pulsesY > 0){
        htimY->Instance->RCR = pulsesY-1;
        htimY->Instance->ARR = periodY;
        htimY->Instance->CCR1 = periodY>>1;
        htimY->Instance->EGR = TIM_EGR_UG;
        htimY->Instance->CR1 |= TIM_CR1_CEN;
        HAL_TIM_PWM_Start(htimY, TIM_CHANNEL_1);
    }

    while((htimX->Instance->CR1 | htimY->Instance->CR1) & TIM_CR1_CEN);

    if(pulsesX > 0){
        info->pos.x = dirX ? info->pos.x+10 : info->pos.x-10;
        info->update = true;
    }
        
    if(pulsesY > 0){
        info->pos.y = dirY ? info->pos.y+10 : info->pos.y-10;
        info->update = true;
    }
}

void CNC_HL_Control(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY, UART_HandleTypeDef *huart){
    switch (info->mode) {
        case  HMI_Mode_Zero:
        break;

        case  HMI_Mode_Man:
            info->move = HMI_Move_Jog;
            CNC_Jog(info, htimX, htimY);
        break;

        case  HMI_Mode_Face:
            
        break; 

        case  HMI_Mode_Ser:
            if(info->state == HMI_State_Run){
                if(info->move == HMI_Move_None){
                    if(ReadGcode(huart, info)) 
                        info->move = HMI_Move_Absolute; 
                    else
                        WriteACK(huart, false);
                } 
                if(info->move == HMI_Move_Absolute) {
                    CNC_Absolute(info, htimX, htimY);
                    info->move = HMI_Move_None;
                    info->update = true;
                    WriteACK(huart, true);
                }
            }               
        break;
    }
}
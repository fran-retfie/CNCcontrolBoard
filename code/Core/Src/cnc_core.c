
//--- Francesco Osti 29/12/2022 ---//

#include "cnc_core.h"
#include "cnc_config.h"
#include "data_parser.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

//very efficient way of approximating hypotenuse using integer numbers
int32_t ihyp(uint16_t x, uint16_t y){
    if(y > x){
        return (uint32_t) y + ((((uint32_t) x * (uint32_t) x)/y)>>1);
    }
    else {
        return (uint32_t) x + ((((uint32_t) y * (uint32_t) y)/x)>>1);
    }
}

void CNC_Stepper(HMI_info_t* info, bool runX, bool runY, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){
    //runX &= !(!HAL_GPIO_ReadPin(limitX_Port, limitX_Pin) || ((info->pos.x == 0) && !info->dir.x) || ((info->pos.x >= max_limX) && info->dir.x) );
    //runY &= !(!HAL_GPIO_ReadPin(limitY_Port, limitY_Pin) || ((info->pos.y == 0) && !info->dir.y) || ((info->pos.y >= max_limX) && info->dir.y) );
    if(!HAL_GPIO_ReadPin(SWSTOP_GPIO_Port, SWSTOP_Pin)){
        if(runX){
            if(!(htimX->Instance->CR1 && TIM_CR1_CEN)){
                htimX->Instance->ARR = info->pulseLenght.x;
                htimX->Instance->CCR3 = ((info->pulseLenght.x)>>1);
            }
            htimX->Instance->CR1 &= ~TIM_CR1_OPM;
            htimX->Instance->CR1 |= TIM_CR1_CEN;
        }
        else
            htimX->Instance->CR1 |= TIM_CR1_OPM;

        if(runY){
            if(!(htimY->Instance->CR1 && TIM_CR1_CEN)){
                htimY->Instance->ARR = info->pulseLenght.y;
                htimY->Instance->CCR3 = ((info->pulseLenght.y)>>1);
            }
            htimY->Instance->CR1 &= ~TIM_CR1_OPM; 
            htimY->Instance->CR1 |= TIM_CR1_CEN;
        }
        else
            htimY->Instance->CR1 |= TIM_CR1_OPM;          
    }
    else
    {
        info->state = HMI_State_Stop;
        htimX->Instance->CR1 |= TIM_CR1_OPM;
        htimY->Instance->CR1 |= TIM_CR1_OPM;
    }     
}

void CNC_Absolute(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){

    info->dir.x = (info->pos.x < info->commanded.pos.x);
    info->dir.y = (info->pos.y < info->commanded.pos.y);

    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, info->dir.x);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, info->dir.y);

    uint16_t dX = info->dir.x ? (info->commanded.pos.x - info->pos.x) : (info->pos.x - info->commanded.pos.x); //abs
    uint16_t dY = info->dir.y ? (info->commanded.pos.y - info->pos.y) : (info->pos.y - info->commanded.pos.y); //abs

    bool runX = dX > 0;
    bool runY = dY > 0;

    //sistema questa parte dai...
    int32_t den;
    if(((dX > dY) ? dX : dY) >= (1<<8))
        den = (int32_t) ihyp(dX, dY) * freqX_1mm_min;
    else
        den = (int32_t) ihyp(dX<<7, dY<<7) * (freqX_1mm_min>>7);

    info->pulseLenght.x = (uint16_t) div(den, (int32_t) info->feed * (int32_t) dX).quot;
    info->pulseLenght.y = (uint16_t) div(den, (int32_t) info->feed * (int32_t) dY).quot;

    CNC_Stepper(info, runX, runY, htimX, htimY);    

    if(!runX && !runY)
        info->move = HMI_Move_Done; 
}

void CNC_Jog(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){

    info->dir.x = (info->commanded.speed.x > 0);
    info->dir.y = (info->commanded.speed.y > 0);

    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, info->dir.x);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, info->dir.y);

    info->pulseLenght.x = (uint16_t) div(freqX_1mm_min, abs(info->commanded.speed.x)).quot;
    info->pulseLenght.y = (uint16_t) div(freqY_1mm_min, abs(info->commanded.speed.y)).quot;

    CNC_Stepper(info, info->commanded.speed.x != 0, info->commanded.speed.y != 0, htimX, htimY); 
}

void CNC_TIM_Callback_X(HMI_info_t* info, TIM_HandleTypeDef *htimX){
    htimX->Instance->ARR = info->pulseLenght.x;
    htimX->Instance->CCR3 = ((info->pulseLenght.x)>>1);

    if(info->dir.x){
        if(info->pulsesCnt.x == stepX_01mm-1){
            info->pulsesCnt.x = 0;
            if(info->pos.x < max_limX)
                info->pos.x++;
            info->update = true;
        }
        else
            info->pulsesCnt.x++;
    }
    else{
        if(info->pulsesCnt.x == 0){
            info->pulsesCnt.x = 3;
            if(info->pos.x > 0)
                info->pos.x--;
            info->update = true;
        }
        else
            info->pulsesCnt.x--;
    }
}

void CNC_TIM_Callback_Y(HMI_info_t* info, TIM_HandleTypeDef *htimY){
    htimY->Instance->ARR = info->pulseLenght.y;
    htimY->Instance->CCR1 = ((info->pulseLenght.y)>>1);

    if(info->dir.y){
        if(info->pulsesCnt.y == stepY_01mm-1){
            info->pulsesCnt.y = 0;
            if(info->pos.y < max_limY)
                info->pos.y++;
            info->update = true;
        }
        else
            info->pulsesCnt.y++;
    }
    else{
        if(info->pulsesCnt.y == 0){
            info->pulsesCnt.y = 3;
            if(info->pos.y > 0)
                info->pos.y--;
            info->update = true;
        }
        else
            info->pulsesCnt.y--;
    }
}

void CNC_HL_Control(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY, UART_HandleTypeDef *huart, volatile uint16_t *adc_data){
    switch (info->mode) {
        case  HMI_Mode_Zero:
            if(info->state == HMI_State_Run){
                if(HMI_Move_None){
                    info->move = HMI_Move_Jog;
                    info->commanded.speed.x = -200;  
                    info->commanded.speed.y = -200;
                    CNC_Jog(info, htimX, htimY);
                }
            }
        break;

        case  HMI_Mode_Man:
            info->move = HMI_Move_Jog;
            int16_t jogX = ((int16_t) (adc_data[6]>>1)) - (1844/2); //subtract center value X
            int16_t jogY = ((int16_t) (adc_data[7]>>1)) - (1814/2); //subtract center value Y
            info->commanded.speed.x = (abs(jogX) > 70) ? jogX : 0;  // dead zone near center X
            info->commanded.speed.y = (abs(jogY) > 70) ? jogY : 0;  // dead zone near center Y
            CNC_Jog(info, htimX, htimY);
        break;

        case  HMI_Mode_Face:
            
        break; 

        case  HMI_Mode_Ser:
            if(info->state == HMI_State_Run){
                switch (info->move) {
                case HMI_Move_None:
                    if(ReadGcode(huart, info)) 
                        info->move = HMI_Move_Absolute; 
                    else
                        WriteACK(huart, false);
                break;

                case HMI_Move_Done:
                    info->move = HMI_Move_None;
                    info->update = true;
                    WriteACK(huart, true);
                break;

                case HMI_Move_Absolute:
                    CNC_Absolute(info, htimX, htimY);
                break;

                case HMI_Move_Jog:
                    info->move = HMI_Move_None;;
                break;
                }
            }               
        break;
    }
}

void CNC_Stop(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){
    htimX->Instance->CR1 |= TIM_CR1_OPM;
    htimY->Instance->CR1 |= TIM_CR1_OPM;

    info->state = HMI_State_Stop;
    info->update = true;
    info->cnt2 = 30;
}

void CNC_Limit_X(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){
    htimX->Instance->CR1 |= TIM_CR1_OPM;

    info->state = HMI_State_Stop;
    info->pos.x = 0;
    info->zeroed.x = true;
    info->update = true;
}

void CNC_Limit_Y(HMI_info_t* info, TIM_HandleTypeDef *htimX, TIM_HandleTypeDef *htimY){
    htimY->Instance->CR1 |= TIM_CR1_OPM;

    info->state = HMI_State_Stop;
    info->pos.y = 0;
    info->zeroed.y = true;
    info->update = true;
}

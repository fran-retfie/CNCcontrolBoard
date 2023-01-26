
//--- Francesco Osti 29/12/2022 ---//

#include "cnc_core.h"
#include "cnc_config.h"
#include "data_parser.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

//very efficient way of approximating hypotenuse using integer numbers
uint32_t ihyp(uint32_t x, uint32_t y){
    if(y > x)
        return y + (((x*x)/y)>>1);
    else
        return x + (((y*y)/x)>>1);
}

void CNC_Stepper(HMI_info_t* const info, bool runX, bool runY){

    runX &= !(HAL_GPIO_ReadPin(limitX_Port, limitX_Pin) || ((info->pos.x == 0) && !info->dir.x) || ((info->pos.x >= max_limX*stepX_01mm) && info->dir.x) );
    runY &= !(HAL_GPIO_ReadPin(limitY_Port, limitY_Pin) || ((info->pos.y == 0) && !info->dir.y) || ((info->pos.y >= max_limY*stepY_01mm) && info->dir.y) );
    
    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, info->dir.x);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, info->dir.y);
    
    bool run = (!(SWSTOP_GPIO_Port->IDR & SWSTOP_Pin) && (info->state == HMI_State_Run || info->move == HMI_Move_Jog));

    if(run){
        if(runX){
            info->htimX->Instance->ARR = info->pulseLenght.x;
            info->htimX->Instance->CCR3 = ((info->pulseLenght.x)>>1);
            info->htimX->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN;
        }
        else{
            info->htimX->Instance->CR1 |= TIM_CR1_OPM;
            info->htimX->Instance->SR = 0x00000000U;
        }

        if(runY){
            info->htimY->Instance->ARR = info->pulseLenght.y;
            info->htimY->Instance->CCR1 = ((info->pulseLenght.y)>>1);
            info->htimY->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; 
        }
        else{
            info->htimY->Instance->CR1 |= TIM_CR1_OPM;
            info->htimY->Instance->SR = 0x00000000U;          
        }
    }
    else{
        info->state = HMI_State_Stop;
        info->htimX->Instance->CR1 |= TIM_CR1_OPM;
        info->htimY->Instance->CR1 |= TIM_CR1_OPM;
    }
}

void CNC_Absolute(HMI_info_t* const info){

    info->dir.x = (info->pos.x < info->commanded.pos.x);
    info->dir.y = (info->pos.y < info->commanded.pos.y);

    uint32_t dX = info->dir.x ? (info->commanded.pos.x - info->pos.x)>>2 : (info->pos.x - info->commanded.pos.x)>>2; //abs
    uint32_t dY = info->dir.y ? (info->commanded.pos.y - info->pos.y)>>2 : (info->pos.y - info->commanded.pos.y)>>2; //abs

    bool runX = dX > 0;
    bool runY = dY > 0;


    //sistema questa parte dai...
    uint32_t den;
    uint32_t range = (dX > dY) ? dX : dY;

    if(range > (1<<16)-1){
       dX <<= 2;
       dY <<= 2; 
    }

    if(range < (1<<8))
        den = ihyp(dX<<7, dY<<7) * ((freqX_1mm_min>>7)/info->feed);
    else
        den = ihyp(dX, dY) * (freqX_1mm_min/info->feed);

    info->pulseLenght.x = (uint16_t) (den/dX);
    info->pulseLenght.y = (uint16_t) (den/dY);

    CNC_Stepper(info, runX, runY);    

    if(!runX && !runY)
        info->move = HMI_Move_Done; 
}

bool CNC_AbsoluteX(HMI_info_t* const info){
    info->dir.x = (info->pos.x < info->commanded.pos.x);
    uint32_t dX = info->dir.x ? (info->commanded.pos.x - info->pos.x)>>2 : (info->pos.x - info->commanded.pos.x)>>2; //abs
    bool runX = dX > 0;
    info->pulseLenght.x = (uint16_t) (freqX_1mm_min / info->feed);

    CNC_Stepper(info, runX, false);    

    return !runX;
}

bool CNC_AbsoluteY(HMI_info_t* const info){
    info->dir.y = (info->pos.y < info->commanded.pos.y);
    uint32_t dY = info->dir.y ? (info->commanded.pos.y - info->pos.y)>>2 : (info->pos.y - info->commanded.pos.y)>>2; //abs
    bool runY = dY > 0;
    info->pulseLenght.y = (uint16_t) (freqY_1mm_min / info->feed);

    CNC_Stepper(info, false, runY);    

    return !runY;
}

void CNC_Jog(HMI_info_t* const info){

    info->dir.x = (info->commanded.speed.x > 0);
    info->dir.y = (info->commanded.speed.y > 0);

    if(info->commanded.speed.x != 0)
        info->pulseLenght.x = (uint16_t) freqX_1mm_min / abs(info->commanded.speed.x);
    else
        info->pulseLenght.x = 200;

    if(info->commanded.speed.y != 0)
        info->pulseLenght.y = (uint16_t) freqY_1mm_min / abs(info->commanded.speed.y);
    else
        info->pulseLenght.x = 200;

    CNC_Stepper(info, info->commanded.speed.x != 0, info->commanded.speed.y != 0); 
}

void CNC_TIM_Callback_X(HMI_info_t* const info){
    if(info->dir.x)
        info->pos.x++;
    else
        if(info->pos.x != 0)
            info->pos.x--;
    info->update = true;         
}

void CNC_TIM_Callback_Y(HMI_info_t* const info){
    if(info->dir.y)
        info->pos.y++;
    else
        if(info->pos.y != 0)
            info->pos.y--;
    info->update = true;  
}

void CNC_HL_Control(HMI_info_t* const info, UART_HandleTypeDef *huart, volatile uint16_t *adc_data){
    switch (info->mode) {
        case  HMI_Mode_Zero:
            if(info->state == HMI_State_Run){
                if(HMI_Move_None){
                    info->move = HMI_Move_Jog;
                    info->commanded.speed.x = -200;  
                    info->commanded.speed.y = -200;
                    CNC_Jog(info);
                }
            }
        break;

        case  HMI_Mode_Man:
            info->move = HMI_Move_Jog;
            int16_t jogX = ((int16_t) (adc_data[6]>>1)) - (1844/2); //subtract center value X
            int16_t jogY = ((int16_t) (adc_data[7]>>1)) - (1814/2); //subtract center value Y
            info->commanded.speed.x = (abs(jogX) > 70) ? jogX : 0;  // dead zone near center X
            info->commanded.speed.y = (abs(jogY) > 70) ? jogY : 0;  // dead zone near center Y
            CNC_Jog(info);
        break;

        case  HMI_Mode_Face:
            if(info->state == HMI_State_Run){
                switch (info->move) {
                    case HMI_Move_None:
                        if(info->P1.x > info->P2.x){
                            uint32_t swapX = info->P1.x;
                            info->P1.x = info->P2.x;
                            info->P2.x = swapX;
                        }

                        if(info->P1.y > info->P2.y){
                            uint32_t swapY = info->P1.y;
                            info->P1.y = info->P2.y;
                            info->P2.y = swapY;
                        }

                        info->commanded.pos.x = info->P1.x;
                        info->commanded.pos.y = info->P1.y;
                        info->move = HMI_Move_Absolute;
                    break;

                    case HMI_Move_Face1:
                        info->commanded.pos.x = info->P2.x;
                        if(CNC_AbsoluteX(info))
                            info->move = HMI_Move_Face2;
                    break;

                    case HMI_Move_Face2:
                        uint32_t newPosY = info->pos.y + face_depth_Y;
                        if(newPosY < info->P2.y)
                            info->commanded.pos.y = newPosY;
                        else
                            info->commanded.pos.y = info->P2.y;
                        if(CNC_AbsoluteY(info))
                            info->move = HMI_Move_Face3;
                    break;

                    case HMI_Move_Face3:
                        info->commanded.pos.x = info->P1.x;
                        if(CNC_AbsoluteX(info)){
                            if(info->pos.y >= info->P2.y){
                                info->move = HMI_Move_None;
                                info->state = HMI_State_Stop;
                            }
                            else
                                info->move = HMI_Move_Face4;
                        }
                    break;

                    case HMI_Move_Face4:
                        info->commanded.pos.y = info->pos.y - face_retract_Y;
                        if(CNC_AbsoluteY(info))
                            info->move = HMI_Move_Face1;
                    break;

                    case HMI_Move_Done:
                        info->move = HMI_Move_Face1;
                    break;

                    case HMI_Move_Absolute:
                        CNC_Absolute(info);
                    break;

                    default:
                        info->move = HMI_Move_None;
                    break;
                }
            }
        break; 

        case  HMI_Mode_Ser:
            if(info->state == HMI_State_Run){
                switch (info->move) {
                case HMI_Move_None:
                    //HAL_UART_Receive_DMA(&huart1, UART1_rxBuffer, 12);
                    //if(ReadGcode(huart, info)) 
                    //    info->move = HMI_Move_Absolute; 
                    //else
                    //    WriteACK(huart, false);
                break;

                case HMI_Move_Done:
                    info->move = HMI_Move_None;
                    info->update = true;
                    WriteACK(huart, true);
                break;

                case HMI_Move_Absolute:
                    CNC_Absolute(info);
                break;

                default:
                    info->move = HMI_Move_None;
                break;
                }
            }            
        break;
    }
}

void CNC_Stop(HMI_info_t* const info){
    info->htimX->Instance->CR1 |= TIM_CR1_OPM;
    info->htimY->Instance->CR1 |= TIM_CR1_OPM;
    info->htimX->Instance->SR = 0x00000000U;
    info->htimY->Instance->SR = 0x00000000U;

    info->state = HMI_State_Stop;
    info->update = true;
    info->cnt2 = 10;
}

void CNC_Limit_X(HMI_info_t* const info){
    info->htimX->Instance->CR1 |= TIM_CR1_OPM;
    info->htimX->Instance->SR = 0x00000000U;

    info->state = HMI_State_Stop;
    info->pos.x = 0;
    info->zeroed.x = true;
    info->update = true;
}

void CNC_Limit_Y(HMI_info_t* const info){
    info->htimY->Instance->CR1 |= TIM_CR1_OPM;
    info->htimY->Instance->SR = 0x00000000U;

    info->state = HMI_State_Stop;
    info->pos.y = 0;
    info->zeroed.y = true;
    info->update = true;
}


//--- Francesco Osti 29/12/2022 ---//

#include "cnc_core.h"
#include "cnc_config.h"
#include "data_parser.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>

//very efficient way of approximating hypotenuse using integer numbers
float ihyp(float x, float y){
    if(y > x)
        return y + (((x*x)/y)*0.5);
    else
        return x + (((y*y)/x)*0.5);
}

void CNC_Stepper(HMI_info_t* const info){

    info->run.x &= !(HAL_GPIO_ReadPin(limitX_Port, limitX_Pin) || ((info->pos.x == 0) && !info->dir.x) || ((info->pos.x >= max_limX*stepX_01mm) && info->dir.x)) || !info->zeroed.x;
    info->run.y &= !(HAL_GPIO_ReadPin(limitY_Port, limitY_Pin) || ((info->pos.y == 0) && !info->dir.y) || ((info->pos.y >= max_limY*stepY_01mm) && info->dir.y)) || !info->zeroed.y;
    
    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, info->dir.x);
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, info->dir.y);
    
    bool run = (!(SWSTOP_GPIO_Port->IDR & SWSTOP_Pin) && (info->state == HMI_State_Run || info->move == HMI_Move_Jog));
    info->run.x &= run;
    info->run.y &= run;

    if(info->run.x){
        info->htimX->Instance->ARR = info->pulseLenght.x;
        info->htimX->Instance->CCR1 = ((info->pulseLenght.x)>>1);
        if(!(info->htimX->Instance->CR1 & TIM_CR1_CEN))
            info->htimX->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN;
    }
    else{
        //info->htimX->Instance->CR1 |= TIM_CR1_OPM;
        info->htimX->Instance->SR = 0x00000000U;
    }

    if(info->run.y){
        info->htimY->Instance->ARR = info->pulseLenght.y;
        info->htimY->Instance->CCR3 = ((info->pulseLenght.y)>>1);
        if(!(info->htimY->Instance->CR1 & TIM_CR1_CEN))
            info->htimY->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN; 
    }
    else{
    //    info->htimY->Instance->CR1 |= TIM_CR1_OPM;
        info->htimY->Instance->SR = 0x00000000U;          
    }

    if(!run){
        info->state = HMI_State_Stop;
        //info->htimX->Instance->CR1 |= TIM_CR1_OPM;
        //info->htimY->Instance->CR1 |= TIM_CR1_OPM;
    }
}

void CNC_JogX_Forced(HMI_info_t* const info){
    info->run.x = true;
    HAL_GPIO_WritePin(dirX_Port, dirX_Pin, true);
    info->htimX->Instance->ARR = 200;
    info->htimX->Instance->CCR1 = 100;
    if(!(info->htimX->Instance->CR1 & TIM_CR1_CEN))
        info->htimX->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN;
}

void CNC_JogY_Forced(HMI_info_t* const info){
    info->run.y = true;
    HAL_GPIO_WritePin(dirY_Port, dirY_Pin, true);
    info->htimY->Instance->ARR = 200;
    info->htimY->Instance->CCR1 = 100;
    if(!(info->htimY->Instance->CR1 & TIM_CR1_CEN))
        info->htimY->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN;
}

void CNC_Absolute(HMI_info_t* const info){

    info->dir.x = (info->pos.x < info->commanded.pos.x);
    info->dir.y = (info->pos.y < info->commanded.pos.y);

    uint32_t dX = info->dir.x ? (info->commanded.pos.x - info->pos.x) : (info->pos.x - info->commanded.pos.x); //abs
    uint32_t dY = info->dir.y ? (info->commanded.pos.y - info->pos.y) : (info->pos.y - info->commanded.pos.y); //abs

    info->run.x = dX > 0;
    info->run.y = dY > 0;

    //sistema questa parte dai...
    float den = ihyp((float) dX, (float) dY) * (float) (freqX_1mm_min/info->feed);

    
    info->pulseLenght.x = (uint16_t) (den/((float) dX));
    info->pulseLenght.y = (uint16_t) (den/((float) dY));

    //da sistemare
    if(info->pulseLenght.x < 10) info->pulseLenght.x = 1000;
    if(info->pulseLenght.y < 10) info->pulseLenght.y = 1000;

    CNC_Stepper(info);    

    if(!info->run.x && !info->run.y)
        info->move = HMI_Move_Done; 
}

bool CNC_AbsoluteX(HMI_info_t* const info){
    info->dir.x = (info->pos.x < info->commanded.pos.x);
    uint32_t dX = info->dir.x ? (info->commanded.pos.x - info->pos.x) : (info->pos.x - info->commanded.pos.x); //abs
    
    info->run.x = dX > 0;
    info->run.y = false;

    info->pulseLenght.x = (uint16_t) (freqX_1mm_min / info->feed);

    CNC_Stepper(info);    

    return !info->run.x;
}

bool CNC_AbsoluteY(HMI_info_t* const info){
    info->dir.y = (info->pos.y < info->commanded.pos.y);
    uint32_t dY = info->dir.y ? (info->commanded.pos.y - info->pos.y) : (info->pos.y - info->commanded.pos.y); //abs
    
    info->run.x = false;
    info->run.y = dY > 0;

    info->pulseLenght.y = (uint16_t) (freqY_1mm_min / info->feed);

    CNC_Stepper(info);    

    return !info->run.y;
}

void CNC_Jog(HMI_info_t* const info){

    info->dir.x = (info->commanded.speed.x >= 0);
    info->dir.y = (info->commanded.speed.y >= 0);

    if(info->commanded.speed.x != 0)
        info->pulseLenght.x = (uint16_t) (freqX_1mm_min) / abs(info->commanded.speed.x);
    else
        info->pulseLenght.x = 200;

    if(info->commanded.speed.y != 0)
        info->pulseLenght.y = (uint16_t) (freqY_1mm_min) / abs(info->commanded.speed.y);
    else
        info->pulseLenght.y = 200;

    info->run.x = info->commanded.speed.x != 0;
    info->run.y = info->commanded.speed.y != 0;

    CNC_Stepper(info); 
}

void CNC_TIM_Callback_X(HMI_info_t* const info){
    if(info->run.x)
        info->htimX->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN;

    if(info->dir.x)
        info->pos.x++;
    else{
        if(info->pos.x != 0)
            info->pos.x--;
    }
    info->update = true;         
}

void CNC_TIM_Callback_Y(HMI_info_t* const info){
    if(info->run.y)
        info->htimY->Instance->CR1 |= TIM_CR1_ARPE | TIM_CR1_OPM | TIM_CR1_CEN;

    if(info->dir.y)
        info->pos.y++;
    else{
        if(info->pos.y != 0)
            info->pos.y--;
    }
    info->update = true;  
}

void CNC_HL_Control(HMI_info_t* const info, UART_HandleTypeDef *huart, volatile uint16_t *adc_data){
    switch (info->mode) {
        case  HMI_Mode_Zero:

            HAL_GPIO_WritePin(spindle_Port, spindle_Pin, false);

            if(info->state == HMI_State_Run){
                switch (info->move){
                    case  HMI_Move_None:
                    case  HMI_Move_ZeroX:
                        info->move = HMI_Move_ZeroX;
                        info->commanded.speed.x = -2000;  
                        info->commanded.speed.y = 0;
                        CNC_Jog(info);
                    break;

                    case  HMI_Move_ZeroXB:
                        CNC_JogX_Forced(info);
                        if(!HAL_GPIO_ReadPin(limitX_Port, limitX_Pin)){
                            info->run.x = false;
                            info->move = HMI_Move_ZeroY;
                        }
                    break;

                    case  HMI_Move_ZeroY:
                        info->commanded.speed.x = 0;  
                        info->commanded.speed.y = -2000;
                        CNC_Jog(info);
                    break;

                    case  HMI_Move_ZeroYB:
                        CNC_JogX_Forced(info);
                        if(!HAL_GPIO_ReadPin(limitX_Port, limitX_Pin)){
                            info->run.y = false;
                            info->move = HMI_Move_Done;
                        }
                    break;

                    default:
                        info->state = HMI_State_Stop;
                        info->run.x = false;
                        info->run.y = false;
                    break;
                }
            }
        break;

        case  HMI_Mode_Man:
            info->move = HMI_Move_Jog;
            int16_t jogX = ((int16_t) (adc_data[6]>>1)) - (1844/2); //subtract center value X
            int16_t jogY = ((int16_t) (adc_data[7]>>1)) - (1814/2); //subtract center value Y
            info->commanded.speed.x = (abs(jogX) > 70) ? jogX*4 : 0;  // dead zone near center X
            info->commanded.speed.y = (abs(jogY) > 70) ? jogY*4 : 0;  // dead zone near center Y
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

    HAL_GPIO_WritePin(spindle_Port, spindle_Pin, false);

    info->run.x = false;
    info->run.y = false;
    info->state = HMI_State_Stop;
    info->update = true;
    info->cnt2 = 10;
}

void CNC_Limit_X(HMI_info_t* const info){
    info->htimX->Instance->CR1 |= TIM_CR1_OPM;
    info->htimX->Instance->SR = 0x00000000U;

    HAL_GPIO_WritePin(spindle_Port, spindle_Pin, false);

    info->state = HMI_State_Stop;
    info->pos.x = 0;
    info->zeroed.x = true;
    info->update = true;
    info->run.x = false;
}

void CNC_Limit_Y(HMI_info_t* const info){
    info->htimY->Instance->CR1 |= TIM_CR1_OPM;
    info->htimY->Instance->SR = 0x00000000U;

    HAL_GPIO_WritePin(spindle_Port, spindle_Pin, false);

    info->state = HMI_State_Stop;
    info->pos.y = 0;
    info->zeroed.y = true;
    info->update = true;
    info->run.y = false;
}

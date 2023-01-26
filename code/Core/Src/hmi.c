
// Francesco Osti 25/12/2022 //

#include "hmi.h"
#include "main.h"
#include "lcd.h"
#include "cnc_core.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t __Read_Pushbuttons(){
    return (PB0_GPIO_Port->IDR & (PB0_Pin | PB1_Pin | PB2_Pin))>>(13-5) | (PB7_GPIO_Port->IDR & (0x7C))>>2;
}

static char ModeNames[4][17]  = {"AZZERAMENTO     ", "MANUALE         ", "SPIANATURA      ", "COMPUTER        "};
static char StateNames[4][17] = {"STOP            ", "PAUSE           ", "RUN             "};

void __BP_Control(HMI_info_t* const info, uint8_t mask, bool pause_mask){
  uint8_t newPushbuttons = __Read_Pushbuttons();
  uint8_t PBpressed = ~newPushbuttons & info->pushbuttons & mask;

  if((PBpressed & PB_MODE) && info->zeroed.x && info->zeroed.y && (info->state == HMI_State_Stop)){
    info->mode = (info->mode >= 3) ? HMI_Mode_Zero : info->mode+1;
    info->state = HMI_State_Stop;
    info->move = HMI_Move_None;
    info->update = true;
    info->cnt2 = 10;
  }

  if(PBpressed & PB_STOP){
    CNC_Stop(info);
  }

  if(PBpressed & PB_SET){
    info->Psel = !info->Psel;
    info->update = true;
  }

  if(PBpressed & PB_JOY){
    if(info->Psel){
      info->P1.x = info->pos.x;
      info->P1.y = info->pos.y;
      info->P1set = true;
    }
    else {
      info->P2.x = info->pos.x;
      info->P2.y = info->pos.y;
      info->P2set = true;
    }

    info->update = true;
  }

  if((PBpressed & PB_RUN) && (info->state != HMI_State_Stop)){
    CNC_Stop(info);
    info->state = pause_mask ? HMI_State_Pause : HMI_State_Stop;
  }

  if((~newPushbuttons & PB_RUN) && !HAL_GPIO_ReadPin(SWSTOP_GPIO_Port, SWSTOP_Pin)){
    info->cnt1++;
    if(info->cnt1 > 20) {
      info->state = HMI_State_Run;
      info->update = true;
      info->cnt2 = 10;
      info->cnt1 = 0;
    }
  } 
  else {
  info->cnt1 = 0;
  }

  info->pushbuttons = newPushbuttons;
}

void HMI_Update(HMI_info_t* const info){
  switch (info->mode) {
  case  HMI_Mode_Zero:
    __BP_Control(info, (PB_STOP | PB_RUN | PB_MODE | PB_SET | PB_JOY), false);
  break;

  case  HMI_Mode_Man:
    __BP_Control(info, (PB_STOP | PB_RUN | PB_MODE | PB_SET | PB_JOY), false);

  
  break;

  case  HMI_Mode_Face:
    __BP_Control(info, (PB_STOP | PB_RUN | PB_MODE | PB_SET | PB_JOY), true);
  break; 

  case  HMI_Mode_Ser:
    __BP_Control(info, (PB_STOP | PB_RUN | PB_MODE | PB_SET | PB_JOY), true);
  break;
  }

  if(info->cnt2 != 0){
    info->cnt2--;
    if(info->update){
      char textStr[17];
      sprintf(textStr, ModeNames[info->mode]);
      setCursor(0, 0);
      writeLCD(textStr);
      sprintf(textStr, StateNames[info->state]);
      setCursor(0, 1);
      writeLCD(textStr);
      info->update = false;
    }
    if(info->cnt2 == 0) info->update = true;
  } 
  else {
    if(info->update){
      char textStr[17];

      if(info->zeroed.y) {
        sprintf(textStr, "Y: %05lumm   P1\xdb", info->pos.y/stepY_01mm);
        memmove(textStr+8, textStr+7, 4);
        if(info->P1set)
          textStr[15] = 0xffU;
        if(info->Psel)
          textStr[12] = 0x7eU;
        textStr[7] = '.';
      }
      else
        sprintf(textStr, "Y: ???");
      //setCursor(0, 0);
      //writeLCD(textStr);

      if(info->zeroed.x){
        sprintf(textStr, "X: %05lumm   P2\xdb", info->pos.x/stepY_01mm);
        memmove(textStr+8, textStr+7, 4);
        if(info->P2set)
          textStr[15] = 0xffU;
        if(!info->Psel)
          textStr[12] = 0x7eU;
        textStr[7] = '.';
      }
      else
        sprintf(textStr, "X: ???");
      //setCursor(0, 1);
      //writeLCD(textStr);
      info->update = false;
    }
  }
}

/*
void Test_Stuff(){
    //HAL_Delay(700);
    setCursor(10,  0);
    sprintf(textStr, "%u   ", adc_data[5]);
    writeLCD(textStr);

    setCursor(0,  1);
    sprintf(textStr, "%u   ", adc_data[6]);
    writeLCD(textStr);

    setCursor(10,  1);
    sprintf(textStr, "%u   ", adc_data[7]);
    writeLCD(textStr);

    setCursor(0,  0);
    uint8_t PBvals = Read_Pushbuttons();
    for(int i = 8; i != 0; i--){
      putLCD((PBvals & 0x80U) ? '1' : '0');
      PBvals <<= 1;
    }

    setCursor(0,  0);
    if(HAL_GPIO_ReadPin(SWSTOP_GPIO_Port, SWSTOP_Pin)) sprintf(textStr, "STOP");
    else                                               sprintf(textStr, "    ");
    setCursor(0,  0);
    writeLCD(textStr);

    HAL_StatusTypeDef status = HAL_UART_Receive(&huart1, textStr, 1,500);
    if(status == HAL_OK) HAL_UART_Transmit(&huart1, textStr, 1,100);
}
*/
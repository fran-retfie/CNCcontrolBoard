
// Francesco Osti 25/12/2022 //

#include "lowLevelStuff.h"

uint8_t Read_Pushbuttons(){
    return (PB0_GPIO_Port->IDR & (PB0_Pin | PB1_Pin | PB2_Pin))>>(13-5) | (PB7_GPIO_Port->IDR & (0x7C))>>2;
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
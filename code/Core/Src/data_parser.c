
// Francesco Osti 25/12/2022 //

#include "data_parser.h"
#include <stdio.h>
#include <string.h>

bool CheckSum(uvec16_t* coord, uint16_t sum_value, uint16_t xor_value){

    // check checksum values
    uint16_t sum_check = coord->x + coord->y;
    uint16_t xor_check = coord->x ^ coord->y;

    return ((sum_value == sum_check) && (xor_value == xor_check));
}

void WriteACK(UART_HandleTypeDef *huart, bool status){
    char uartData = status ? 'A' : 'N';
    HAL_UART_Transmit(huart, (uint8_t*) &uartData, 1, 100);
}

bool ReadGcode(UART_HandleTypeDef *huart, HMI_info_t* info){
    char uartData[11];
    while(1){
      if(HAL_UART_Receive(huart, (uint8_t*) uartData, 11, 100) == HAL_OK){
        if(uartData[10] == 0){
          break;
        }
      }
    }

    uint16_t *g_cmd, *sum_value, *xor_value;
    info->pos.x  = *(uint16_t*) uartData;
    info->pos.y  = *((uint16_t*) uartData + 1);
    g_cmd     = (uint16_t*) (uartData + 4);
    sum_value = (uint16_t*) (uartData + 6);
    xor_value = (uint16_t*) (uartData + 8);

    return CheckSum(&(info->pos), *sum_value, *xor_value);
}
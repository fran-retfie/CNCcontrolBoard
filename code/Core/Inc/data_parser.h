
// Francesco Osti 25/12/2022 //

#ifndef __DATA_PARSER_H
#define __DATA_PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "cnc_info.h"

bool CheckSum(uvec16_t* coord, uint16_t sum_value, uint16_t xor_value);
void WriteACK(UART_HandleTypeDef *huart, bool status);
bool ReadGcode(UART_HandleTypeDef *huart, HMI_info_t* info);

#endif

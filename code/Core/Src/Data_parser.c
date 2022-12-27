#include "Data_parser.h"


bool CheckSum(uint16_t X_value, uint16_t Y_value, uint16_t sum_value, uint16_t xor_value){

    // check checksum values
    uint16_t sum_check = X_value + Y_value;
    uint16_t xor_check = X_value ^ Y_value;

    return ((sum_value == sum_check) && (xor_value == xor_check));
}
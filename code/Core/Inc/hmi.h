
// Francesco Osti 25/12/2022 //

#ifndef __HMI_H
#define __HMI_H

#include <stdbool.h>
#include <stdint.h>
#include "cnc_info.h"
#include "cnc_config.h"

#define PB_MODE 0x10U
#define PB_RUN  0x40U
#define PB_SET  0x08U
#define PB_SPINDLE 0x02U
#define PB_JOY  0x01U

uint8_t __Read_Pushbuttons(void);
void __BP_Control(HMI_info_t* const info, uint8_t mask, bool pause_mask);
void HMI_Update(HMI_info_t* const info, volatile uint16_t *adc_data);

//void    Test_Stuff();

#endif
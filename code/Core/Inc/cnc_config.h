
// Francesco Osti 25/12/2022 //

#ifndef __CNC_CONFIG_H
#define __CNC_CONFIG_H

//output pin definition
#define enX_Pin   O8_Pin
#define enX_Port  O8_GPIO_Port
#define enY_Pin   O9_Pin
#define enY_Port  O9_GPIO_Port
#define dirX_Pin  O10_Pin
#define dirX_Port O10_GPIO_Port
#define dirY_Pin  O11_Pin
#define dirY_Port O11_GPIO_Port
#define enX_tim   htim15
#define enY_tim   htim1
#define limitX_Pin SW0_Pin
#define limitX_Port SW0_GPIO_Port
#define limitY_Pin SW1_Pin
#define limitY_Port SW1_GPIO_Port
#define spindle_Pin PO3_Pin
#define spindle_Port PO3_GPIO_Port

//step required to move 0.1mm
#define stepX_01mm 8
#define stepY_01mm 8

//max velocity
#define max_velX 4000
#define max_velY 4000

//max accelleration
#define max_accX 40
#define max_accY 40

//stepper speed divider
#define freqX_1mm_min 65514 
#define freqY_1mm_min 65514

//CNC limits in steps units
#define max_limX 11280 //1300.0 mm
#define max_limY 40000 //4000.0 mm
#define min_limX 0
#define min_limY 0

//CNC face parameters
#define face_depth_Y   380
#define face_retract_Y 380

#endif

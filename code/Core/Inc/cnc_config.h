
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
#define enX_tim   htim1
#define enY_tim   htim15
#define limitX_Pin SW0_Pin
#define limitX_Port SW0_GPIO_Port
#define limitY_Pin SW1_Pin
#define limitY_Port SW1_GPIO_Port

//step required to move 0.1mm
#define stepX_01mm 4
#define stepY_01mm 4

//max velocity
#define max_velX 1000
#define max_velY 1000

//max accelleration
#define max_accX 40
#define max_accY 40

//stepper speed divider
#define freqX_1mm_min 65217
#define freqY_1mm_min 65217

//CNC limits in steps units
#define max_limX stepX_01mm*13000 //1300.0 mm
#define max_limY stepY_01mm*40000 //4000.0 mm
#define min_limX 0
#define min_limY 0

#endif

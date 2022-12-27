#ifndef INC_LCD_CONFIG_H_
#define INC_LCD_CONFIG_H_

#include "main.h"

/* CONFIG FOR LIBRARY USER */
#define DATA_PORT D4_GPIO_Port
#define CMD_PORT  EN_GPIO_Port

//4 pin mode -> pins
#define DATA5_Pin D4_Pin
#define DATA6_Pin D5_Pin
#define DATA7_Pin D6_Pin
#define DATA8_Pin D7_Pin

//#define RS_Pin RS_Pin
#define E_Pin  EN_Pin

#endif /* INC_LCD_CONFIG_H_ */

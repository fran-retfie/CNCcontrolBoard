/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PB7_Pin GPIO_PIN_2
#define PB7_GPIO_Port GPIOE
#define PB6_Pin GPIO_PIN_3
#define PB6_GPIO_Port GPIOE
#define PB5_Pin GPIO_PIN_4
#define PB5_GPIO_Port GPIOE
#define PB4_Pin GPIO_PIN_5
#define PB4_GPIO_Port GPIOE
#define PB3_Pin GPIO_PIN_6
#define PB3_GPIO_Port GPIOE
#define PB2_Pin GPIO_PIN_13
#define PB2_GPIO_Port GPIOC
#define PB1_Pin GPIO_PIN_14
#define PB1_GPIO_Port GPIOC
#define PB0_Pin GPIO_PIN_15
#define PB0_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOC
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOC
#define A2_Pin GPIO_PIN_2
#define A2_GPIO_Port GPIOC
#define A3_Pin GPIO_PIN_3
#define A3_GPIO_Port GPIOC
#define DIR2_Pin GPIO_PIN_1
#define DIR2_GPIO_Port GPIOA
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOA
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOA
#define SWSTOP_Pin GPIO_PIN_6
#define SWSTOP_GPIO_Port GPIOA
#define A6_Pin GPIO_PIN_7
#define A6_GPIO_Port GPIOA
#define A7_Pin GPIO_PIN_4
#define A7_GPIO_Port GPIOC
#define PO0_Pin GPIO_PIN_5
#define PO0_GPIO_Port GPIOC
#define PO1_Pin GPIO_PIN_0
#define PO1_GPIO_Port GPIOB
#define PO2_Pin GPIO_PIN_2
#define PO2_GPIO_Port GPIOB
#define PO3_Pin GPIO_PIN_7
#define PO3_GPIO_Port GPIOE
#define O0_Pin GPIO_PIN_8
#define O0_GPIO_Port GPIOE
#define O1_Pin GPIO_PIN_9
#define O1_GPIO_Port GPIOE
#define O2_Pin GPIO_PIN_10
#define O2_GPIO_Port GPIOE
#define O3_Pin GPIO_PIN_11
#define O3_GPIO_Port GPIOE
#define O4_Pin GPIO_PIN_12
#define O4_GPIO_Port GPIOE
#define O5_Pin GPIO_PIN_13
#define O5_GPIO_Port GPIOE
#define O6_Pin GPIO_PIN_14
#define O6_GPIO_Port GPIOE
#define O7_Pin GPIO_PIN_15
#define O7_GPIO_Port GPIOE
#define DIR3_Pin GPIO_PIN_12
#define DIR3_GPIO_Port GPIOB
#define O8_Pin GPIO_PIN_14
#define O8_GPIO_Port GPIOB
#define O9_Pin GPIO_PIN_15
#define O9_GPIO_Port GPIOB
#define O10_Pin GPIO_PIN_8
#define O10_GPIO_Port GPIOD
#define O11_Pin GPIO_PIN_9
#define O11_GPIO_Port GPIOD
#define O12_Pin GPIO_PIN_10
#define O12_GPIO_Port GPIOD
#define O13_Pin GPIO_PIN_11
#define O13_GPIO_Port GPIOD
#define O14_Pin GPIO_PIN_12
#define O14_GPIO_Port GPIOD
#define O15_Pin GPIO_PIN_13
#define O15_GPIO_Port GPIOD
#define SW0_Pin GPIO_PIN_14
#define SW0_GPIO_Port GPIOD
#define SW1_Pin GPIO_PIN_15
#define SW1_GPIO_Port GPIOD
#define SW2_Pin GPIO_PIN_6
#define SW2_GPIO_Port GPIOC
#define SW3_Pin GPIO_PIN_7
#define SW3_GPIO_Port GPIOC
#define I0_Pin GPIO_PIN_0
#define I0_GPIO_Port GPIOD
#define I1_Pin GPIO_PIN_1
#define I1_GPIO_Port GPIOD
#define I2_Pin GPIO_PIN_3
#define I2_GPIO_Port GPIOD
#define I3_Pin GPIO_PIN_4
#define I3_GPIO_Port GPIOD
#define I4_Pin GPIO_PIN_5
#define I4_GPIO_Port GPIOD
#define I5_Pin GPIO_PIN_6
#define I5_GPIO_Port GPIOD
#define I6_Pin GPIO_PIN_7
#define I6_GPIO_Port GPIOD
#define I7_Pin GPIO_PIN_3
#define I7_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_6
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_7
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_8
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_9
#define D7_GPIO_Port GPIOB
#define EN_Pin GPIO_PIN_0
#define EN_GPIO_Port GPIOE
#define RS_Pin GPIO_PIN_1
#define RS_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

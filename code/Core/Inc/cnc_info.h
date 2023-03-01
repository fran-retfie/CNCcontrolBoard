
// Francesco Osti 25/12/2022 //

#ifndef __CNCINFO_H
#define __CNCINFO_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32l4xx_hal.h"

typedef struct {
    uint32_t x;
    uint32_t y;
} uvec32_t;

typedef struct {
    int32_t x;
    int32_t y;
} svec32_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} uvec16_t;

union comanded_t {
   uvec32_t pos;
   svec32_t speed;
};

typedef struct {
  bool x;
  bool y;
} bvec_t;

typedef enum
{
  HMI_Mode_Zero,
  HMI_Mode_Man,
  HMI_Mode_Face1,
  HMI_Mode_Face2,
  HMI_Mode_Ser, 
} HMI_Mode_t;

typedef enum
{
  HMI_State_Stop,
  HMI_State_Pause,
  HMI_State_Run
} HMI_State_t;

typedef enum
{
  HMI_Move_Absolute,
  HMI_Move_Jog,
  HMI_Move_None,
  HMI_Move_Done,
  HMI_Move_Face1,
  HMI_Move_Face2,
  HMI_Move_Face3,
  HMI_Move_Face4,
  HMI_Move_ZeroX,
  HMI_Move_ZeroXB,
  HMI_Move_ZeroY,
  HMI_Move_ZeroYB
} HMI_Move_t;

typedef struct
{
    HMI_Mode_t mode;
    HMI_State_t state;
    HMI_Move_t move;
    uvec32_t pos;
    uvec16_t pulseLenght;
    bvec_t dir;
    union comanded_t commanded;
    uint32_t feed;
    uvec32_t P1;
    uvec32_t P2;
    bool P1set;
    bool P2set;
    bool Psel;
    uint8_t cnt1;
    uint8_t cnt2;
    bool update;
    bool HMIupdate;
    bool HMIline;
    bvec_t run;
    bvec_t zeroed;
    uint8_t pushbuttons;
    TIM_HandleTypeDef* htimX;
    TIM_HandleTypeDef* htimY;
} HMI_info_t;

#endif
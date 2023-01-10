
// Francesco Osti 25/12/2022 //

#ifndef __CNCINFO_H
#define __CNCINFO_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} uvec16_t;

typedef struct {
    int16_t x;
    int16_t y;
} svec16_t;

union comanded_t {
   uvec16_t pos;
   svec16_t speed;
};

typedef enum
{
  HMI_Mode_Zero,
  HMI_Mode_Man,
  HMI_Mode_Face, 
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
  HMI_Move_Done
} HMI_Move_t;

typedef struct
{
    HMI_Mode_t mode;
    HMI_State_t state;
    HMI_Move_t move;
    uvec16_t pos;
    svec16_t pulseLenght;
    uvec16_t pulsesCnt;
    bool dirX;
    bool dirY;
    union comanded_t commanded;
    uint16_t feed;
    uint8_t cnt1;
    uint8_t cnt2;
    bool update;
    bool zeroed;
    uint8_t pushbuttons;
} HMI_info_t;

#endif
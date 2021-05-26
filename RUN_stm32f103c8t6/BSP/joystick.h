#ifndef _JOYSTICK_H
#define _JOYSTICK_H 

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"



extern uint16_t joystick_data[2];

void JoyStick_Init(void);

#define M_JOYSTICK_THREASHOLD_LEFT      5
#define M_JOYSTICK_THREASHOLD_RIGHT     4000
#define M_JOYSTICK_THREASHOLD_UP        4000
#define M_JOYSTICK_THREASHOLD_DOWN      100


#endif



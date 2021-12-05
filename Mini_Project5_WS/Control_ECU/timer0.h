/******************************************************************************
 *
 * Module: Timer0
 *
 * File Name: timer0.h
 *
 * Description: Header file for the Timer0 AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/

#include "std_types.h"

#ifndef TIMER0_H_
#define TIMER0_H_


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}Timer0_Clock;

typedef enum
{
	NORMAL , PWM , CTC , FAST_PWM
}Timer0_Mode;

typedef struct
{
	Timer0_Mode mode;
	Timer0_Clock clock;
	uint8 init_value;
	uint8 OCR0_value;
}Timer0_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void Timer0_init(Timer0_ConfigType *Config_PTR);
void Timer0_setCallBack(void(*a_ptr)(void) , Timer0_Mode mode);
void Timer0_stop(void);
void Timer0_DeInit(void);





#endif /* TIMER0_H_ */

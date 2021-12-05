/******************************************************************************
 *
 * Module: Timer0
 *
 * File Name: timer0.c
 *
 * Description: Source file for the Timer0 AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/

#include "timer0.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr_Normal)(void) = NULL_PTR;

static volatile void (*g_callBackPtr_Compare)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *
 *******************************************************************************/

/* Interrupt Service Routine for Timer0 Normal mode */
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr_Normal != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_Normal)();
	}
}


/* Interrupt Service Routine for Timer0 Compare mode */
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr_Compare != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr_Compare)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/


void Timer0_init(Timer0_ConfigType *Config_PTR)
{
	/******************************* Timer0 Description **********************************
	 * Configering the timer to work in either Normal mode or CTC mode by ptr to struct
	 * Normal mode:   WGM00 = 0 , WGM01 = 0
	 * PWM mode:      WGM00 = 1 , WGM01 = 0
	 * CTC mode:      WGM00 = 0 , WGM01 = 1
	 * Fast PWM mode: WGM00 = 1 , WGM01 = 1
	 **************************************************************************************/
	TCCR0 = (((Config_PTR->mode >> 1) << WGM01) & 1) | ((Config_PTR->mode & 1) << WGM00);
	/* Non PWM mode FOC0=1*/
	TCCR0 |= (1<<FOC0);

	/*Setting Timer clock by setting 1st 3-bits CS00:2*/
	TCCR0 |=  (0x07 & Config_PTR->clock);

	TCNT0 = Config_PTR->init_value; //Set Timer initial value

	OCR0 = Config_PTR->OCR0_value; // Set Compare Value

	/*Enabling Interrupts according to the mode  */
	if(BIT_IS_CLEAR(TCCR0 , WGM01))
	{
		TIMSK |= (1<<TOIE0); // Enable Timer0 Overflow Interrupt
		TIMSK &= ~(1<<OCIE0); // Disable Timer0 Compare Interrupt
	}
	else if(BIT_IS_SET(TCCR0 , WGM01))
	{
		TIMSK |= (1<<OCIE0); // Enable Timer0 Compare Interrupt
		TIMSK &= ~(1<<TOIE0); // Disable Timer0 Overflow Interrupt
	}
	/*Enable Globel Interrupt*/
	SREG|=(1<<7);


}


/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*a_ptr)(void) , Timer0_Mode mode)
{
	/* Save the address of the Call back function in a global variable */
	if(mode == NORMAL)
	{
		g_callBackPtr_Normal = a_ptr; //Save Callback Function for Normal mode
	}
	else if(mode == CTC)
	{
		g_callBackPtr_Compare = a_ptr; //Save Callback Function for Compare mode
	}
}

/*
 * Description: Function to stop the Timer0 from counting.
 */
void Timer0_stop(void)
{
	/*Setting Timer clock by setting 1st 3-bits CS00:2 to 0*/
	TCCR0 &= ~(0x07);
}

/*
 * Description: Function to disable the Timer0 Driver
 */
void Timer0_DeInit(void)
{
	/* Clear All Timer0 Registers */
	TCCR0 = 0;
	TCNT0 = 0;
	OCR0 = 0;

	/* Disable the interrupts */
	TIMSK &= ~(1<<TOIE0); // Disable Timer0 Overflow Interrupt
	TIMSK &= ~(1<<OCIE0); // Disable Timer0 Compare Interrupt

}




/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Buzzer AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/

#include "gpio.h"
#include "buzzer.h"

void Buzzer_init(void)
{
	/* Setting pin as output for buzzer */
	GPIO_setupPinDirection(BUZZER_PORT_ID , BUZZER_PIN_ID, PIN_OUTPUT);
}
void Buzzer_on(void)
{
	/* Enabling Buzzer */
	GPIO_writePin(BUZZER_PORT_ID , BUZZER_PIN_ID, LOGIC_HIGH);
}
void Buzzer_off(void)
{
	/* Disabling Buzzer */
	GPIO_writePin(BUZZER_PORT_ID , BUZZER_PIN_ID, LOGIC_LOW);
}

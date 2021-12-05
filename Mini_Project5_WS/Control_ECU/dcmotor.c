/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name:  dcmotor.h
 *
 * Description: Source file for the DC Motor AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/


#include "dcmotor.h"
#include "common_macros.h"
#include "gpio.h"

void DcMotor_Init(void)
{
	/*Setting two pins for the motor*/
	GPIO_setupPinDirection(DCMOTOR_PORT_ID, DCMOTOR_PIN0_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DCMOTOR_PORT_ID, DCMOTOR_PIN1_ID, PIN_OUTPUT);
	/*stopping the motor by writing zero */
	GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN0_ID, LOGIC_LOW);
	GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN1_ID, LOGIC_LOW);

}
/* Description:
  * Control the DC Motor direction using L293D H-bridge.
  */
void DcMotor_Rotate(DcMotor_State state)
{
	if(state == STOP)
	{
		/*stopping the motor by writing zero */
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN0_ID, LOGIC_LOW);
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN1_ID, LOGIC_LOW);

	}
	else if( state == A_CW)
	{
		// Rotate the motor --> anti-clock wise
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN0_ID, LOGIC_LOW);
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN1_ID, LOGIC_HIGH);

	}
	else if(state == CW)
	{
		// Rotate the motor --> clock wise
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN0_ID, LOGIC_HIGH);
		GPIO_writePin(DCMOTOR_PORT_ID, DCMOTOR_PIN1_ID, LOGIC_LOW);

	}

}


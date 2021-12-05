/*********************************************************************
 *
 * Module: HMI_ECU main
 *
 * File Name: main.c
 *
 * Description: Main app for the HMI_ECU
 *
 * Author: Mustafa Esam
 *
 *********************************************************************/

#include "dcmotor.h"
#include "external_eeprom.h"
#include "timer0.h"
#include "twi.h"
#include "buzzer.h"
#include "uart.h"
#include <util/delay.h> /* For the delay functions */

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* Define Commands in Communication between the two Controllers*/
#define MISMATCH          0x00  /* Means the password sent doesn't match the one saved in eeprom */
#define MATCH             0x01  /* Means the password sent matchs the one saved in eeprom */
#define OPENDOOR          0x02  /* Means the user wants to open the door */
#define CHANGEPASS        0x03  /* Means the usaer wants to change the saved password */
#define TRIGGER           0x04  /* Means trigger the buzzer alarm */
#define READY             0x05  /* Means ready to start the timer to count used for sync */

/* Define Time for timer to count by interrupts */
#define SEC_15            457   /* The number of interrupts needed to count 15 sec */
#define SEC_18            548   /* The number of interrupts needed to count 15 + 3 sec */
#define SEC_33            1005  /* The number of interrupts needed to count 15 + 3 + 15 sec */
#define MINUTE            1828  /* The number of interrupts needed to count 1 min */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Globel variable to save the number of overflow interrupts of timer0 */
uint16 g_timer0Ticks = 0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Function to be set as the Callback Function for Timer0
 * It counts the number of overflow interrupts
 * Uses a global variable to save the count
 */
void Timer0_interruptCounter(void)
{
	/* Increment on every interrupt */
	g_timer0Ticks++;
}

/*
 * Description:
 * Function to save the password for the system in eeprom
 * It takes the password array of the system as argument
 * Takes the adress in the eeprom as argument
 */
void Save_Password(uint8 * a_password , uint16 a_adress)
{

	/* Loop to save the password of 5 numbers in eeprom */
	for(uint8 i = 0; i < 5; i++ )
	{
		/* Writing in the eeprom by taking the 1st adress then incrementing it in the loop */
		EEPROM_writeByte(a_adress + i, a_password[i]);
		_delay_ms(10);
	}
}

/*
 * Description:
 * Function to check that the password given match the one saved in eeprom for the system
 * It takes the password array of the system as argument
 * Takes the adress in the eeprom as argument
 * Reads the value of each byte int the memory and compare it to the password
 * Returns 1 if match 0 if mismatch
 */
uint8 Check_Password(uint8 * a_password , uint16 a_adress)
{
	/* Variable to return the compare result
	 * Variable to save byte read from eeprom */
	uint8 password_status = 0 , byte_val;
	/* Loop to check the password of 5 numbers in eeprom */
	for(uint8 i = 0; i < 5; i++ )
	{
		/* Reading from the eeprom by taking the 1st adress then incrementing it in the loop */
		EEPROM_readByte(a_adress + i, &byte_val);
		_delay_ms(10);
		if( a_password[i] != byte_val )
		{
			/* Return 0 means mismatch of passwords */
			password_status = MISMATCH;
			break;
		}
		/* Else: return 1 means match */
		password_status = MATCH;
	}
	return password_status;
}

/*******************************************************************************
 *                                Main Function                                *
 *******************************************************************************/

void main(void)
{
	/* Struct to configer UART with Baud rate = 9600 bps and one stop bit*/
	Uart_ConfigType Config_Uart = { 9600 , ONE_STOP_BIT };

	/* Struct ti configer Timer 0 in overflow mode with prescaler 1024 and initial value = 0*/
	Timer0_ConfigType Config_Timer0 = {NORMAL , F_CPU_1024 , 0  , 255};

	/* Struct to configer I2C with bit rate = 400 kbps and the adress of the Microcontroller is 0x01*/
	I2c_ConfigType  Config_I2c = { FAST_MODE , 0x01};

	/* Initializing Drivers */
	UART_init(&Config_Uart);     /* Initializing UART to communicate with HMI_ECU */
	Timer0_init(&Config_Timer0); /* Initializing Timer0 to count time for motor */
	TWI_init(&Config_I2c);       /* Initializing I2C to communicate with eeprom */
	DcMotor_Init();              /* Initializing DC motor to open and close door */
	Buzzer_init();               /* Initializing buzzer for the alarm */
	/* Setting Callback Function for Timer 0 */
	Timer0_setCallBack(Timer0_interruptCounter, NORMAL);
	/*
	 * Password of 5 numbers each in a byte
	 * Array of bytes to the password of 5 numbers
	 * Char for UART sending string
	 */
	uint8 password[6] = {0};
	uint8 option; /* Variable to save Received request */
	uint8 password_check_status = MISMATCH; /* Initially mismatch to enter the loop 1st time*/

	/*
	 * In case of mismatch of password the password
	 * must be cleared and new password to be saved
	 * To exit the loop the two passwords must be exact
	 */
	while( password_check_status == MISMATCH)
	{
		/* Receiving password */
		UART_receiveString(password);
		/* Initializing password and sending it to be saved in eeprom*/
		Save_Password(password, 0x0311);
		/* Receiving reenetered password */
		UART_receiveString(password);
		/* Checking the reenetered password */
		password_check_status = Check_Password(password, 0x0311);
		/* Sending password compare result to HMI_ECU */
		UART_sendByte(password_check_status);
	}


	while(1)
	{

		/* Receiving requests from HMI_ECU */
		option = UART_recieveByte();
		if(option == OPENDOOR)
		{
			/* Receiving password from HMI */
			UART_receiveString(password);
			/* Checking password with the saved in eeprom */
			password_check_status = Check_Password(password, 0x0311);
			/* sending results to HMI */
			if( password_check_status == MISMATCH)
			{
				/* Send to HMI that password missmatched */
				UART_sendByte(MISMATCH);
			}
			else if(password_check_status == MATCH)
			{
				/* Sending to HMI that password matched */
				UART_sendByte(MATCH);
				/* Wait for HMI to be ready to start and send ready messege */
				UART_sendByte(READY);
				UART_recieveByte();
				/* Initializing Timer to count the time for openning and closing the door*/
				Timer0_init(&Config_Timer0);
				/* Clearing ticks to start counting */
				g_timer0Ticks = 0;
				/* Rotating DC motor for 15 sec CW to open*/
				DcMotor_Rotate(CW);
				/* wait 15 sec till the door is open */
				while(g_timer0Ticks < SEC_15);
				/* Stopping Door for 3 sec */
				DcMotor_Rotate(STOP);
				while( g_timer0Ticks < SEC_18);
				/* Start count for 15 sec and Closing the Door*/
				DcMotor_Rotate(A_CW);
				while( g_timer0Ticks < SEC_33);
				DcMotor_Rotate(STOP);
				/* Stopping Timer to reset it */
				Timer0_DeInit();
				g_timer0Ticks = 0;

			}
		}
		else if(option == CHANGEPASS)
		{

			/* Taking enterd password */
			UART_receiveString(password);
			/* Checking reentered password and responding to HMI */
			password_check_status = Check_Password(password, 0x0311);
			UART_sendByte(password_check_status);
			if(password_check_status == MATCH)
			{
				/* Taking new password from HMI */
				UART_receiveString(password);
				/* Saving password in eeprom */
				Save_Password(password, 0x0311);
			}
		}
		else if(option == TRIGGER)
		{
			/* Triggering buzzer for 1 min */
			Timer0_init(&Config_Timer0);
			Buzzer_on();
			while(g_timer0Ticks < MINUTE);
			/* Stopping Timer to reset it */
			Timer0_DeInit();
			Buzzer_off();
			g_timer0Ticks = 0;
		}
	}
}




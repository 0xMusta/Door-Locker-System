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

#include "timer0.h"
#include "uart.h"
#include "lcd.h"
#include "keypad.h"
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
/* Global variable to save the number of overflow interrupts of timer0 */
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
 * Function to set the initail password for the system
 * It takes the password array of the system as argument
 * Takes values pressed by the keypad stores it in the password array
 * Sends it to the Control_ECU by UART to be saved in eeprom
 */
void Passowrd_init(uint8 *a_password)
{
	/*
	 * Variable to count the 5 passowrd numbers
	 * Variable to check the Entered number is 0
	 */
	uint8 counter , num_check;
	LCD_clearScreen();
	LCD_displayString("Please Enter");
	LCD_displayStringRowColumn(1, 0, "Password: ");

	/*Loop to get 5 numbers*/
	for( counter = 0; counter < 5; counter++)
	{
		/* Taking the number from keypad  */
		num_check = KEYPAD_getPressedKey();
		if( num_check == 0)
		{
			/* Changing the value to send it as string by UART because it has the same value as \0 */
			num_check = 0xFF;
		}
		/* Saving number in the string*/
		a_password[counter] = num_check;
		/*Display the password in the form of ***** */
		LCD_displayCharacter('*');
		/* Delay to read the key once every click */
		_delay_ms(400);

	}

	a_password[5] = '#' ; /*Char for UART sending string Protocol */
	a_password[6] = '\0' ;  /*NULL operator for end of string in memory*/
	UART_sendString(a_password); /*Sending password to the control micro to save it in eeprom*/

}

/*
 * Description:
 * Function to take password from keypad pressed keys
 * It takes the password array of the system as argument
 * Saves the pressed password in the argument array
 * Send password to Control_ECU to compare it with the saved one in eeprom
 */
void TakeSend_Password(uint8 * a_password)
{
	uint8 num_check;/* Variable to check the Entered number is 0 */
	/*Loop to get 5 numbers*/
	for(uint8 counter = 0; counter < 5; counter++)
	{
		/* Taking the number from keypad  */
		num_check = KEYPAD_getPressedKey();
		if( num_check == 0)
		{
			/* Changing the value to send it as string by UART because it has the same value as \0 */
			num_check = 0xFF;
		}
		/* Saving number in the string*/
		a_password[counter] = num_check;
		LCD_displayCharacter('*');/*Display the password in the form of ***** */
		_delay_ms(400);
	}


	a_password[5] = '#' ; /* Char for UART sending string Protocol */
	a_password[6] = '\0' ;  /* NULL operator for end of string in memory*/
	UART_sendString(a_password); /* Sending password to the control micro to save it in eeprom*/

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

	/* Initializing Drivers*/
	LCD_init(); /* Initializing LCD */
	UART_init(&Config_Uart); /* Initializing UART */
	/* Setting Callback Function for Timer 0 */
	Timer0_setCallBack(Timer0_interruptCounter, NORMAL);
	/* Variable to Save the chosen option
	 * Variable to count wrong trials
	 */
	uint8 option , wrong_trials = 0;
	/* Password of 5 numbers each in a byte
	 * Array of bytes to the password of 5 numbers
	 * Char for UART sending string and Null operator
	 */
	uint8 password[7] = {0} , receive_password_msg = MISMATCH; /* Initially mismatch to enter the loop 1st time*/
	/* In case of mismatch of password the password
	 * must be cleared and new password to be saved*/
	/* To exit the loop the two passwords must be exact*/
	while( receive_password_msg == MISMATCH)
	{
		/* Initializing passowrd and sending it to be saved in eeprom*/
		Passowrd_init(password);
		LCD_clearScreen();
		LCD_displayString("Please Reenter ");
		LCD_displayStringRowColumn(1, 0, "Password: ");
		TakeSend_Password(password);
		/* Control micro send messege after compering the reentered password*/
		receive_password_msg = UART_recieveByte();
	}

	while(1)
	{

		/*Displaying options*/
		LCD_clearScreen();
		LCD_displayString(" + : Open Door");
		LCD_displayStringRowColumn(1, 0, " - : Change Password");
		/*
		 * First saving the option in the variable
		 * keep getting the value of the pressed key until it is the right choice
		 */
		option = KEYPAD_getPressedKey();
		_delay_ms(500);

		if(option == '+')/* Open Door */
		{

			/* Sending Door open request to control micro */
			UART_sendByte(OPENDOOR);
			LCD_clearScreen();
			LCD_displayString("Please Enter ");
			LCD_displayStringRowColumn(1, 0, "Password: ");
			TakeSend_Password(password);/* Taking password and sending it to check if it's correct */
			/* Control micro send messege after compering the password with saved one */
			receive_password_msg = UART_recieveByte();
			if(receive_password_msg == MISMATCH)
			{
				LCD_clearScreen();
				LCD_displayString(" Wrong Password");
				_delay_ms(1000);
				LCD_clearScreen();
				/* Increment wrong trials*/
				wrong_trials++;
			}
			else if(receive_password_msg == MATCH)
			{
				/* Wait for Control MC to be ready to start and send ready messege */
				UART_sendByte(READY);
				UART_recieveByte();
				/* Initializing Timer to count the time for openning and closing the door*/
				Timer0_init(&Config_Timer0);
				LCD_clearScreen();
				g_timer0Ticks = 0;
				LCD_displayString("Door unlocking");
				/* wait 15 sec till the door is open */
				while(g_timer0Ticks < SEC_15);
				/* Start count for 3 sec */
				LCD_clearScreen();
				LCD_displayString(" Door is Open");
				/* Waiting 3 sec */
				while( g_timer0Ticks < SEC_18);
				/* Start count for 15 sec */
				LCD_clearScreen();
				LCD_displayString(" Door locking");
				while( g_timer0Ticks < SEC_33);
				/* Stopping Timer to reset it */
				Timer0_DeInit();
				g_timer0Ticks = 0;
				/* Clearing wrong trials because the password was right before 3rd trial */
				wrong_trials = 0;
			}
		}
		else if(option == '-') /*  change password */
		{
			/* Sending to Controller micro pass change request */
			UART_sendByte(CHANGEPASS);
			LCD_clearScreen();
			LCD_displayString("Please Enter ");
			LCD_displayStringRowColumn(1, 0, "Password: ");
			TakeSend_Password(password); /* Comparing it to the saved password */
			/* Control micro send messege after compering the reentered password*/
			receive_password_msg = UART_recieveByte();
			if(receive_password_msg == MISMATCH)
			{
				LCD_clearScreen();
				LCD_displayString(" Wrong Password");
				_delay_ms(1000);
				LCD_clearScreen();
				/* Incrementing wrong trials */
				wrong_trials++;
			}
			else if (receive_password_msg == MATCH)
			{
				LCD_clearScreen();
				LCD_displayString("Enter New");
				LCD_displayStringRowColumn(1, 0, "Password: ");
				/* Taking passowrd and sending it to be saved in eeprom*/
				TakeSend_Password(password);
				/* Clearing wrong trials because the password was right before 3rd trial */
				wrong_trials = 0;
			}
		}
		if(wrong_trials == 3)
		{
			/* Sending request to controller micro to trigger buzzer */
			UART_sendByte(TRIGGER); /* Sending command to controller micro to trigger buzzer */
			LCD_clearScreen();
			LCD_displayString("   ERROR !!   ");
			/* Timer count 1 min */
			Timer0_init(&Config_Timer0);
			while(g_timer0Ticks < MINUTE);
			/* Stopping Timer to reset it */
			Timer0_DeInit();
			g_timer0Ticks = 0;
			/* Clearing wrong trials to restart the system */
			wrong_trials = 0;
		}

	}
}

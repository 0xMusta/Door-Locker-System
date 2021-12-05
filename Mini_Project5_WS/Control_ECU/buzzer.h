/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define BUZZER_PORT_ID          PORTD_ID
#define BUZZER_PIN_ID           PIN2_ID
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Buzzer_init(void);
void Buzzer_on(void);
void Buzzer_off(void);

#endif /* BUZZER_H_ */

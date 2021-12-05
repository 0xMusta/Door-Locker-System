/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name:  dcmotor.h
 *
 * Description: Header file for the DC Motor AVR driver
 *
 * Author: Mustafa Esam
 *
 *******************************************************************************/


#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* DC Motor HW Ports and Pins Ids */
#define DCMOTOR_PORT_ID                PORTC_ID
#define DCMOTOR_PIN0_ID                PIN6_ID
#define DCMOTOR_PIN1_ID                PIN7_ID

/*Enum for the motor states of operation*/
typedef enum DcMotor_State {STOP , CW , A_CW} DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


void DcMotor_Init(void);

/* Description:
  * Control the DC Motor direction using L293D H-bridge.
  */
void DcMotor_Rotate(DcMotor_State state );

#endif /* DCMOTOR_H_ */

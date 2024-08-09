/*
 * DC_mot.c
 *
 *  Created on: Oct 5, 2022
 *      Author: Mohamed Asker
 */

#include "DC_mot.h"
#include "common_macros.h"
#include "PWM.h"
#include "gpio.h"


void DcMotor_Init(void)
{
	GPIO_setupPinDirection(DC_MOT_INT1_Port_ID, DC_MOT_INT1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOT_INT2_Port_ID, DC_MOT_INT2_PIN_ID, PIN_OUTPUT);

	GPIO_writePin(DC_MOT_INT1_Port_ID, DC_MOT_INT1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_MOT_INT2_Port_ID, DC_MOT_INT2_PIN_ID, LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	if(state == STOP)
	{
		GPIO_writePin(DC_MOT_INT1_Port_ID, DC_MOT_INT1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOT_INT2_Port_ID, DC_MOT_INT2_PIN_ID, LOGIC_LOW);
	}

	else if(state == ANTICLOCKWISE)
	{
		GPIO_writePin(DC_MOT_INT1_Port_ID, DC_MOT_INT1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOT_INT2_Port_ID, DC_MOT_INT2_PIN_ID, LOGIC_HIGH);
	}

	else if(state == CLOCKWISE)
	{
		GPIO_writePin(DC_MOT_INT1_Port_ID, DC_MOT_INT1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_MOT_INT2_Port_ID, DC_MOT_INT2_PIN_ID, LOGIC_LOW);
	}

	PWM_Timer0_Start(speed);

}

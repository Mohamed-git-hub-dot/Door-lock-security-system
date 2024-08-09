/*
 * PWM.c
 *
 *  Created on: Oct 6, 2022
 *      Author: Mohamed Asker
 */

#include"common_macros.h"
#include "PWM.h"
#include "avr/io.h"


void PWM_Timer0_Start(uint8 duty_cycle)
{
//	uint8 cycle = (uint8) (duty_cycle * 255);
//	cycle = cycle / 100;
	TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01) | (1 << CS00);
	TCNT0 = 0;
	OCR0 = (uint8) ( ( (uint32)duty_cycle*255 )/100) ;
	SET_BIT(DDRB,3);
}

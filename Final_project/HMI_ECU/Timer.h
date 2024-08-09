/*
 * Timer.h
 *
 *  Created on: Nov 1, 2022
 *      Author: Mohamed Asker
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

typedef enum
{
	No_clock,FCPU,FCPU_8,FCPU_64,FCPU_256,FCPU_1024
}Timer1_prescaler;

typedef enum
{
	NORMAL_MODE,CTC_MODE
}Timer1_Mode;

typedef struct
{
	uint16 initial_val;
	uint16 compare_val;
	Timer1_prescaler prescaler;
	Timer1_Mode mode;
}Timer1_configType;

void Timer1_init(const Timer1_configType* config_ptr);
void Timer1_deinit();
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER_H_ */

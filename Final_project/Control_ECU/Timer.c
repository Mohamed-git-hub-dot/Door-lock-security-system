/*
 * Author : Mohamed Asker
 */

#include "common_macros.h"
#include "avr/io.h"
#include "Timer.h"
#include "avr/interrupt.h"

static volatile void (*g_ptr)(void) = NULL_PTR;

void Timer1_init(const Timer1_configType* config_ptr)
{
	TCCR1A = (1 << FOC1A) | (1 << FOC1B);
	TCCR1B = (TCCR1B & 0xF8) | ( (config_ptr -> prescaler) & 0x07);
	TCNT1 =config_ptr -> initial_val;

	if(config_ptr -> mode == NORMAL_MODE)
	{
		TIMSK |= (1 << TOIE1);
	}

	else if(config_ptr -> mode == CTC_MODE)
	{
		TCCR1B |= (1 << WGM12);
		OCR1A = config_ptr -> compare_val;
		TIMSK |= (1 << OCIE1A);
	}

}

void Timer1_deinit()
{
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;
}

void Timer1_setCallBack(void(*a_ptr)(void))
{
	g_ptr = a_ptr;
}

ISR(TIMER1_OVF_vect)
{
	if(g_ptr != NULL_PTR)
	{
		(*g_ptr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_ptr != NULL_PTR)
	{
		(*g_ptr)();
	}
}


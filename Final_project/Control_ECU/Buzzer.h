
/*
 * Author : Mohamed Asker
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

#define BUZZER_PORT   PORTC_ID
#define BUZZER_PIN    PIN2_ID

void Buzzer_init();
void Buzzer_on(void);
void Buzzer_off(void);


#endif /* BUZZER_H_ */

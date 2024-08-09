/*
 * DC_mot.h
 *
 *  Created on: Oct 5, 2022
 *      Author: Mohamed Asker
 */

#ifndef DC_MOT_H_
#define DC_MOT_H_

#include "std_types.h"

#define DC_MOT_INT1_Port_ID     PORTD_ID
#define DC_MOT_INT1_PIN_ID      PIN2_ID

#define DC_MOT_INT2_Port_ID     PORTD_ID
#define DC_MOT_INT2_PIN_ID      PIN3_ID

typedef enum
{
	STOP,CLOCKWISE,ANTICLOCKWISE

}DcMotor_State;

void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state,uint8 speed);



#endif /* DC_MOT_H_ */

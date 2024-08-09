
#include "Buzzer.h"
#include "DC_mot.h"
#include "external_eeprom.h"
#include "twi.h"
#include "uart.h"
#include "Timer.h"
#include "gpio.h"
#include "avr/interrupt.h"
#include "util/delay.h"

#define MC2_READY               0x10
#define MC1_READY               0x20
#define PASSWORD_PASS           0xFF
#define PASSWORD_FAIL           0x00
#define FAIL                    0x30
#define OPEN                    0x50
#define CHANGE                  0x60
#define FINISHED_ROTATING_CW    0x70
#define FINISHED_DELAY          0x80
#define FINISHED_ROTATING_ACW   0x90
#define START_DELAY             0x40
#define START_ROTATE_ACW        0x32

uint8 g_tick = 0;
uint8 str1[10],str2[10],stored_pass[10],new_pass[10];


// Function that returns user for main menu (receive new string) //
void menu(void);

/* Function that make MC2 receive two strings and compare between them
 * if identical or not
 */

void compare(void);

/*Read The saved password from EEPROM //
 * Description
 * goes to the address in saved in EEPROM and store it in empty array
 */
void read_pass()
{
	for(int i = 0 ; i< 5 ; i++)
	{
		EEPROM_readByte(0x0311+i , &stored_pass[i]);
		//_delay_ms(10);
	}
}

/* Write The new password to EEPROM //
 * Description
 * goes to the address in EEPROM and store it in empty consecutive locations
 */

void save_pass()
{
	/*This loop saves the password in EEprom*/
	for(int i = 0 ; i< 5 ; i++)
	{
		EEPROM_writeByte(0x0311+i , str2[i]); //Save the password
		_delay_ms(100);
	}

	read_pass();
}
/* Check whether the two paswords entered from the user are identical or not
 * if identical, save in EEPROM.OTHERWISE,ask user to enter pass again
 */

uint8 check_pass(uint8 *pass1 , uint8 *pass2)
{
	uint8 flag = 0;

	for(uint8 i=0;i<5;i++)
	{
		if(pass1[i] != pass2[i])
		{
			flag=1;
		}
	}
	return flag;
}

/* Call back function for Motor clockwising
 * DESCRIPTION : responsible for calculate the time for motor to rotate clockwise
 */
void g_tickCounterMotor_CW()      // for FCPU=8MHZ , PRESCALER 64
{
	g_tick++;

	if(g_tick==30)
	{
		Timer1_deinit();                     // Turn off timer
		g_tick=0;                             // start g_ticks from 0 again
		UART_sendByte(FINISHED_ROTATING_CW);  // send flag to MC1 that motor finished rotating Clockwise
	}

}

/*
 * Description : Function to handle the time-dependant operation
 * Counts 3 seconds in which the motor is stopped
 */

void g_tickHoldMotor()   // for FCPU=8MHZ , PRESCALER 64
{
	g_tick++;

	if(g_tick==6)
	{
		Timer1_deinit();                 // Turn off timer
		g_tick=0;                        // start g_ticks from 0 again
		UART_sendByte(FINISHED_DELAY);   // send flag to MC1 that motor finished his holding delay
	}

}


/*
 * Description : Function to handle the time-dependant operation
 * Counts 15 seconds while closing the door then stops the motor
 */
void g_tickCounterMotor_ACW()   // for FCPU=8MHZ , PRESCALER 64
{
	g_tick++;

	if(g_tick==30)
	{
		Timer1_deinit();                       // Turn off timer
		g_tick=0;                              // start g_ticks from 0 again
		UART_sendByte(FINISHED_ROTATING_ACW);  // send flag to MC1 that motor finished rotating totally
		DcMotor_Rotate(STOP,0);                // Turn off motor
	}
}

int main()
{
	sei(); // Enbling Global interrupt enable //
	UART_configtype uart = {baud9600,Asynchronous_mode,Disabled,one_bit,eight_bits,Ignored};
	// Initializing UART to work at specifies frame for the two MCS //
	/* Initialize the TWI/I2C Driver */
	TWI_ConfigType twi = {0x01,400000};
	TWI_init(&twi);
	UART_init(&uart);

	Buzzer_init(); // initializing Buzzer pin //
	DcMotor_Init(); // initializing DC_motor pins //

	while(1)
	{
		compare();
	}

	return 0;
}
/*
 * Function to receive the new string from user and compare it with the one
 * saved in the EEPROM whether its matched or not
 */
void menu()
{
	static uint8 count = 0;
	uint8 flag2 = 0;
	uint8 temp;

	UART_sendByte(MC2_READY);   // tells the MC1 he is ready to receive string and receive it //
	UART_receiveString(new_pass);

	flag2 = check_pass(new_pass, stored_pass); // compare between the two pass //

	if(flag2 == 0)
	{
		count = 0;
		UART_sendByte(PASSWORD_PASS);
		temp = UART_recieveByte(); // flag to know if user want to open the door or change the pass//

		if(temp == OPEN)
		{
			Timer1_setCallBack(g_tickCounterMotor_CW);
			// send config. of timer
			Timer1_configType timer = {0,0,FCPU_64,NORMAL_MODE};
			Timer1_init(&timer);
			// Make motor rotate Clockwise for 15 sec
			DcMotor_Rotate(CLOCKWISE,60);


			if(UART_recieveByte() == START_DELAY)
			{
				Timer1_setCallBack(g_tickHoldMotor);
				// send config. of timer
				Timer1_init(&timer);
				// Make motor rotate Clockwise for 15 sec
				DcMotor_Rotate(STOP,0);
			}

			if(UART_recieveByte() == START_ROTATE_ACW)
			{
				Timer1_setCallBack(g_tickCounterMotor_ACW);
				// send config. of timer
				Timer1_init(&timer);
				// Make motor rotate Clockwise for 15 sec
				DcMotor_Rotate(ANTICLOCKWISE,60);
			}

			menu();
		}
		else if(temp == CHANGE)
		{
			compare();
		}
	}

	else
	{
		UART_sendByte(PASSWORD_FAIL);
		count++;

		// if 3 attempts expired for wrong password, activate buzzer for 1 min //

		if(count == 3)  // if count equal to 3 attempts, activate buzzer for 1 min//
		{
			Buzzer_on();

			for(uint8 i=0;i<60;i++)
				_delay_ms(1000);
			Buzzer_off();
			count = 0;
		}
		menu();
	}
}
/*
 * description : receive two strings and check whether identical or not //
 */
void compare(void)
{
	uint8 flag1 = 0;

	UART_sendByte(MC2_READY);
	UART_receiveString(str1);
	UART_receiveString(str2);

	flag1 = check_pass(str1,str2);

	if(flag1 == 1)
	{
		UART_sendByte(PASSWORD_FAIL);
		compare();
	}
	else
	{
		UART_sendByte(PASSWORD_PASS);
		save_pass();

		menu();
	}
}


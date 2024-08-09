
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "Timer.h"
#include "util/delay.h"

#define MC2_READY 0x10
#define MC1_READY 0x20
#define PASSWORD_PASS 0xFF
#define PASSWORD_FAIL 0x00
#define FAIL 0x30
#define OPEN 0x50
#define CHANGE 0x60
#define FINISHED_ROTATING_CW    0x70
#define FINISHED_DELAY          0x80
#define FINISHED_ROTATING_ACW   0x90
#define START_DELAY  0x40
#define START_ROTATE_ACW 0x32

void setup_password(void);
void open_door(void);
void change_password(void);
void enter_pass(void);
void Error(void);

uint8 g_edgecount = 0;
uint8 state = 0;
uint8 c = 0;
uint8 g_tick = 0;
uint8 currentstate = 0;


int main()
{
	UART_configtype uart = {baud9600,Asynchronous_mode,Disabled,one_bit,eight_bits,Ignored};
	UART_init(&uart);
	LCD_init();
	// initializing UART and LCD //

	while(1)
	{
		setup_password();
	}
	return 0;
}

/* Function that can be entered only if the entered password from user
 * doesnt match the correct password then he has 3 attempts only befoe
 * activating alarm
 */

void enter_pass()
{
	uint8 arr1[10];
	uint8 i,status;
	c++;

	if(c == 3)
	{
		Error();
	}

	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("plz enter pass: ");
	LCD_moveCursor(1, 0);

	for(i=0;i<5;i++)
	{
		arr1[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(400);
	}

	while(KEYPAD_getPressedKey() != '=') {}
	_delay_ms(200);

	arr1[i] = '#';
	arr1[i+1] = '\0';


	while(UART_recieveByte() != MC2_READY) {}
	UART_sendString(arr1);
	status = UART_recieveByte();

	if(status == PASSWORD_PASS)
	{
		open_door();
	}
	else
	{
		enter_pass();
	}
}

/* Function to display the main menu screen on LCD and waits user to enter
 * if he want to change pass or open the door
 * if open then asks for the password and sends it to MC2 to compare it
 * with the correct password
 */

void open_door(void)
{
	uint8 key = 0,i,status;
	uint8 arr1[10];

	c = 0;
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("+ : open Door");
	LCD_moveCursor(1,0);
	LCD_displayString("- : change pass");
	key = KEYPAD_getPressedKey();

	if(key == '+')
	{
		_delay_ms(200);
		LCD_clearScreen();
		LCD_moveCursor(0,0);
		LCD_displayString("enter pass plz: ");
		LCD_moveCursor(1, 0);

		for(i=0;i<5;i++)
		{
			arr1[i] = KEYPAD_getPressedKey();
			LCD_displayCharacter('*');
			_delay_ms(400);
		}
		while(KEYPAD_getPressedKey() != '=') {}
		_delay_ms(200);

		arr1[i] = '#';
		arr1[i+1] = '\0';


		while(UART_recieveByte() != MC2_READY) {}
		UART_sendString(arr1);
		status = UART_recieveByte();
		state = OPEN;

		if(status == PASSWORD_PASS)
		{
			UART_sendByte(state);

			/* print on screen that the door is unlocking anti clockwise
			 * until it finishes its delay for 3 seconds.
			*/

			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"Door unlocking");

			if(UART_recieveByte() == FINISHED_ROTATING_CW)
			{
				UART_sendByte(START_DELAY);
			}

			/* print on screen that the door is locking for 15 seconds
			 * then the motor turnes off.
			*/

			if(UART_recieveByte() == FINISHED_DELAY)
			{
				UART_sendByte(START_ROTATE_ACW);
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Door is locking");
				for(uint8 i=0;i<15;i++)
				{
					_delay_ms(1000);
				}
			}

			/* wait until motor finished rotating totally then it can go to main
		       menu again
			*/

			if(UART_recieveByte() == FINISHED_ROTATING_ACW)
			{

			}

			open_door();

		}
		else
		{
			enter_pass();
		}

	}
	else
	{
		change_password();
	}
}

/* Function if the user choose to change the password from main menu
 * it asks him first to Enter the correct password. if matched then
 * you can enter the new password. if not you have 3 attempts.
 */

void change_password(void)
{
	uint8 arr1[10];
	uint8 i,status;

	c = 0;
	_delay_ms(200);
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("plz enter pass: "); // tells the user that he awaits for password //
	LCD_moveCursor(1, 0);

	for(i=0;i<5;i++)
	{
		arr1[i] = KEYPAD_getPressedKey(); // receive input from string char by char //
		LCD_displayCharacter('*');
		_delay_ms(400);
	}
	while(KEYPAD_getPressedKey() != '=') {} // wait until it receives equal (Enter button) //
	_delay_ms(200);

	arr1[i] = '#';  // convert the array into string by add to it null terminator //
	arr1[i+1] = '\0'; // # character so i can send it as a string bu UART //

	while(UART_recieveByte() != MC2_READY) {}
	UART_sendString(arr1); // wait until MC1 gets Flag that MC2 is ready to receive //
	status = UART_recieveByte(); // send string to MC2 and wait for receive if its succes or fail //
	state = CHANGE;
	if(status == PASSWORD_PASS)
	{
		UART_sendByte(state);
		setup_password();
	}
	else
	{
		enter_pass();
	}

}

/* Function that display on the sceen to tell the user to enter the pass
 * then the confirmation password and receive inputs from keypad
 * then send them to MC2 to compare them.
 */

void setup_password()
{
	uint8 arr1[10],arr2[10];
	uint8 i,status;

	c = 0;
	LCD_clearScreen();
	LCD_moveCursor(0,0);
	LCD_displayString("plz enter pass: ");
	LCD_moveCursor(1, 0);

	for(i=0;i<5;i++)
	{
		arr1[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(400);
	}
	while(KEYPAD_getPressedKey() != '=') {}
	_delay_ms(200);

	arr1[i] = '#';
	arr1[i+1] = '\0';

	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "plz re-enter the");
	LCD_displayStringRowColumn(1, 0, "same pass:");

	for(i=0;i<5;i++)
	{
		arr2[i] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(400);
	}
	while(KEYPAD_getPressedKey()!= '=') {}
	_delay_ms(200);

	arr2[i] = '#';
	arr2[i+1] = '\0';

	while(UART_recieveByte() != MC2_READY) {}

	UART_sendString(arr1);   /* send the two strings to MC2 and compare the, there*/
	_delay_ms(5);
	UART_sendString(arr2);

	status = UART_recieveByte();

	if(status == PASSWORD_FAIL)
	{
		setup_password();
	}
	else
	{
		open_door();
	}
}

/*
 * Function that display on the screen Error message if the user entered wrong
 * pass for three consecutive times.
 */
void Error()
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Wrong password");

	for(uint8 i=0;i<60;i++)
		_delay_ms(1000);

	open_door();
}

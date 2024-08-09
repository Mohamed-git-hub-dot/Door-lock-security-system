 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Mohamed Asker
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"


typedef enum
{
	Asynchronous_mode=0,synchronous_mode=1
}UART_Mode_select;

typedef enum
{
	Disabled=0,Reserved=1,Even_parity=2,odd_parity=3
}UART_Parity_Mode;

typedef enum
{
	one_bit=0,two_bits=1
}UART_Stop_Bit;

typedef enum
{
	five_bits=0,six_bits=1,seven_bits=2,eight_bits=3,nine_bits=3
}UART_Char_size;

typedef enum
{
	Falling_edge=0,Rising_edge=1,Ignored=8
}UART_Clock_Polarity;

typedef enum
{
	baud10=10,baud300=300,baud600=600,baud1200=1200,baud2400=2400,baud4800=4800,\
	baud9600=9600,baud14400=14400,baud19200=19200,\
	baud38400=38400,baud57600=57600,baud115200=115200
}UART_BAUDrate;

typedef struct
{
	UART_BAUDrate BAUDRATE;
	UART_Mode_select mode;
	UART_Parity_Mode parity;
	UART_Stop_Bit bit;
	UART_Char_size size;
	UART_Clock_Polarity polarity;

}UART_configtype;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(UART_configtype* config_ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */

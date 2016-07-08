/*
 * uart_atmega32.c
 *
 *  Created on: 06.07.2016
 *      Author: Michail Kurochkin
 */

#include <avr/wdt.h>
#include <stdio.h>
#include "types.h"
#include "uart.h"

int usart_send_byte(struct uart *uart, u8 byte)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = byte;
	return 0;
}

u8 usart_get_byte(struct uart *uart)
{
	if ((UCSRA & (1 << RXC)))
		return UDR;
	return 0;
}

u8 usart_get_blocked(struct uart *uart)
{
	while (!(UCSRA & (1 << RXC)));
	return UDR;
}

static int serial_out(char byte)
{
	while(!(UCSRA & (1 << UDRE)));
	UDR = byte;
	return 0;
}

static char serial_in(void)
{
	while(!(UCSRA & (1 << RXC)));
	return UDR;
}


int usart_init(struct uart *uart)
{
	if (uart->chip_id != 0)
		return -EINVAL;

	u16 ubrr = F_CPU / 16 / uart->baud_rate - 1;

	UBRRH = (u8) (ubrr >> 8);
	UBRRL = (u8) ubrr;
	UCSRB = (1 << RXEN) | (1 << TXEN);

	UCSRC = (1 << URSEL) | (3 << UCSZ0);

	if (uart->fdev_type)
		fdevopen((void *) serial_out, (void *) serial_in);
	return 0;
}


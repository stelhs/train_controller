/*
 * uart_atmega32.c
 *
 *  Created on: 06.07.2016
 *      Author: Michail Kurochkin
 */

#include "uart.h"

#include <stdio.h>
#include <avr/io.h>

/**
 * Вывод в RS232 порт 0
 * @param var - символ для отправки
 */
int usart_send_byte(struct uart *uart, u8 byte)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = byte;
	return 0;
}

/**
 * Чтение из RS232 порт 0
 * @return Ожидает и возвращает прочитанный символ
 */
static char
usart_get_blocked(struct uart *uart)
{
	while (!(UCSRA & (1 << RXC)));
	return UDR;
}

/**
 * Чтение из RS232 порт 0
 * @return возвращает символ из внутреннего буфера
 */
char
usart_get_byte(struct uart *uart)
{
	if ((UCSRA & (1 << RXC)))
		return UDR;
	return 0;
}


/**
 * Инициалтизация RS232 порта 0
 * @param ubrr - скорость BAUD RATE
 */
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
		fdevopen((void *) usart_send_byte, (void *) usart_get_blocked);
}


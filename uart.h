/*
 * serial.h
 *
 *  Created on: 25.01.2014
 *      Author: Michail Kurochkin
 */

#ifndef UART_H_
#define UART_H_

#include "types.h"

#define SERIAL_BAUD_RATE				9600

struct uart {
	u8 chip_id;
	int baud_rate;
	u8 fdev_type: 1; // set uart for output printf() or not
};

int usart_init(struct uart *uart);

u8 usart_get_byte(struct uart *uart);
u8 usart_get_blocked(struct uart *uart);

#endif /* UART_H_ */

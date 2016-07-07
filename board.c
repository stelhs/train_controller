/*
 * init.c
 *
 *  Created on: 11.02.2012
 *      Author: Michail Kurochkin
 */

#include <stdio.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "gpio.h"
#include "config.h"
#include "board.h"
#include "uart.h"


struct gpio gpio_list[] = {
	{ // traction +1
		.direction_addr = (u8 *) &DDRA,
		.port_addr = (u8 *) &PORTA,
		.pin = 3,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // 0 (reset/reverse)
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 4,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // traction -1
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 5,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // gerkon
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 6,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // tahogenerator
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin = 3,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // input 100Hz
		.direction_addr = (u8 *) &DDRA,
		.port_addr = (u8 *) &PORTA,
		.pin = 4,
		.direction = GPIO_OUTPUT,
		.pull_up = 1
	},
	{ // led ready
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin = 2,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // led error
		.direction_addr = (u8 *) &DDRB,
		.port_addr = (u8 *) &PORTB,
		.pin = 0,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // led reverse
		.direction_addr = (u8 *) &DDRB,
		.port_addr = (u8 *) &PORTB,
		.pin = 1,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // led traction
		.direction_addr = (u8 *) &DDRB,
		.port_addr = (u8 *) &PORTB,
		.pin = 2,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // pwm speedometer
		.direction_addr = (u8 *) &DDRB,
		.port_addr = (u8 *) &PORTB,
		.pin = 3,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // traction forward
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 0,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // traction backward
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 1,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // left traction
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin = 2,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{ // right traction
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin = 4,
		.direction = GPIO_OUTPUT,
		.output_state = 0
	},
	{
		.direction_addr = NULL,
		.port_addr = NULL,
	},
};

struct led led_ready = {
	.gpio = gpio_list + 6
};

struct led led_error = {
	.gpio = gpio_list + 7
};

struct led led_reverse = {
	.gpio = gpio_list + 8
};

struct led led_traction = {
	.gpio = gpio_list + 9
};

struct gpio_input input_gerkon = {
	.gpio = gpio_list + 3,
	.on_change = gerkon_changed
};

struct gpio_key traction_up = {
	.input = {
		.gpio = gpio_list + 0
	},
	.on_click = traction_up_handler
};

struct gpio_key traction_down = {
	.input = {
		.gpio = gpio_list + 2
	},
	.on_click = traction_down_handler
};

struct gpio_key traction_reset = {
	.input = {
		.gpio = gpio_list + 1
	},
	.on_click = traction_reset_handler
};

struct uart console = {
	.chip_id = 0,
	.baud_rate = 9600,
	.fdev_type = 1
};



/**
 * Init board hardware
 */
void board_init(void)
{
	gpio_register_list(gpio_list);
	led_register(&led_ready);
	led_register(&led_error);
	led_register(&led_reverse);
	led_register(&led_traction);

	gpio_debouncer_register_input(&input_gerkon);

	gpio_keys_register_key(&traction_up);
	gpio_keys_register_key(&traction_down);
	gpio_keys_register_key(&traction_reset);

	usart_init(&console);
	wdt_enable(WDTO_2S); // Включаем вэтчдог
	sei();
}


/*
 * init.c
 *
 *  Created on: 11.02.2012
 *      Author: Michail Kurochkin
 */

#include <stdio.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "board.h"

#include "ac_motors.h"
#include "types.h"
#include "idle.h"
#include "gpio.h"
#include "leds.h"
#include "config.h"
#include "uart.h"
#include "ac_motors.h"
#include "train_controller.h"
#include "speedometer.h"
#include "eeprom_fs.h"

struct gpio gpio_list[] = {
	{ // traction +1
		.direction_addr = (u8 *) &DDRA,
		.port_addr = (u8 *) &PORTA,
		.pin_addr = (u8 *) &PINA,
		.pin = 3,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // 0 (reset/reverse)
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin_addr = (u8 *) &PINC,
		.pin = 4,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // traction -1
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin_addr = (u8 *) &PINC,
		.pin = 5,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // gerkon
		.direction_addr = (u8 *) &DDRC,
		.port_addr = (u8 *) &PORTC,
		.pin_addr = (u8 *) &PINC,
		.pin = 6,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // tahogenerator
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin_addr = (u8 *) &PIND,
		.pin = 3,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // input 100Hz
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin_addr = (u8 *) &PIND,
		.pin = 2,
		.direction = GPIO_INPUT,
		.pull_up = 1
	},
	{ // led ready
		.direction_addr = (u8 *) &DDRA,
		.port_addr = (u8 *) &PORTA,
		.pin = 4,
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
		.output_state = 1
	},
	{ // right traction
		.direction_addr = (u8 *) &DDRD,
		.port_addr = (u8 *) &PORTD,
		.pin = 4,
		.direction = GPIO_OUTPUT,
		.output_state = 1
	},
	{ // balance regulator
		.direction_addr = (u8 *) &DDRA,
		.port_addr = (u8 *) &PORTA,
		.pin_addr = (u8 *) &PINA,
		.pin = 0,
		.direction = GPIO_INPUT,
		.pull_up = 0
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

struct uart console = {
	.chip_id = 0,
	.baud_rate = 9600,
	.fdev_type = 1
};

struct ac_motor motor_left = {
	.semistor = gpio_list + 13,
	.power_forward = gpio_list + 11,
	.power_backward = gpio_list + 12,
};

struct ac_motor motor_right = {
	.semistor = gpio_list + 14,
	.power_forward = gpio_list + 11,
	.power_backward = gpio_list + 12,
};

/* Run from ac_motor when external power is loss.
 * Run from IRQ. */
void callback_external_power_loss(void)
{
	led_off(&led_error);
	led_off(&led_ready);
	led_off(&led_reverse);
	led_off(&led_traction);
	odometer_save_state();
	train_controller_save_state();
	led_on(&led_error);
	led_on(&led_ready);
	led_on(&led_reverse);
	led_on(&led_traction);
}


/**
 * Init board hardware
 */
static void board_init(void)
{
	int rc;
	u16 power_on_cnt;

	sys_timer_init();
	usart_init(&console);

	gpio_register_list(gpio_list);
	led_register(&led_ready);
	led_register(&led_error);
	led_register(&led_reverse);
	led_register(&led_traction);

	ac_motors_subsystem_init();
	ac_motor_register(&motor_left);
	ac_motor_register(&motor_right);

	eeprom_init_fs();
	speedometer_init();
	train_controller_init();

	rc = eeprom_read_file("poweron", (u8 *)&power_on_cnt);
	if (rc < 0) {
		eeprom_create_file("poweron", sizeof(power_on_cnt));
		power_on_cnt = 0;
	}
	power_on_cnt++;
	eeprom_write_file("poweron", (u8 *)&power_on_cnt);

	wdt_enable(WDTO_2S);

	sei();
}


int main(void)
{
	board_init();
	printf("Init - ok\r\n");
	for(;;)
		idle();
}


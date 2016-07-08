/*
 * traction_motor.c
 *
 *  Created on: 08 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include "ac_motors.h"

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "list.h"
#include "gpio.h"

static struct list list_motors = LIST_INIT;

/* maximum interval between start waveform and enable semistor.
 * That means the minimum motor power */
#define SEMISTOR_MAX_INTERVAL 240

/* minimum interval between start waveform and enable semistor.
 * That means the maximum motor power.  */
#define SEMISTOR_MIN_INTERVAL 32

/* semistor active front interval in timer0 ticks */
#define SEMISTOR_PULSE_INTERVAL 5



/* input AC signal 100Hz irq handler */
ISR(INT0_vect)
{
	struct le *le;
	LIST_FOREACH(&list_motors, le) {
		struct ac_motor *motor = list_ledata(le);

		if (motor->full_power)
			continue;

		gpio_set_value(motor->semistor, OFF);
		motor->semistor_counter = motor->semistor_interval;
	}
}

/* timer0 irq handler */
ISR(TIMER0_COMP_vect)
{
	struct le *le;
	LIST_FOREACH(&list_motors, le) {
		struct ac_motor *motor = list_ledata(le);

		if (motor->semistor_counter > 1)
			motor->semistor_counter--;

		if (motor->semistor_counter == SEMISTOR_PULSE_INTERVAL)
			gpio_set_value(motor->semistor, ON);

		if (motor->semistor_counter == 1) {
			gpio_set_value(motor->semistor, OFF);
			motor->semistor_counter = 0;
		}
	}
}


/**
 * Register AC motor
 * @param motor - motor descriptor with initialized all GPIO fields
 */
void ac_motor_register(struct ac_motor *motor)
{
	cli();
	ac_motor_disable(motor);
	gpio_set_value(motor->semistor, OFF);
	motor->semistor_interval = 0;
	motor->semistor_counter = 0;
	motor->full_power = 0;
	list_append(&list_motors, &motor->le, motor);
	sei();
}


/**
 * Set motor power
 * @param motor - motor descriptor
 * @param power - power value range between 0 to 100%
 */
void ac_motor_set_power(struct ac_motor *motor, u8 power)
{
	u16 range = (SEMISTOR_MAX_INTERVAL - SEMISTOR_MIN_INTERVAL);

	if (power == 100) {
		cli();
		motor->semistor_interval = 0;
		motor->semistor_counter = 0;
		motor->full_power = 1;
		gpio_set_value(motor->semistor, ON);
		sei();
		return;
	}

	motor->full_power = 0;
	power = 100 - power; /* invert power */
	motor->semistor_interval = power * 100 / range + SEMISTOR_MIN_INTERVAL;
}


void ac_motor_enable_forward(struct ac_motor *motor)
{
	gpio_set_value(motor->power_backward, OFF);
	gpio_set_value(motor->power_forward, ON);
}

void ac_motor_enable_bacward(struct ac_motor *motor)
{
	gpio_set_value(motor->power_forward, OFF);
	gpio_set_value(motor->power_backward, ON);
}

void ac_motor_disable(struct ac_motor *motor)
{
	gpio_set_value(motor->power_backward, OFF);
	gpio_set_value(motor->power_forward, OFF);
}

/**
 * Must be placed into init section
 */
void ac_motors_subsystem_init(void)
{
	/* configure external interrupts for Int0 */
	MCUCR |= _BV(ISC01);
	GICR |= _BV(INT0);
	GIFR |= _BV(INT0);

	/* configure timer0 to highest frequency */
	TCCR0 =  _BV(WGM01) | _BV(WGM00) | _BV(COM01) | _BV(CS00);
	OCR0 = 0;
	TIMSK = 1 << TOIE0;	\
}
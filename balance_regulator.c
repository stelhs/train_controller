/*
 * balance_regulator.c
 *
 *  Created on: 08 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include <avr/interrupt.h>
#include "types.h"
#include "balance_regulator.h"
#include "sys_timer.h"

static volatile s16 adc_value = 512;

ISR(ADC_vect)
{
	adc_value = (ADCH << 8) | ADCL;
}

static void timer_handler(void *arg)
{
	struct balance_regulator *balance = (struct balance_regulator *)arg;

	balance->value = (adc_value - 512) * 50 / 512;
	if (balance->value != balance->prev_value && balance->on_change)
		balance->on_change(balance->value);

	/* start ADC balance regulator value*/
	ADCSRA |= _BV(ADSC);
}


void balance_regulator_init(struct balance_regulator *balance)
{
	/* configure ADC */
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS2)
				| _BV(ADPS1) | _BV(ADPS0);
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX1);
	SREG |= _BV(SREG_I);

	balance->prev_value = balance->value = 0;

	balance->timer.devisor = 10;
	balance->timer.handler = timer_handler;
	balance->timer.priv = balance;
	sys_timer_add_handler(&balance->timer);
}

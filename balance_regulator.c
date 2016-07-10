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

static volatile s32 adc_value = 512;

ISR(ADC_vect)
{
	adc_value = ADCL | (ADCH << 8);

	/* start ADC balance regulator value*/
	ADCSRA |= _BV(ADSC);
}

static void idle_handler(void *arg)
{
	s32 val;
	struct balance_regulator *balance = (struct balance_regulator *)arg;

	cli();
	val = adc_value;
	sei();

	balance->value = ((val - 512) * 50 / 512) * 2;
	if (balance->value != balance->prev_value && balance->on_change) {
		balance->on_change(balance->priv, balance->value);
		balance->prev_value = balance->value;
	}
}


void balance_regulator_init(struct balance_regulator *balance)
{
	/* configure ADC */
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS0)
				| _BV(ADPS1) | _BV(ADPS2);
	ADMUX = _BV(REFS1) | _BV(REFS0);
	SREG |= _BV(SREG_I);

	balance->prev_value = balance->value = 0;

	balance->wrk.handler = idle_handler;
	balance->wrk.priv = balance;
	sys_idle_add_handler(&balance->wrk);

	/* start ADC balance regulator value*/
	ADCSRA |= _BV(ADSC);
}

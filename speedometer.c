/*
 * speedometer.c
 *
 *  Created on: 10 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "list.h"
#include "gpio.h"

#define SPEED_INDICATOR_MAX 40 /* max speed value Km/h */
#define SPEED_INDICATOR_PWM_MAX 235 /* max PWD output value */


/* input tahogenerator signal */
ISR(INT1_vect)
{
	printf("x\r\n");
}

/**
 * set speedomenetr indicator value
 * @param speed - speed value beetween
 */
void speed_indicator_set(u8 speed)
{
	if (speed > SPEED_INDICATOR_MAX + 3)
		speed = SPEED_INDICATOR_MAX + 3;

	OCR0 = ((u32)speed * SPEED_INDICATOR_PWM_MAX) / SPEED_INDICATOR_MAX;
}


void speedometer_init(void)
{
	/* configure external interrupts for Int1 */
	MCUCR |= _BV(ISC10);
	GICR |= _BV(INT1);
}



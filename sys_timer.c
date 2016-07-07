/*
 * sys_timer.c
 *
 *  Created on: 07 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include "sys_timer.h"

static struct list list_subscribers = LIST_INIT;

u32 jiffies = 0;

SIGNAL(SIG_OUTPUT_COMPARE2)
{
	struct le *le;
	jiffies++;

	LIST_FOREACH(&list_subscribers, le) {
		struct sys_timer *timer = list_ledata(le);

		if (!(jiffies % timer->devisor))
			timer->handler(timer->priv);
	}
}

void sys_timer_add_handler(struct sys_timer *timer)
{
	list_append(&list_subscribers, &timer->le, timer);
}

void sys_timer_init(void)
{
	OCR2 = TIMER2_DELAY;
	TCCR2 = 0b011;
	TIFR |= OCF2;
	TIMSK |= _BV(OCIE2);
}

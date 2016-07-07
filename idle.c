/*
 * idle.c
 *
 *  Created on: 07 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include "list.h"
#include "idle.h"

static struct list list_subscribers = LIST_INIT;


/**
 * Must be placed into main cycle
 */
void idle(void)
{
	LIST_FOREACH(&list_subscribers, le) {
		struct sys_idle *tsk = list_ledata(le);
		tsk->handler(tsk->priv);
	}
}

void sys_idle_add_handler(struct sys_wrk *wrk)
{
	list_append(&list_subscribers, &wrk->le, wrk);
}

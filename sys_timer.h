/*
 * sys_timer.h
 *
 *  Created on: 07 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

struct sys_timer {
	int devisor;
	void *priv;
	void (*handler)(void *);
// Private:
	struct le le;
};

#endif /* SYS_TIMER_H_ */

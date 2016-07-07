/*
 * idle.h
 *
 *  Created on: 07 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef IDLE_H_
#define IDLE_H_

struct sys_work {
	void *priv;
	void (*handler)(void *);
// Private:
	struct le le;
};


#endif /* IDLE_H_ */

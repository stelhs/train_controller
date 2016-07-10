/*
 * balance_regulator.h
 *
 *  Created on: 08 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef BALANCE_REGULATOR_H_
#define BALANCE_REGULATOR_H_

#include "types.h"
#include "idle.h"

struct balance_regulator {
	s16 value; /* from -100% to 100% */
	s16 prev_value;
	struct sys_work wrk;
	void (*on_change)(void *, s16 value);
	void *priv;
};

void balance_regulator_init(struct balance_regulator *balance);

#endif /* BALANCE_REGULATOR_H_ */

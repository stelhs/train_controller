/*
 * balance_regulator.h
 *
 *  Created on: 08 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef BALANCE_REGULATOR_H_
#define BALANCE_REGULATOR_H_

#include "types.h"
#include "sys_timer.h"

struct balance_regulator {
	s16 value; /* from -50% to 50% */
	s16 prev_value;
	struct sys_timer timer;
	void (*on_change)(s16 value);
};

void balance_regulator_init(struct balance_regulator *balance);

#endif /* BALANCE_REGULATOR_H_ */

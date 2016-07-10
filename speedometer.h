/*
 * speedometer.h
 *
 *  Created on: 10 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef SPEEDOMETER_H_
#define SPEEDOMETER_H_

#include "types.h"
#include "sys_timer.h"

struct speedometer {
	struct sys_timer timer;
	u16 speed;
	u16 taho_counter;
};

void speedometer_init(void);
void speedometer_indicator_set(u8 speed);
u8 speedometer_get_speed(void);

#endif /* SPEEDOMETER_H_ */

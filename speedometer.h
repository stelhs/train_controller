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
#include "idle.h"

struct speedometer {
	struct sys_timer speed_timer;
	struct sys_timer odometer_timer;
	struct sys_work wrk;
	u16 speed; /* Speed in tick per timer */
	u8 speed_km; /* Speed in km/hour */
	u8 speed_m; /* Speed in m/sec */
	u16 taho_counter;
	u32 distance; /* distance in meters (no more 1000 meters) */
	u16 distance_km; /* distance in kilometers */
	u8 save_distance_flag :1;

};

void speedometer_init(void);
void speedometer_indicator_set(u8 speed);
u8 speedometer_get_speed(void);

#endif /* SPEEDOMETER_H_ */

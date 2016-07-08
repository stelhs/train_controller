/*
 * traction_motor.h
 *
 *  Created on: 08 июля 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef AC_MOTORS_H_
#define AC_MOTORS_H_

#include "types.h"
#include "list.h"
#include "gpio.h"

struct ac_motor {
	struct gpio *semistor;
	struct gpio *power_forward;
	struct gpio *power_backward;
// Private:
	struct le le;
	u16 semistor_interval;
	u16 semistor_counter;
	u8 full_power :1;
};

void ac_motor_register(struct ac_motor *motor);
void ac_motor_set_power(struct ac_motor *motor, u8 power);
void ac_motor_enable_forward(struct ac_motor *motor);
void ac_motor_enable_bacward(struct ac_motor *motor);
void ac_motor_disable(struct ac_motor *motor);
void ac_motors_subsystem_init(void);

#endif /* AC_MOTORS_H_ */

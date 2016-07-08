/*
 * train_controller.h
 *
 *  Created on: 08 jule 2016 г.
 *      Author: Michail Kurochkin
 */
#ifndef TRAIN_CONTROLLER_H_
#define TRAIN_CONTROLLER_H_

#include "types.h"

void gerkon_state_changed(void *arg);
void traction_up_handler(void *arg);
void traction_down_handler(void *arg);
void traction_reset_handler(void *arg);
void traction_reverse_handler(void *arg, t_counter hold_counter);
void power_balance_regulator_changed(s16 value);

#endif /* TRAIN_CONTROLLER_H_ */

/*
 * train_controller.c
 *
 *  Created on: 08 jule 2016 г.
 *      Author: Michail Kurochkin
 */

#include <stdio.h>
#include <math.h>
#include "train_controller.h"
#include "board.h"
#include "types.h"

void gerkon_state_changed(void *arg)
{
	printf("gerkon changed\r\n");
}

void traction_up_handler(void *arg)
{
	printf("traction_up\r\n");
	ac_motor_enable_bacward(&motor_left);
}

void traction_down_handler(void *arg)
{
	printf("traction_down\r\n");
	ac_motor_enable_forward(&motor_left);
}

void traction_reset_handler(void *arg)
{
	printf("traction_reset\r\n");
	ac_motor_disable(&motor_left);

}

void traction_reverse_handler(void *arg, t_counter hold_counter)
{
	printf("traction_reverse\r\n");
}

void power_balance_regulator_changed(s16 value)
{
	printf("bal=%d\r\n", value);
	printf("power=%d\r\n", abs(value));
	ac_motor_set_power(&motor_left, abs(value));
	ac_motor_set_power(&motor_right, abs(value));
}

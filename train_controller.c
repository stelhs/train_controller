/*
 * train_controller.c
 *
 *  Created on: 08 jule 2016 г.
 *      Author: Michail Kurochkin
 */

#include <stdio.h>
#include "train_controller.h"
#include "types.h"


void gerkon_state_changed(void *arg)
{
	printf("gerkon changed\r\n");
}

void traction_up_handler(void *arg)
{
	printf("traction_up\r\n");
}

void traction_down_handler(void *arg)
{
	printf("traction_down\r\n");
}

void traction_reset_handler(void *arg)
{
	printf("traction_reset\r\n");
}

void traction_reverse_handler(void *arg, t_counter hold_counter)
{
	printf("traction_reverse\r\n");
}

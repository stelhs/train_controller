/*
 * init.h
 *
 *  Created on: 07.07.2016
 *      Author: Michail Kurochkin
 */

#ifndef INIT_H_
#define INIT_H_

#include "gpio.h"
#include "leds.h"
#include "ac_motors.h"

#define CLEAR_WATCHDOG() { asm("wdr"); }

extern struct gpio gpio_list[];
extern struct led led_ready;
extern struct led led_error;
extern struct led led_reverse;
extern struct led led_traction;
extern struct ac_motor motor_left;
extern struct ac_motor motor_right;

void callback_external_power_loss(void);

#endif /* INIT_H_ */

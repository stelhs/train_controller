/*
 * init.h
 *
 *  Created on: 07.07.2016
 *      Author: Michail Kurochkin
 */

#ifndef INIT_H_
#define INIT_H_

#define CLEAR_WATCHDOG() { asm("wdr"); }

extern struct led led_ready;
extern struct led led_error;
extern struct led led_reverse;
extern struct led led_traction;
extern struct ac_motor motor_left;
extern struct ac_motor motor_right;

#endif /* INIT_H_ */

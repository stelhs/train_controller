/*
 * init.h
 *
 *  Created on: 07.07.2016
 *      Author: Michail Kurochkin
 */

#ifndef INIT_H_
#define INIT_H_

#include "gpio.h"
#include "types.h"

void board_init(void);

#define CLEAR_WATCHDOG() { asm("wdr"); }

extern struct led led_list[];
#define LED_READY (led_list + 0)
#define LED_ERROR (led_list + 1)
#define LED_REVERSE (led_list + 2)
#define LED_TRACTION (led_list + 3)

#endif /* INIT_H_ */

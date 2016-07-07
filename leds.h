#ifndef LED_LIB_H
#define LED_LIB_H

#include "types.h"

struct led
{
	struct le le;
	struct gpio *gpio;
	u8 state : 1; // current state
	t_counter blink_timer; // timer counter
	t_counter interval1; // active interval
	t_counter interval2; // inactive interval
	int blink_counter; //
};

void leds_init(struct led *leds);

void led_on(struct led *led);
void led_off(struct led *led);
void led_set_blink(struct led *led, t_counter interval1,
			t_counter interval2, int count);

void leds_update(void);

#endif

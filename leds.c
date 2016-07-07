#include "types.h"
#include "leds.h"

static struct list *list_leds = LIST_INIT;

/**
 * Register new led
 * @param led - struct allocated not in stack with one necessary parameter: gpio
 */
void led_register(struct led *led)
{
	gpio_set_state(led->gpio, OFF);
	led->blink_timer = 0;
	led->interval1 = 0;
	led->interval2 = 0;
	list_append(&list_leds, &led->le, led);
}

/**
 * enable led light
 */
void led_on(struct led *led)
{
	gpio_set_state(led->gpio, ON);
	led->interval1 = 0;
	led->interval2 = 0;
	led->blink_timer = 0;
}

/**
 * disable led light
 */
void led_off(struct led *led)
{
	gpio_set_state(led->gpio, OFF);
	led->interval1 = 0;
	led->interval2 = 0;
	led->blink_timer = 0;
}

/**
 * set led blinking mode.
 * @param led - led descriptor
 * @param interval1 - light interval
 * @param interval2 - darkness interval
 * @param count - blink count. 0 for not limit
 */
void led_set_blink(struct led *led, t_counter interval1,
			t_counter interval2, int count)
{
	if(interval2 == 0)
		interval2 = interval1;

	gpio_set_state(led->gpio, ON);
	led->interval1 = interval1;
	led->interval2 = interval2;
	led->blink_timer = interval1 + 1;
	led->blink_counter = count ? (count + 1) : 0;
}


/**
 * Update led timers. Function must be integrate into timer ISR callback.
 */
void leds_update(void)
{
	struct le *le;

	LIST_FOREACH(&list_leds, le) {
		struct led *led = list_ledata(le);
		if(led->blink_timer > 1)
			led->blink_timer--;

		if(led->blink_timer > 1 || led->interval1 == 0)
			continue;

		if (led->blink_counter == 1) {
			gpio_set_state(led->gpio, OFF);
			led->blink_timer = 0;
			led->state = 0;
			return;
		}

		if(led->state) {
			gpio_set_state(led->gpio, OFF);
			led->blink_timer = led->interval2 + 1;
		} else  {
			gpio_set_state(led->gpio, ON);
			led->blink_timer = led->interval1 + 1;
			if (led->blink_counter > 1)
				led->blink_counter--;
		}
	}
}

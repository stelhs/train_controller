#ifndef GPIO_KEYS_H
#define GPIO_KEYS_H

#include "types.h"
#include "gpio.h"
#include "gpio_debouncer.h"

#define GPIO_KEY_CLICK_INTERVAL 500
#define GPIO_KEY_HOLD_INTERVAL 1000

struct gpio_key
{
	struct gpio_input input;
	void (*on_press_down)(struct gpio_key *);
	void (*on_press_up)(struct gpio_key *);
	void (*on_click)(struct gpio_key *);
	void (*on_hold)(struct gpio_key *, t_counter hold_counter);

// Private:
	struct sys_timer timer;
	struct sys_work wrk;
	u8 prev_state :1;
	u8 press_down_action :1;
	u8 press_up_action :1;
	u8 click_action :1;
	u8 hold_action :1;

	t_counter click_timer;
	t_counter hold_timer;
	t_counter hold_counter;
};

void gpio_keys_register_key(struct gpio_key *key);

#endif // GPIO_KEYS_H

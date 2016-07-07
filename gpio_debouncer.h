#ifndef GPIO_DEBOUNCER_H
#define GPIO_DEBOUNCER_H

struct gpio_input
{
	struct le le;
	struct gpio *gpio;
	void (*on_change)(struct gpio_input *);
	u8 stable_state :1;
	u8 changed :1;
	u8 prev_state :1;
	t_counter debounce_interval;
	t_counter debounce_counter;
};


#endif

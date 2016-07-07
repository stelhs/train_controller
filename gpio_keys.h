#ifndef GPIO_KEYS_H
#define GPIO_KEYS_H

#include "types.h"

struct gpio_key
{
	struct le le;
	struct gpio_input input;
	void (*on_press_down)(struct gpio_key *);
	void (*on_press_up)(struct gpio_key *);
	void (*on_click)(struct gpio_key *);
	void (*on_double_click)(struct gpio_key *);
	void (*on_hold)(struct gpio_key *);

};


#endif // GPIO_KEYS_H

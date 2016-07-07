#include "gpio_keys.h"


static struct list list_keys = LIST_INIT;


void gpio_keys_register_key(struct gpio_key *key)
{
	gpio_debouncer_register_input(&key->input);
	list_append(&list_keys, &key->le, key);
}


void gpio_key_update(void)
{
	
}






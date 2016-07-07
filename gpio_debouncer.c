#include "list.h"
#include "gpio_debouncer.h"

static struct list gpio_inputs = LIST_INIT;

void gpio_debouncer_register_input(struct gpio_input *input)
{
	input->changed = 0;
	input->debounce_counter = 0;
	input->prev_state = 0;
	input->stable_state = 0;
	list_append(&gpio_inputs, &input->le, input);
}

/**
 * Must be placed into timer ISR
 */
void gpio_debouncer_update(void)
{
	struct le *le;
	u8 curr_state;

	LIST_FOREACH(&gpio_inputs, le) {
		struct gpio_input *input = list_ledata(le);
		curr_state = gpio_get_state(input->gpio);
		if (input->prev_state != curr_state) {
			input->debounce_counter = input->debounce_interval;
			curr_state = input->prev_state;
		}

		if (input->debounce_counter > 1)
			input->debounce_counter --;

		if (input->debounce_counter == 1) {
			input->stable_state = curr_state;
			input->changed = 1;
			input->debounce_counter = 0;
		}
	}
}

/**
 * Must be placed into main cycle
 */
void gpio_debouncer_check(void)
{
	struct gpio_input *input;
	u8 curr_state;

	for(input = input_list; input->gpio; input++) {
		if (input->on_change && input->changed) {
			input->on_change(input);
			input->changed = 0;
		}
	}
}

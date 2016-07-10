/*
 * train_controller.c
 *
 *  Created on: 08 jule 2016 г.
 *      Author: Michail Kurochkin
 */

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "types.h"
#include "train_controller.h"
#include "board.h"
#include "sys_timer.h"
#include "idle.h"
#include "speedometer.h"
#include "ac_motors.h"
#include "gpio_debouncer.h"
#include "gpio_keys.h"
#include "balance_regulator.h"
#include "leds.h"

#define STATIC_TRACTION_BALANCE 0

/**
 * Table of power each position
 */
static u8 position_power_table[] = {
	0,
	10,
	30,
	50,
	75,
	100,
};

/**
 * Train motions states
 */
enum train_motions {
	TRAIN_STOPPED = -2,      //!< TRAIN_STOPPED
	TRAIN_IDLE = -1,         //!< TRAIN_IDLE
	TRAIN_RESET_POSITION = 0,//!< TRAIN_RESET_POSITION
	TRAIN_POSITION_1 = 1,    //!< TRAIN_POSITION_1
	TRAIN_POSITION_2,        //!< TRAIN_POSITION_2
	TRAIN_POSITION_3,        //!< TRAIN_POSITION_3
	TRAIN_POSITION_4,        //!< TRAIN_POSITION_4
	TRAIN_POSITION_LAST,     //!< TRAIN_POSITION_LAST
};

struct train_controller {
	struct ac_motor *motor_left;
	struct ac_motor *motor_right;
	struct led *led_ready;
	struct led *led_error;
	struct led *led_reverse;
	struct led *led_traction;
	u8 ready :1;
	u8 prev_ready :1;
	u8 reverse :1;
	enum train_motions moution_state;
	u8 power;
	s16 balance;
	struct sys_timer timer;
	struct sys_work wrk;
};


/**
 * run once at second
 */
void train_controller_timer(void *arg)
{
	struct train_controller *tc = (struct train_controller *)arg;

	if (tc->moution_state > TRAIN_RESET_POSITION &&
		tc->moution_state < TRAIN_POSITION_LAST) {
		led_set_blink(tc->led_traction, 50, 200, tc->moution_state);
	}

	if (tc->moution_state == TRAIN_POSITION_LAST) {
		led_on(tc->led_traction);
	}
}


void train_controller_work(void *arg)
{
	struct train_controller *tc = (struct train_controller *)arg;
	u8 speed = speedometer_get_speed();

	CLEAR_WATCHDOG();

	if (tc->moution_state == TRAIN_RESET_POSITION && speed == 0) {
		ac_motor_disable(tc->motor_left);
		cli();
		tc->moution_state = TRAIN_STOPPED;
		sei();
		led_off(tc->led_traction);
	}

	if (tc->moution_state == TRAIN_STOPPED && speed > 0) {
		cli();
		tc->moution_state = TRAIN_IDLE;
		sei();
		led_set_blink(tc->led_traction, 600, 600, 0);
	}

	if (tc->moution_state == TRAIN_IDLE && speed == 0) {
		cli();
		tc->moution_state = TRAIN_STOPPED;
		sei();
		led_off(tc->led_traction);
	}
}

/**
 * Set motors power considering traction power balance
 * @param power - power between 0% to 100%
 */
void traction_set_power(struct train_controller *tc, u8 power)
{
	u8 power_left, power_right;
	s16 balance = tc->balance - STATIC_TRACTION_BALANCE;

	tc->power = power;

	/* calculate traction balance */
	if (balance > 0) {
		power_right = power;
		power_left = power - (balance * power / 100);
	} else {
		power_left = power;
		power_right = power - (abs(balance) * power / 100);
	}

	ac_motor_set_power(tc->motor_left, power_left);
	ac_motor_set_power(tc->motor_right, power_right);
}


static void traction_inc_position_safe(struct train_controller *tc)
{
	if (tc->moution_state <= TRAIN_IDLE) {
		if (tc->reverse)
			ac_motor_enable_bacward(tc->motor_left);
		else
			ac_motor_enable_forward(tc->motor_left);
		tc->moution_state = TRAIN_POSITION_1;
		traction_set_power(tc, position_power_table[1]);
		return;
	}

	tc->moution_state++;
	if (tc->moution_state > TRAIN_POSITION_LAST)
		tc->moution_state = TRAIN_POSITION_LAST;

	traction_set_power(tc, position_power_table[tc->moution_state]);
}


static void traction_reset_position_safe(struct train_controller *tc)
{
	if (tc->moution_state < TRAIN_POSITION_1)
		return;

	tc->moution_state = TRAIN_RESET_POSITION;
	led_set_blink(tc->led_traction, 300, 300, 0);
	traction_set_power(tc, position_power_table[tc->moution_state]);
}


static void traction_dec_position_safe(struct train_controller *tc)
{
	if (tc->moution_state < TRAIN_POSITION_1)
		return;

	tc->moution_state--;
	traction_set_power(tc, position_power_table[tc->moution_state]);
}


static void traction_reverse_enable_safe(struct train_controller *tc)
{
	if (tc->moution_state > TRAIN_STOPPED) {
		led_set_blink(tc->led_reverse, 100, 300, 3);
		return;
	}

	tc->reverse = 1;
	led_on(tc->led_reverse);
}


static void traction_reverse_disable_safe(struct train_controller *tc)
{
	if (tc->moution_state > TRAIN_STOPPED) {
		led_set_blink(tc->led_reverse, 100, 300, 3);
		return;
	}

	tc->reverse = 0;
	led_off(tc->led_reverse);
}

/**
 * Gerkon callback
 * @param arg - struct train_controller
 * @param state - new state
 */
static void ready_state_changed(void *arg, u8 state)
{
	struct train_controller *tc = (struct train_controller *)arg;

	tc->ready = state;
	if (state) {
		led_on(tc->led_ready);
		return;
	}

	led_off(tc->led_ready);
	traction_reset_position_safe(tc);
}

static void button_up_handler(void *arg)
{
	struct train_controller *tc = (struct train_controller *)arg;
	if (!tc->ready) {
		led_set_blink(tc->led_error, 300, 0, 1);
		return;
	}

	traction_inc_position_safe(tc);
}

static void button_reset_handler(void *arg)
{
	struct train_controller *tc = (struct train_controller *)arg;
	if (!tc->ready) {
		led_set_blink(tc->led_error, 300, 0, 1);
		return;
	}

	traction_reset_position_safe(tc);
}

static void button_down_handler(void *arg)
{
	struct train_controller *tc = (struct train_controller *)arg;
	if (!tc->ready) {
		led_set_blink(tc->led_error, 300, 0, 1);
		return;
	}

	traction_dec_position_safe(tc);
}


void button_reverse_handler(void *arg, t_counter hold_counter)
{
	struct train_controller *tc = (struct train_controller *)arg;

	if (!tc->ready) {
		led_set_blink(tc->led_error, 300, 0, 1);
		return;
	}

	if (hold_counter > 1)
		return;

	if (tc->reverse)
		traction_reverse_disable_safe(tc);
	else
		traction_reverse_enable_safe(tc);
}


static void balance_regulator_changed(void *arg, s16 value)
{
	struct train_controller *tc = (struct train_controller *)arg;
	tc->balance = value;
	traction_set_power(tc, tc->power);
}


static struct train_controller tc = {
	.ready = 0,
	.prev_ready = 0,
	.reverse = 0,
	.moution_state = TRAIN_STOPPED,
	.power = 0,
	.balance = 0,

	.motor_left = &motor_left,
	.motor_right = &motor_right,
	.led_ready = &led_ready,
	.led_error = &led_error,
	.led_reverse = &led_reverse,
	.led_traction = &led_traction,
	.timer = {
		.devisor = 2000,
		.handler = train_controller_timer,
		.priv = &tc,
	},
	.wrk = {
		.handler = train_controller_work,
		.priv = &tc,
	},
};


static struct gpio_input ready_gerkon = {
	.gpio = gpio_list + 3,
	.on_change = ready_state_changed,
	.priv = &tc
};

static struct gpio_key traction_up = {
	.input = {
		.gpio = gpio_list + 0
	},
	.on_click = button_up_handler,
	.priv = &tc
};

static struct gpio_key traction_reset = {
	.input = {
		.gpio = gpio_list + 1
	},
	.on_click = button_reset_handler,
	.on_hold = button_reverse_handler,
	.priv = &tc
};

static struct gpio_key traction_down = {
	.input = {
		.gpio = gpio_list + 2
	},
	.on_click = button_down_handler,
	.priv = &tc
};


static struct balance_regulator power_balance_regulator = {
	.on_change = balance_regulator_changed,
	.priv = &tc
};


void train_controller_init(void)
{
	gpio_debouncer_register_input(&ready_gerkon);
	gpio_keys_register_key(&traction_up);
	gpio_keys_register_key(&traction_reset);
	gpio_keys_register_key(&traction_down);
	balance_regulator_init(&power_balance_regulator);
	sys_timer_add_handler(&tc.timer);
	sys_idle_add_handler(&tc.wrk);
}

/*
 * speedometer.c
 *
 *  Created on: 10 июля 2016 г.
 *      Author: Michail Kurochkin
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "list.h"
#include "sys_timer.h"
#include "speedometer.h"
#include "eeprom_fs.h"
#include "gpio.h"

#define SPEED_INDICATOR_MAX 40 /* max speed value Km/h */
#define SPEED_INDICATOR_PWM_MAX 235 /* max PWD output value */


void speedometer_timer(void *arg);
void odometer_timer(void *arg);
void speedometer_work(void *arg);
static struct speedometer sm = {
	.speed_timer = {
		.devisor = 100,
		.handler = speedometer_timer,
	},

	.odometer_timer = {
		.devisor = 1000,
		.handler = odometer_timer,
	},

	.wrk = {
		.handler = speedometer_work,
	},
	.speed = 0,
};

/* input tahogenerator signal */
ISR(INT1_vect)
{
	sm.taho_counter ++;
}

void speedometer_timer(void *arg)
{
	sm.speed = sm.taho_counter;
	sm.taho_counter = 0;
}

void odometer_timer(void *arg)
{
	sm.distance += sm.speed_m;
	sm.distance_tmp ++;
}

void speedometer_work(void *arg)
{
	u8 speed_km, speed_m;
	u16 speed_tmp;
	speed_km = (u8)((u32)sm.speed * 10 / 82);
	speed_m = (u32)sm.speed_km * 1000 / 3600;

	cli();
	sm.speed_km = speed_km;
	sm.speed_m = speed_m;
	speed_tmp = sm.distance_tmp;
	sei();

	/* save odometer state each 1000 meters */
	if (speed_tmp > 1000) {
		odometer_save_state();
		cli();
		sm.distance_tmp = 0;
		sei();
	}
}

/**
 * set speedomenetr indicator value
 * @param speed - speed value beetween
 */
void speedometer_indicator_set(u8 speed)
{
	if (speed > SPEED_INDICATOR_MAX + 3)
		speed = SPEED_INDICATOR_MAX + 3;

	OCR0 = ((u32)speed * SPEED_INDICATOR_PWM_MAX) / SPEED_INDICATOR_MAX;
}

/**
 * Return current speed
 * @return
 */
u8 speedometer_get_speed(void)
{
	return sm.speed_km;
}


/**
 * Return odometer value in meters
 * @return
 */
u32 odometer_get_value(void)
{
	return sm.distance;
}

#include "leds.h"
#include "board.h"
/* Save odometer state into flash */
void odometer_save_state(void)
{
	eeprom_write_file("dist", (u8 *)&sm.distance);
}

void speedometer_init(void)
{
	int rc;

	/* configure external interrupts for Int1 */
	MCUCR |= _BV(ISC10);
	GICR |= _BV(INT1);

//	eeprom_fs_format();
	rc = eeprom_read_file("dist", (u8 *)&sm.distance);
	if (rc < 0) {
		eeprom_create_file("dist", sizeof(sm.distance));
		sm.distance = 0;
	}
	printf("dist = %lu\r\n", sm.distance);

	sm.distance_tmp = 0;

	sys_timer_add_handler(&sm.speed_timer);
	sys_timer_add_handler(&sm.odometer_timer);
	sys_idle_add_handler(&sm.wrk);
}



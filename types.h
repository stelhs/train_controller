/*
 * types.h
 *
 *  Created on: 11.02.2012
 *      Author: Michail Kurochkin
 */

#ifndef TYPES_H_
#define TYPES_H_

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned int u16;
typedef int s16;
typedef unsigned long u32;
typedef long s32;

typedef u8 bool;

typedef u32 t_counter;

#define ON 1
#define OFF 0


// Создать строку в программной памяти
#define SET_PGM_STR(num, str)    \
    prog_char __str_pgm_##num[] = {str} \

// Получить созданную ранее строку
#define GET_PGM_STR(num)   \
    (prog_char *)(__str_pgm_##num)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define EINVAL 1

#endif /* TYPES_H_ */

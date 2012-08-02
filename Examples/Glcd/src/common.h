#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

/**
 * @file common.h
 *
 * Types, defines and functions of global use.
 */

/**
 * The CPU frequency.
 */
#define F_CPU 16000000L

/**
 * Callback type without args, used mainly as interrupt handlers.
 */
typedef void (*intr_handler_t)(void);

/**
 * Callback type with a single uint8_t arg. Used mainly to return
 * values from interrupts.
 */
typedef void (*recv_handler_t)(uint8_t);

/**
 * Bit macro to set a single bit.
 */
#define set_bit(addr, bit) do { addr |= _BV(bit); } while (0);

/**
 * Bit macro to clear a single bit.
 */
#define clr_bit(addr, bit) do { addr &= ~_BV(bit); } while (0);

#ifdef DEBUG
#define debug(...) do { printf_P(__VA_ARGS__); } while(0);
#else
#define debug(...) do {} while(0);
#endif

#endif /* COMMON_H */

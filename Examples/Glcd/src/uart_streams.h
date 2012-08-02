#ifndef UART_STREAMS_H
#define UART_STREAMS_H

/**
 * @file uart_streams.h
 *
 * Provides a simple stderr/stdout stream which prints
 * over UART0.
 */

#include <stdio.h>

/**
 * This macro enables usage of stderr for assert() failures if
 * this header is included BEFORE assert.h. */
#define __ASSERT_USE_STDERR

/**
 * Sets up stderr and stdout to use UART0.
 * Interrupts must be disabled. */
void uart_streams_init(void);

#endif /* UART_STREAMS_H */

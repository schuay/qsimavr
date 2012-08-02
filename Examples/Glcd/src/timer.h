#ifndef TIMER_H
#define TIMER_H

/**
 * @file timer.h
 *
 * Simplifies setting timers.
 */

#include <stdbool.h>
#include <stdint.h>
#include "common.h"

enum TimerN {
    Timer1,     /* 16 bit timers */
    Timer3,
    Timer4,
    Timer5,
    Timer0,     /* 8 bit timers, unused for now */
    Timer2,
    TimerNEnd,
};

/**
 * Setup timer (Timer1, Timer3, ...) for output compare
 * mode with a time of ms milliseconds. The timer can
 * be configured to either run once or periodically.
 * The specified handler is executed on each output compare
 * interrupt.
 */
struct timer_conf {
    enum TimerN timer;
    bool once;
    uint16_t ms;
    intr_handler_t output_cmp_handler;
};

/**
 * Sets the timer as specified and starts it.
 */
bool timer_set(const struct timer_conf *conf);

#endif /* TIMER_H */

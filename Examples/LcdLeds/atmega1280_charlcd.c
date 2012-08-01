/*
    atmega48_charlcd.c

    Copyright Luki <humbell@ethz.ch>
    Copyright 2011 Michel Pollet <buserror@gmail.com>

    This file is part of simavr.

    simavr is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    simavr is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

#undef F_CPU
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uint8_t subsecct = 0;
static uint8_t hour = 0;
static uint8_t minute = 0;
static uint8_t second = 0;
static volatile uint8_t update_needed = 0;

#include "avr_hd44780.h"

ISR( TIMER1_COMPA_vect )
{
    subsecct++;
    if (subsecct == 50) {
        second++;
        subsecct = 0;
        update_needed = 1;
        if (second == 60) {
            minute++;
            second = 0;
            if (minute == 60) {
                minute = 0;
                hour++;
                if (hour == 24)
                    hour = 0;
            }
        }
    }
}

static void timer_init(void)
{
    TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);
    OCR1A = 312; /* 20 ms / (62.5 ns * 1024 prescaler) */
    TIMSK1 |= _BV(OCIE1A);
}

/**
 * Shifts the display such that the clock moves back and
 * forth between the left and right edge of the screen.
 * When moving left, the cursor is shifted one spot to the
 * right, when moving right it is shifted to ther left.
 */
static void shift(void)
{
    static int8_t shift = 4;
    static uint8_t right = 1;

    /* Shift display left and right. */
    if (shift == 7) {
        right = 0;
    } else if (shift == 0) {
        right = 1;
    }
    shift += (right ? 1 : -1);

    hd44780_outcmd(HD44780_SHIFT(1, right));
    hd44780_wait_ready(1);

    hd44780_outcmd(HD44780_SHIFT(0, !right));
    hd44780_wait_ready(1);
}

#define WRITE_AT(data, address) { \
        hd44780_outcmd(HD44780_DDADDR(address)); \
        hd44780_wait_ready(1); \
        hd44780_outdata(data); \
        hd44780_wait_ready(0); }

int main()
{
    timer_init();
    hd44780_init();
    /*
     * Clear the display.
     */
    hd44780_outcmd(HD44780_CLR);
    hd44780_wait_ready(1); // long wait

    /*
     * Entry mode: auto-increment address counter, no display shift in
     * effect.
     */
    hd44780_outcmd(HD44780_ENTMODE(1, 0));
    hd44780_wait_ready(0);

    /*
     * Enable display, activate non-blinking cursor.
     */
    hd44780_outcmd(HD44780_DISPCTL(1, 1, 0));
    hd44780_wait_ready(0);

    /*
     * The WH1602 seems to need a longer wait after the clear command.
     */
    hd44780_outcmd(HD44780_CLR);
    hd44780_wait_ready(1);
    hd44780_wait_ready(1);

    EICRA = (1 << ISC00);
    EIMSK = (1 << INT0);

    sei();

    while (1) {
        while (!update_needed)
            sleep_mode();
        update_needed = 0;
        char buffer[16];
        sprintf(buffer, "%2d:%02d:%02d", hour, minute, second);

        /* Mark beginning and end of vram in both lines
         * so we can make sure the shift wraps correctly. */
        WRITE_AT('A', 0x0);
        WRITE_AT('O', 0x27);
        WRITE_AT('a', 0x40);
        WRITE_AT('o', 0x67);

        hd44780_outcmd(HD44780_DDADDR(0x4));
        hd44780_wait_ready(1);

        char *s = buffer;
        while (*s) {
            hd44780_outdata(*s++);
            hd44780_wait_ready(0);
        }

        shift();
    }

}

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "common.h"
#include <util/delay.h>

#include "lcd.h"
#include "timer.h"

#include "uart_streams.h"
#include <assert.h>

#define EXT __extension__

/* The LCD is driven by PORTJ.
 * R/W is connected to GND (meaning 'write').
 * D[0-3] are connected to GND.
 *
 * Character DDRAM addresses:
 * 00 01 02 ... 0F
 * 40 41 42 ... 4F
 *
 * Pin mappings are as follows: */
enum LCDPins {
    RS = PC2,   /* Register Selector. 0: instr write, 1: data write */
    E = PC3,    /* Chip Enable. */
    DB4 = PC4,  /* Data Bus 4. */
    DB5 = PC5,
    DB6 = PC6,
    DB7 = PC7,
};

enum LCDInstructions {
    ClearDisplay = 1 << 0,
    ReturnHome = 1 << 1,
    EntryModeSet = 1 << 2,
    DisplayControl = 1 << 3,
    CursorDisplayShift = 1 << 4,
    FunctionSet = 1 << 5,
    SetCGRAMAddr = 1 << 6,
    SetDDRAMAddr = 1 << 7,
};

/** The width of the LCD in characters. */
#define COLS (16)

/** The height of the LCD in characters. */
#define ROWS (2)

static void send_ctl(uint8_t cmd);
static void send_data(uint8_t data);
static void send_byte(uint8_t packet);
static void send_nibble(uint8_t nibble);

void lcd_putstr_P(const char *s, uint8_t row, uint8_t col) {
    assert(s != NULL);
    for (uint8_t i = 0; ; i++) {
        char c = pgm_read_byte(s + i);
        if (c == '\0') {
            return;
        }
        if (col + i >= COLS) {
            return;
        }
        lcd_putchar(c, row, col + i);
    }
}

void lcd_putchar(char c, uint8_t row, uint8_t col) {
    assert(row < ROWS);
    assert(col < COLS);

    send_ctl(SetDDRAMAddr | (row << 6) | col);
    send_data(c);
}

void lcd_clear(void) {
    for (uint8_t y = 0; y < ROWS; y++) {
        for (uint8_t x = 0; x < COLS; x++) {
            lcd_putchar(' ', y, x);
        }
    }
}

static volatile uint8_t lcd_init_wait = 1;
static void lcd_init_cont(void) {
    lcd_init_wait = 0;
}

void lcd_init(void) {
    const uint8_t msk = _BV(PC1) | _BV(PC0);

    /* Set PC[2-7] to output and zero it. */
    PORTC &= msk;
    DDRC |= ~msk;

    /* Datasheet: 40ms, Tutor: 50ms.
     * This feels very much like busy waiting - on the other hand,
     * this is only executed once during initialization.
     * It would be much nicer to just do this using _delay_ms(),
     * but there is a strict restriction on pure busy waiting.
     * We can't change this to pure timer driven async, because
     * then the first strings could be written before init is done.
     * That could also be handled using buffered lcd writes,
     * but the module was not conceived that way, and thus we need
     * to live with this forced 50 ms wait.
     */
    struct timer_conf conf = { Timer4, true, 50, lcd_init_cont };
    timer_set(&conf);

    while (lcd_init_wait) {
        sleep_cpu();
    }

    /* 8 bit data length.
     * Needs to be sent 3 times to account for all possible states
     * of the LCD. */
    send_nibble(FunctionSet | EXT 0b00010000);
    _delay_us(39);

    send_nibble(FunctionSet | EXT 0b00010000);
    _delay_us(39);

    send_nibble(FunctionSet | EXT 0b00010000);
    _delay_us(39);

    /* Make sure the next send is received correctly in 4 bit mode. */
    send_nibble(FunctionSet);
    _delay_us(39);

    /* 4 bit data length, 2 display lines, 5x11 font type. */
    send_ctl(FunctionSet | EXT 0b00001100);

    _delay_us(39);

    send_ctl(FunctionSet | EXT 0b00001100);

    _delay_us(37);

    /* Enable display. */
    send_ctl(DisplayControl | EXT 0b00000100);

    _delay_us(37);

    lcd_clear();

    /* Entry mode set, shift cursor/DDRAM right.  */
    send_ctl(EntryModeSet | EXT 0b00000010);
}

/**
 * Sends data to the LCD's display RAM.
 */
static void send_data(uint8_t data) {
    set_bit(PORTC, RS);
    send_byte(data);
}

/**
 * Sends cmd to the LCD.
 */
static void send_ctl(uint8_t cmd) {
    clr_bit(PORTC, RS);
    send_byte(cmd);
}

/**
 * Sends upper nibble.
 */
static void send_nibble(uint8_t nibble) {
    const uint8_t msk = 0x0F;

    /* It looks like the only time constraint we need to
     * worry about is the enable cycle time of 1200 ns.
     * However, the disassembly shows 21 cycles between the start
     * of send_nibble to the next call of send_nibble, which
     * takes approximately 1312.5 ns so we should be good. */

    set_bit(PORTC, E);
    PORTC = (PORTC & msk) | (nibble & ~msk);

    /* Short delay (again, this isn't documented but seems to be necessary). */
    _delay_us(1);

    clr_bit(PORTC, E);

    /* Contrary to the datasheet (?) and the comment above,
     * the lcd doesn't work without this delay. I'm not sure
     * why, especially because its over 50 times as much as
     * specified. */
    _delay_us(50);
}

/**
 * Sends first the lower, then the upper nibble.
 */
static void send_byte(uint8_t packet) {
    send_nibble(packet);
    send_nibble(packet << 4);
}

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdbool.h>

#include "glcd_hal.h"
#include "common.h"
#include <util/delay.h>
#include "uart_streams.h"
#include <assert.h>

/** Converts pixel coordinates to the corresponding page. */
#define PAGE(x, y) (y / PX_PER_LINE)

/**
 * Converts pixel coordinates to the corresponding chip.
 * Note the reverse logic here: (0,0) returns CS1.
 * This is because the chip is ORed bitwise into the command packet,
 * and chip selects are LOW active.
 */
#define CHIP(x, y) (CS1 - x / PX_PER_CHIP)

/** Converts pixel coordinates to the position within a block. */
#define ADDR(x, y) (x % PX_PER_CHIP)

/** Utility macro since __extension__ is looong. */
#define EXT __extension__

/**
 * Zeroes represent the bits we can touch.
 * NOTE: RST is also masked. It's low active and should never be touched. */
#define PORTE_MSK (EXT 0b10000011)

/** The last read / write position. */
static struct {
    uint8_t x, y;
} glb;

/* The GLCD is driven by PORTA and PORTE (PE2-7).
 *
 * Pin mappings are as follows: */
enum GLCDPinsE {
    CS0 = PE2,  /* Select Segment 1 - 64. LOW active. */
    CS1 = PE3,  /* Select Segment 65 - 128. LOW active. */
    RS = PE4,   /* Register Select. 0: instr, 1: data */
    RW = PE5,   /* Read/Write: 0: write, 1: read */
    E = PE6,    /* Chip Enable. */
    RST = PE7,  /* Reset. LOW active. */
};

enum GLCDPinsA {
    D0 = PA0,   /* Data Bus. */
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
};

enum StatusFlags {
    Reset = PA4,
    OnOff = PA5,
    Busy = PA7,
};

enum GLCDInstructions {
    DisplayOnOff = EXT 0b00111110,
    SetAddress = EXT 0b01000000,
    SetPage = EXT 0b10111000,
    DisplayStartLine = EXT 0b11000000,
};


static void _glcd_send_ctl(uint8_t chip, uint8_t cmd);
static void _glcd_send(uint8_t ctl, uint8_t data);
static void _glcd_send_data(uint8_t chip, uint8_t data);
static uint8_t _glcd_recv(uint8_t ctl);
static uint8_t _glcd_recv_status(uint8_t chip);
static uint8_t _glcd_recv_data(uint8_t chip);
static void _glcd_busy_wait(uint8_t chip);
static void _glcd_inc_addr(void);
static void _glcd_set_addr(void);

uint8_t halGlcdInit(void) {
    /* Setup PORTE. Note the special handling of RST.
     * For convenience, PORTE_MSK pretends that RST doesn't
     * belong to the GLCD (we should never touch it).
     * However, we need to set it up as output and pull it high. */
    PORTE = (PORTE & PORTE_MSK) | _BV(RST);
    DDRE |= ~PORTE_MSK | _BV(RST);

    _glcd_send_ctl(CS0, DisplayOnOff | 0x01);
    _glcd_send_ctl(CS1, DisplayOnOff | 0x01);
    _glcd_send_ctl(CS0, DisplayStartLine | 0x00);
    _glcd_send_ctl(CS1, DisplayStartLine | 0x00);

    /* Reset our address store. */
    memset(&glb, 0, sizeof(glb));

    return 0;
}

uint8_t halGlcdSetAddress(const uint8_t xCol, const uint8_t yPage) {
    if (xCol >= WIDTH || yPage >= HEIGHT) {
        return 1;
    }

    glb.x = xCol;
    glb.y = yPage;

    _glcd_set_addr();

    return 0;
}

static void _glcd_set_addr(void) {
    _glcd_send_ctl(CHIP(glb.x, glb.y), SetPage | PAGE(glb.x, glb.y));
    _glcd_send_ctl(CHIP(glb.x, glb.y), SetAddress | ADDR(glb.x, glb.y));
}

static void _glcd_inc_addr(void) {
    bool reset_addr = false;

    glb.x++;
    if (glb.x == PX_PER_CHIP) {
        reset_addr = true;
    } else if (glb.x == WIDTH) {
        glb.x = 0;
        glb.y = (glb.y + PX_PER_LINE) % HEIGHT;
        reset_addr = true;
    }

    if (reset_addr) {
        _glcd_set_addr();
    }
}

uint8_t halGlcdWriteData(const uint8_t data) {
    _glcd_send_data(CHIP(glb.x, glb.y), data);
    _glcd_inc_addr();
    return 0;
}

/**
 * For some reason, reading display RAM needs to be done twice to return
 * the actual contents of the current address.
 */
uint8_t halGlcdReadData(void) {
    (void)_glcd_recv_data(CHIP(glb.x, glb.y));
    _glcd_set_addr();
    uint8_t data = _glcd_recv_data(CHIP(glb.x, glb.y));
    _glcd_inc_addr();
    return data;
}

/**
 * Writes data into the GLCD display RAM.
 * chip is either CS0 or CS1.
 */
void _glcd_send_data(uint8_t chip, uint8_t data) {
    _glcd_busy_wait(chip);
    _glcd_send(_BV(chip) | _BV(RS), data);
}

/**
 * Sends an instruction to the GLCD.
 * @param chip is either CS0 or CS1.
 * @param cmd is the command to send and will be written to PORTA.
 */
static void _glcd_send_ctl(uint8_t chip, uint8_t cmd) {
    _glcd_busy_wait(chip);
    _glcd_send(_BV(chip), cmd);
}

/**
 * The exact timing is achieved by disassembling the optimized object
 * file and inserting NOPs as needed:
 *
 * \code
    00000000 <_glcd_send>:
       0:   76 98           cbi     0x0e, 6 ; 14
       2:   9f ef           ldi     r25, 0xFF       ; 255
       4:   91 b9           out     0x01, r25       ; 1
       6:   2e b1           in      r18, 0x0e       ; 14
       8:   23 78           andi    r18, 0x83       ; 131
       a:   20 68           ori     r18, 0x80       ; 128
       c:   8c 77           andi    r24, 0x7C       ; 124
       e:   28 2b           or      r18, r24
      10:   2e b9           out     0x0e, r18       ; 14
      12:   62 b9           out     0x02, r22       ; 2
      14:   76 9a           sbi     0x0e, 6 ; 14
      16:   76 98           cbi     0x0e, 6 ; 14
      18:   08 95           ret
 * \endcode
 *
 * cpi, sbi: 2 cycles
 * in, out, andi, ori, or: 1 cycle
 * At 16 MHz, one cycle takes approximately 62.5 ns.
 */
static void _glcd_send(uint8_t ctl, uint8_t data) {
    /* Pull E low. 420 ns */
    clr_bit(PORTE, E);

    /* Set PORTA to output. */
    DDRA = 0xff;

    /* Set data. 140 ns */
    PORTE = (PORTE & PORTE_MSK) | (ctl & ~PORTE_MSK);
    PORTA = data;

    /* Pull E high. 420 ns */
    set_bit(PORTE, E);

    _NOP(); _NOP(); _NOP();
    _NOP(); _NOP(); _NOP();

    /* Pull E low. */
    clr_bit(PORTE, E);
}

static void _glcd_busy_wait(uint8_t chip) {
    uint8_t status;
    for (uint8_t i = 0; i < 255; i++) {
       status = _glcd_recv_status(chip);
       if (!(status & (_BV(Reset) | _BV(Busy)))) {
           return;
       }
       _delay_us(2);
    }
    debug(PSTR("GLCD busy wait timed out, last status: %d.\n"), status);
}

/**
 * Reads status from selected chip.
 */
static uint8_t _glcd_recv_status(uint8_t chip) {
    return _glcd_recv(_BV(chip) | _BV(RW));
}

/**
 * Reads display RAM contents at current address
 * from selected chip.
 */
static uint8_t _glcd_recv_data(uint8_t chip) {
    _glcd_busy_wait(chip);
    return _glcd_recv(_BV(chip) | _BV(RW) | _BV(RS));
}

/**
 * Sends the ctl instruction to the GLCD and
 * returns read data from PORTA.
 */
static uint8_t _glcd_recv(uint8_t ctl) {
    /* Pull E low. 420 ns */
    clr_bit(PORTE, E);

    /* Set PORTA to input. */
    DDRA = 0x00;

    _NOP(); _NOP();

    /* Set data. 140 ns */
    PORTE = (PORTE & PORTE_MSK) | (ctl & ~PORTE_MSK);

    _NOP(); _NOP();

    /* Pull E high. 320 ns */
    set_bit(PORTE, E);

    _NOP(); _NOP(); _NOP();
    _NOP(); _NOP();

    /* Read data. */
    uint8_t data = PINA;

    /* Pull E low. */
    clr_bit(PORTE, E);

    return data;
}

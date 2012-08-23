/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "char_display.h"
#include "temperature_control.h"

#ifdef OLD_AVR_GCC
#include <avr/delay.h>
#else
#include <util/delay.h>
#endif

#include "util.h"
#include <string.h>

#define	LCD_PORT	*port
#define	LCD_DIR		*ddr
#define	LCD_PIN		*pin

static volatile uint8_t LCD_PORT;
static volatile uint8_t LCD_DIR;
static volatile uint8_t LCD_PIN;

// internal prototypes
static void write(uint8_t byte, bool rs);
static inline void writeNibble(uint8_t byte, bool rs);
static inline void pollBusy();
static uint8_t read(bool rs);

// internal definitions and macros
#define CTL_MASK	((1<<RS) | (1<<RW) | (1<<EN))
#define PORT_MASK	(CTL_MASK | DATA_MASK)
#define nop()		asm volatile("nop;")
#define BUSY		0x80
#define RAM_LINE_WIDTH	0x40

static uint8_t buffer[LINES][LINE_WIDTH];
static bool dirty[LINES];
static uint8_t line;
static uint8_t position;

void lcdInit(volatile uint8_t *_port) {
	port = _port;
	ddr = _port-1;
	pin = _port-2;

	// init ports
	LCD_DIR = PORT_MASK;
	LCD_PORT = 0;

	lcdClear();

	_delay_ms(40);		// busy flag not available
	// TODO: test this after a long power off
	//writeNibble(0x03, false);

	_delay_us(40);		// busy flag not available
	write(0x28, false);
	_delay_us(40);		// busy flag not available
	write(0x28, false);
	pollBusy();
	write(0x0C, false);
	pollBusy();
	write(0x01, false);
	pollBusy();
	write(0x06, false);
}

void lcdRefresh() {
	uint8_t addr;
	uint8_t line;
	uint8_t col;

	for(line=0; line<LINES; line++) {
		if (dirty[line] == true) {
			// goto first character
			addr = (1<<7) | (line * RAM_LINE_WIDTH);
			pollBusy();
			write(addr, false);
			// send line
			for(col=0; col<LINE_WIDTH; col++) {
				pollBusy();
				write(buffer[line][col], true);
			}
			dirty[line] = false;
		}
	}
}

void lcdClear() {
	memset(buffer, ' ', sizeof(buffer));
	memset(dirty, true, sizeof(dirty));		// does not work?
	// workaround:
	dirty[0] = true;
	dirty[1] = true;
	lcdGoto(0, 0);
}

void lcdGoto(uint8_t _line, uint8_t _position) {
	line= _line;
	position = _position;
}

void lcdWrite(char *string) {
	while(*string != 0 && line < LINES) {
		if (position < LINE_WIDTH) {
			buffer[line][position] = *string;
			string++;
			position++;
			dirty[line] = true;
		}
		else {
			position = 0;
			line++;
		}
	}
}

void write(uint8_t byte, bool rs) {
	LCD_PORT = 0;
	LCD_DIR = PORT_MASK;

	writeNibble(byte >> 4, rs);
	//_delay_us(2);					// according to datasheet ?
	writeNibble(byte >> 0, rs);
}

void writeNibble(uint8_t byte, bool rs) {
	// data output direction must be already set

	LCD_PORT = (rs<<RS);				// set mode
	LCD_PORT |= (1<<EN);
	LCD_PORT |= (byte & DATA_MASK);		// set data
	LCD_PORT &= ~(1<<EN);				// write data
}

uint8_t read(bool rs) {
	uint8_t byte;

	LCD_DIR = CTL_MASK;

	LCD_PORT = (rs<<RS) | (1<<RW);		// set mode
	LCD_PORT |= (1<<EN);
	nop();								// wait for data
	nop();
	byte = (LCD_PIN & DATA_MASK) << 4;	// read data
	LCD_PORT &= ~(1<<EN);
	//_delay_us(2);						// according to datasheet ?
	LCD_PORT |= (1<<EN);
	nop();								// wait for data
	nop();
	byte |= (LCD_PIN & DATA_MASK);
	LCD_PORT &= ~(1<<EN);
	return byte;
}

void pollBusy() {
	while ((read(false) & BUSY) != 0);
}

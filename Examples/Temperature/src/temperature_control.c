/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

/*
 * Pin Description
 *
 * LCD_DATA		PA0-3
 * RS			PA6
 * RW			PA5
 * EN			PA4
 * 1-wire		PD6
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "temperature_control.h"
#include "char_display.h"
#include "uart.h"
#include "util.h"
#include "temp_sensor.h"
#include <avr/sleep.h>

static bool refresh = false;
static int16_t temp;
static uint8_t data;

TEMP_READ_CALLBACK {
	temp = temperature;
	refresh = true;
	tempStartRead();
}

UART_CALLBACK {
	data = value;
	refresh = true;
}

int main() {
	int16_t temperature;
	int16_t frac;
	uint8_t count = 0;
	char degStr[] = {0xb2, 'C', 0};

	uartInit();
	uartSendChar(10);

	lcdInit(&PORTB);
	tempInit();

	sei();

	tempStartRead();

	set_sleep_mode(SLEEP_MODE_IDLE);

	while (1) {
		// background task
		while (refresh == true) {
			cli();
			temperature = temp;
			refresh = false;
			sei();

			// refresh display
			lcdClear();
			lcdGoto(0, 0);
			lcdWrite("Temp: ");
			if (temp != TEMP_ERROR) {
				lcdWrite(int2string(temperature>>4));
				lcdWrite(".");
				frac = (temperature&0xf)*10000/16;
				if (frac < 1000) {
					lcdWrite("0");
				}
				lcdWrite(int2string(frac));
				lcdWrite(degStr);
			}
			else {
				lcdWrite("no sensor");
			}
			lcdGoto(1, 0);
			lcdWrite(int2string(count));
			count++;

			lcdGoto(1, 6);
			lcdWrite("data: 0x");
			lcdWrite(hex2string(data));
			lcdRefresh();
		}
		sleep_mode();
	}
}

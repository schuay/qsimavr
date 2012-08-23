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
#include <stdio.h>
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

	printf("Hello\n");

#ifndef NO_LCD
	lcdInit(&PORTB);
#endif
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
#ifdef NO_LCD
#else
			lcdClear();
			lcdGoto(0, 0);
			lcdWrite("Temp: ");
#endif
			if (temp != TEMP_ERROR) {
				frac = (temperature&0xf)*10000/16;
#ifdef NO_LCD
				printf("Temp: %d.%s%d C\n", temperature >> 4, frac < 1000 ? "0" : "", frac);
#else
				lcdWrite(int2string(temperature>>4));
				lcdWrite(".");
				if (frac < 1000) {
					lcdWrite("0");
				}
				lcdWrite(int2string(frac));
				lcdWrite(degStr);
#endif
			}
			else {
#ifdef NO_LCD
				printf("no sensor\n");
#else
				lcdWrite("no sensor");
#endif
			}

#ifdef NO_LCD
			printf("%d data: 0x%x\n", count, data);
#else
			lcdGoto(1, 0);
			lcdWrite(int2string(count));

			lcdGoto(1, 6);
			lcdWrite("data: 0x");
			lcdWrite(hex2string(data));
			lcdRefresh();
#endif

			count++;
		}
		sleep_mode();
	}
}

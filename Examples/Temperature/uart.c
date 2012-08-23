/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "uart.h"
#include "temperature_control.h"
#include <avr/interrupt.h>
#include <stdio.h>

#ifdef OLD_AVR_GCC
#include <avr/signal.h>
#endif

#define BAUD_RATE_REG	(F_CPU / (BAUD_RATE * 16))

static int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,
					  _FDEV_SETUP_WRITE);

static int
uart_putchar(char c, FILE *stream)
{

  if (c == '\n')
    uart_putchar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

void uartInit() {
	UCSR0B = _BV(TXEN0);
	UBRR0 = 8; /* 115200 */

	stdout = &mystdout;
}

void uartSendChar(uint8_t c) {
	while ((UCSR0A & (1<<UDRE0)) == 0);
	UDR0 = c;
}

void uartSendString(char *string) {
	while (*string != 0) {
		uartSendChar(*string);
		string++;
	}
}

void uartNL() {
	uartSendChar('\n');
	uartSendChar('\a');
}
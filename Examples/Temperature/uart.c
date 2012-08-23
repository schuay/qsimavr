/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "uart.h"
#include "temperature_control.h"
#include <avr/interrupt.h>

#ifdef OLD_AVR_GCC
#include <avr/signal.h>
#endif

#define BAUD_RATE_REG	(F_CPU / (BAUD_RATE * 16))

void uartInit() {
	UBRRL = (BAUD_RATE_REG >> 0) & 0xff;
	UBRRH = (BAUD_RATE_REG >> 8) & 0xff;

	UCSRA = 0;
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);	// no parity, 8 data bits
	UCSRB = (1<<TXEN) | (1<<RXEN) | (1<<RXCIE);
}

void uartSendChar(uint8_t c) {
	while ((UCSRA & (1<<UDRE)) == 0);
	UDR = c;
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

#ifdef OLD_AVR_GCC
SIGNAL(SIG_USART_RECV) {
#else
ISR(USART_RXC_vect) {
#endif
	uart_callback(UDR);
}

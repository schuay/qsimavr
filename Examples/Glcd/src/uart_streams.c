#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "uart_streams.h"
#include "uart.h"

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar,
        NULL, _FDEV_SETUP_WRITE);

void uart_streams_init(void) {
#define BAUD 115200
#define BAUD_TOL 3
#include <util/setbaud.h>
    struct uart_conf conf = {
        Uart0,
        TransmitterEnable,
        UBRR_VALUE,
#if USE_2X
        true,
#else
        false,
#endif
        0,
        0 };
    uart_init(&conf);

    stderr = &uart_stream;
    stdout = &uart_stream;
}

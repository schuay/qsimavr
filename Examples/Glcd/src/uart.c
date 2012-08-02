#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <assert.h>
#include <stdlib.h>

#include "timer.h"
#include "util.h"
#include "uart.h"
#include "common.h"

static volatile uint8_t * const UCSRnA[] = { &UCSR0A, &UCSR1A, &UCSR2A, &UCSR3A };
static volatile uint8_t * const UCSRnB[] = { &UCSR0B, &UCSR1B, &UCSR2B, &UCSR3B };
static volatile uint16_t * const UBRRn[] = { &UBRR0, &UBRR1, &UBRR2, &UBRR3 };
static volatile uint8_t * const UDRn[] = { &UDR0, &UDR1, &UDR2, &UDR3 };

static intr_handler_t udri_handler[UartNEnd];
static recv_handler_t rxci_handler[UartNEnd];

#define BAUD 115200 /* 1 MBit */
#define BAUD_TOL 5

void uart_init(const struct uart_conf *conf) {
    assert(conf != 0);
    assert(conf->uart < UartNEnd);

    enum UartN n = conf->uart;

    udri_handler[n] = conf->data_reg_empty_handler;
    rxci_handler[n] = conf->rx_complete_handler;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        *UCSRnA[n] = (conf->double_speed ? _BV(U2X0) : 0);
        *UCSRnB[n] = conf->ucsrnb;
        *UBRRn[n] = conf->ubrrn;
    }
}

ISR(USART0_RX_vect, ISR_BLOCK) {
    rxci_handler[Uart0](*UDRn[Uart0]);
}

ISR(USART1_RX_vect, ISR_BLOCK) {
    rxci_handler[Uart1](*UDRn[Uart1]);
}

ISR(USART2_RX_vect, ISR_BLOCK) {
    rxci_handler[Uart2](*UDRn[Uart2]);
}

/* Used by bt_hal
ISR(USART3_RX_vect, ISR_BLOCK) {
    rxci_handler[Uart3](*UDRn[Uart3]);
}
*/

ISR(USART0_UDRE_vect, ISR_BLOCK) {
    udri_handler[Uart0]();
}

ISR(USART1_UDRE_vect, ISR_BLOCK) {
    udri_handler[Uart1]();
}

ISR(USART2_UDRE_vect, ISR_BLOCK) {
    udri_handler[Uart2]();
}

ISR(USART3_UDRE_vect, ISR_BLOCK) {
    udri_handler[Uart3]();
}

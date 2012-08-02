#ifndef UART_H
#define UART_H

/**
 * @file uart.h
 *
 * Handles setting up UART modules.
 */

#include <stdbool.h>

#include "common.h"

enum ucsrnbflags {
    ReceiverEnable = _BV(RXEN0),
    TransmitterEnable = _BV(TXEN0),
    DataRegEmptyIntrEnable = _BV(UDRIE0),
    TXCompleteIntrEnable = _BV(TXCIE0),
    RXCompleteIntrEnable = _BV(RXCIE0),
};

enum UartN {
    Uart0,
    Uart1,
    Uart2,
    Uart3,
    UartNEnd,
};

/**
 * Options for setting up uart.
 * The following example sets up uart0 for 115200 baud:
 * \code
    #define BAUD 115200
    #include <util/setbaud.h>
        struct uart_conf conf = {
            Uart0,
            UBRR_VALUE,
            TransmitterEnable,
    #if USE_2X
            true,
    #else
            false,
    #endif
            0,
            0 };
    \endcode
 */
struct uart_conf {
    enum UartN uart;
    uint8_t ucsrnb;
    uint16_t ubrrn;
    bool double_speed;
    intr_handler_t data_reg_empty_handler;
    recv_handler_t rx_complete_handler;
};

/**
 * Sets up a UART module as specified by conf.
 */
void uart_init(const struct uart_conf *conf);

#endif /* UART_H */

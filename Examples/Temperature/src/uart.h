#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#define BAUD_RATE	115200UL

void uartInit(void);
void uartSendChar(uint8_t c);
void uartSendString(char *string);
void uartNL(void);

#define UART_CALLBACK	void uart_callback(uint8_t value)

extern UART_CALLBACK;

#endif /* UART_H_ */

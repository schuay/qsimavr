#ifndef ONE_WIRE_H_
#define ONE_WIRE_H_

#include <avr/io.h>
#include "util.h"
#include "temperature_control.h"

void owInit(void);
void owReset(void);
void owWrite(bool value);
void owRead(void);

typedef enum {
	RESET_SUCCCESS,
	RESET_ERROR,
	READ_0,
	READ_1,
	WRITE_SUCCESS,
} return_state_t;

#define OW_CALLBACK	void owCallback(return_state_t owState)
#define BUS_PORT	PORTG
#define BUS_PIN		PING
#define BUS_DDR		DDRG
#define BUS_BIT		PG0

// time values are given in 500ns steps
#define RESET_PULSE_DURATION	(480 * 2)
#define RESET_WAIT_DURATION		(480 * 2)
#define RESET_PRESENT_PULSE_MIN	((15 + 060) * 2)
#define RESET_PRESENT_PULSE_MAX	((60 + 240) * 2)
#define WRITE_1_DURATION		(10 * 2)
#define WRITE_0_DURATION		(60 * 2)
#define TIMESLOT_DURATION		(61 * 2)
#define START_READ_DURATION		(4 * 2)
#define READ_1_MAX				(15 *2)

extern OW_CALLBACK;

#endif /* ONE_WIRE_H_ */

/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "one_wire.h"
#include <avr/interrupt.h>

#ifdef OLD_AVR_GCC
#include <avr/signal.h>
#endif

typedef enum {
	IDLE,
	RESET,
	READ,
	WRITE,
} state_t;

// internal functins
static void inline startTimer(void);
static void inline stopTimer(void);
static inline void busLow(void);
static inline void busPullUp(void);

static state_t state = IDLE;

void owInit() {
	// init ports
	busPullUp();

	// timer 1, ctc with OCR1A as top, inputcapture on rising edge
	TCCR1A = 0;
	TCCR1B = (1<<WGM12) | (1<<ICES1);

	// enable interrupt on compareA (TOP) and on compareB
	TIMSK |= (1<<OCIE1A) | (1<<OCIE1B);
	state = IDLE;
}

void owReset() {
	if (state != IDLE) return;

	state = RESET;

	// set timing
	OCR1B = RESET_PULSE_DURATION;
	OCR1A = RESET_PULSE_DURATION + RESET_WAIT_DURATION;
	busLow();
	startTimer();
}

void owWrite(bool value) {
	if (state != IDLE) return;

	state = WRITE;

	// set timing
	if (value == true)
		OCR1B = WRITE_1_DURATION;
	else
		OCR1B = WRITE_0_DURATION;
	OCR1A = TIMESLOT_DURATION;
	busLow();
	startTimer();
}

void owRead() {
	if (state != IDLE) return;

	state = READ;

	// set timing
	OCR1B = START_READ_DURATION;
	OCR1A = TIMESLOT_DURATION;
	busLow();
	startTimer();
}

void startTimer() {
	TCNT1 = 0;
	// enable prescaler 8
	TCCR1B |= (1<<CS11);
}

void stopTimer() {
	// disable prescaler 8
	TCCR1B &= ~(1<<CS11);
	state = IDLE;
	TIFR |= (1<<OCF1A) | (1<<OCF1B);
}

void busLow() {
	BUS_DDR |= (1<<BUS_BIT);	// output
	BUS_PORT &= ~(1<<BUS_BIT);	// low
}

void busPullUp() {
	BUS_DDR &= ~(1<<BUS_BIT);	// input
	BUS_PORT &= ~(1<<BUS_BIT);	// no internal pullup
}

#ifdef OLD_AVR_GCC
SIGNAL(SIG_OUTPUT_COMPARE1A) {
#else
ISR(TIMER1_COMPA_vect) {
#endif
	return_state_t retState = RESET_ERROR;

	switch (state) {
	case RESET:
		if (ICR1 < (RESET_PULSE_DURATION + RESET_PRESENT_PULSE_MAX) &&
				ICR1 > (RESET_PULSE_DURATION + RESET_PRESENT_PULSE_MIN)) {	// slave answered ?
			retState = RESET_SUCCCESS;
		}
		else {
			retState = RESET_ERROR;
		}
		break;
	case WRITE:
		retState = WRITE_SUCCESS;
		break;
	case READ:
		if (ICR1 < READ_1_MAX) {			// read 1 ?
			retState = READ_1;
		}
		else {
			retState = READ_0;
		}
		break;
	case IDLE:
		// avoid compiler warning
		break;
	}
	stopTimer();
	owCallback(retState);
}

#ifdef OLD_AVR_GCC
SIGNAL(SIG_OUTPUT_COMPARE1B) {
#else
ISR(TIMER1_COMPB_vect) {
#endif
	busPullUp();	// release bus
}

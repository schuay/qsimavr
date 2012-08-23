/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "temp_sensor.h"
#include "one_wire.h"

typedef enum {
	IDLE,
	CONVERT,
	WAIT,
	ISSUE_READ,
	READ_SCRATCHPAD,
} state_t;

typedef enum {
	RESET,
	SKIP_ROM,
	ACTION,
} substate_t;

typedef enum {
	COM_SKIP_ROM		= 0xcc,
	COM_CONVERT			= 0x44,
	COM_READ_SCRATCHPAD	= 0xbe,
} commands_t;

#define BYTE_SIZE		8
#define SCRATCHPAD_SIZE	9

#define TEMP_LSB		0
#define TEMP_MSB		1
#define COUNT_REMAIN	6
#define COUNT_PER_C		7

static void sendByte(uint8_t byte);
static bool sendingByte(void);
static void readingScratchpad(bool value);
static void processTemperature(void);

static state_t state = IDLE;
static substate_t substate = RESET;
static int8_t value;
static uint8_t pos;
static uint8_t scratchpadPos;
static uint8_t scratchpad[SCRATCHPAD_SIZE];
static uint8_t nextCommand;

void tempInit() {
	owInit();
	state = IDLE;
	substate = RESET;
}

void tempStartRead() {
	if (state != IDLE) return;

	state = CONVERT;
	//state = ISSUE_READ;
	nextCommand = COM_CONVERT;
	//nextCommand = COM_READ_SCRATCHPAD;
	substate = RESET;
	owReset();
}

void sendByte(uint8_t byte) {
	pos = 1;
	value = byte;
	owWrite(value & 0x01);
}

bool sendingByte() {
	if (pos < BYTE_SIZE) {
		value >>= 1;
		pos++;
		owWrite(value & 0x01);
		return true;
	}
	else {
		return false;
	}
}

void readingScratchpad(bool val) {
	scratchpad[scratchpadPos] >>= 1;
	scratchpad[scratchpadPos] |= (val<<7);
	pos++;
	if (pos == BYTE_SIZE) {
		pos = 0;
		scratchpadPos++;
		if (scratchpadPos == SCRATCHPAD_SIZE) {
			processTemperature();
			return;
		}
	}
	owRead();
}

void processTemperature() {
	volatile int16_t temperature;
	state = IDLE;
	//uint8_t i;

	temperature = (int16_t)scratchpad[TEMP_LSB] | (scratchpad[TEMP_MSB] << BYTE_SIZE);	// in 0.5 deg
	temperature >>= 1;																	// in 1 deg
	temperature <<= 4;																	// in 1/16 deg
    temperature -= 4;																	// -0,25
    temperature += scratchpad[COUNT_PER_C] - scratchpad[COUNT_REMAIN];					// add fractions

	//temperature >>= 1;
	//temperature = ((float)temperature-0.25+(16.0-scratchpad[COUNT_REMAIN])/16.0) * 16.0;

	//for (i=0; i<SCRATCHPAD_SIZE; i++) {
	//uartSendString(int2string(scratchpad[i]));
	//uartNL();
	//}

	temp_read_callback(temperature);
}

OW_CALLBACK {
	static uint16_t count = 0;
	/*uartSendChar(state+'0');
	uartSendChar(' ');
	uartSendChar(substate+'0');
	uartSendChar(' ');
	uartSendChar(owState+'0');
	uartSendChar(' ');
*/
	switch(owState) {
	case RESET_ERROR:
		state = IDLE;
		temp_read_callback(TEMP_ERROR);
		break;

	case RESET_SUCCCESS:
		// write "skip ROM" command (0xcc)
		substate = SKIP_ROM;
		sendByte(COM_SKIP_ROM);
		break;

	case WRITE_SUCCESS:
		if (substate == SKIP_ROM) {
			if (sendingByte() != true) {
				substate = ACTION;
				// send command
				sendByte(nextCommand);
			}
		}
		else {
			switch(state) {
			case CONVERT:
				if (sendingByte() != true) {
					// wait for conversion
					count = 0;
					state = WAIT;
					owRead();
					count = 0;
				}
				break;
			case ISSUE_READ:
				if (sendingByte() != true) {
					// read scratchpad
					state = READ_SCRATCHPAD;
					pos = 0;
					scratchpadPos = 0;
					owRead();
				}
				break;

			default:
				// do nothing
				break;
			}
		}
		break;

	case READ_0:
		switch(state) {
		case WAIT:
			// conversion still in progress
			count++;
			if (count > 10000) {		// read timeout
				state = IDLE;
				temp_read_callback(TEMP_ERROR);
				break;
			}
			owRead();
			break;
		case READ_SCRATCHPAD:
			readingScratchpad(false);
			break;
		default:
			// do nothing
			break;
		}
		break;

	case READ_1:
		switch(state) {
		case WAIT:
			// conversion finish, start read scratchpad
			state = ISSUE_READ;
			nextCommand = COM_READ_SCRATCHPAD;
			substate = RESET;
			owReset();
			break;
		case READ_SCRATCHPAD:
			readingScratchpad(true);
			break;
		default:
			// do nothing
			break;
		}
		break;
	default:
		// do nothing
		break;
	}
}

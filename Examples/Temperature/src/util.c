/*
 * Temperature Control
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 */

#include "util.h"
#include <stdlib.h>

static char buffer[7];

uint8_t nibble2Ascii(uint8_t val) {
	val &= 0x0f;

	if (val < 10)
		return val + '0';
	else
		return val - 10 + 'a';
}

// the pointer is valid until the next conversion
char *int2string(int16_t val) {
	itoa(val, buffer, 10);
	return buffer;
}

// the pointer is valid until the next conversion
char *hex2string(int16_t val) {
	itoa(val, buffer, 16);
	return buffer;
}

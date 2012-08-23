#ifndef UTIL_H_
#define UTIL_H_

#include <inttypes.h>

typedef enum {
	false	= 0,
	true	= 1,
} bool;


uint8_t nibble2Ascii(uint8_t val);
char *int2string(int16_t val);
char *hex2string(int16_t val);
#endif /* UTIL_H_ */

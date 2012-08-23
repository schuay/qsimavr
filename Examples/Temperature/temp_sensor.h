#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#include <inttypes.h>

void tempInit();
void tempStartRead();

#define TEMP_READ_CALLBACK	void temp_read_callback(int16_t temperature)
#define TEMP_ERROR	0xfff

extern TEMP_READ_CALLBACK;

#endif /* TEMP_SENSOR_H_ */

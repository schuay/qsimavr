PROG = avrprog2
GCC = avr-gcc
DOXYGEN = doxygen

MCU = atmega1280

CFLAGS = -mmcu=$(MCU) -Wall -Wextra -Werror -pedantic -Wstrict-prototypes -Os \
		 -DDEBUG -Iinclude -std=gnu99 -frename-registers -fshort-enums \
		 -fpack-struct -c -g
LDFLAGS = -mmcu=$(MCU) -Wl,-u,vfprintf -lprintf_min 

APP = avr-pong

source = src/uart.c src/uart_streams.c src/main.c src/lcd.c src/timer.c \
		 src/pong.c src/glcd.c src/glcd_hal.c
objects = $(source:.c=.o)

.PHONY: all doc

all: $(APP).elf

doc:
	$(DOXYGEN)

$(APP).elf: $(objects)
	$(GCC) $(LDFLAGS) $(objects) -o $@

%.o: %.c
	$(GCC) $(CFLAGS) $< -o $@

install: $(APP).elf
	$(PROG) --flash w:$<

clean:
	$(RM) -f $(objects) $(APP).elf
	$(RM) -rf doc/

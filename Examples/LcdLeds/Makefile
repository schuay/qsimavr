TARGET = main
MCU = atmega1280
LFLAGS = -mmcu=$(MCU)
CFLAGS = -mmcu=$(MCU) -c -Os -std=gnu99 -Wall -Wextra -pedantic -std=gnu99

objs = avr_hd44780.o atmega1280_charlcd.o

all: $(TARGET).elf

$(TARGET).elf: $(objs)
	avr-gcc $(LFLAGS) $^ -o $@

%.o: %.c
	avr-gcc $(CFLAGS) $< -o $@

install: $(TARGET).elf
	avrprog2 --flash w:$<

clean:
	rm -f $(TARGET).elf $(objs)

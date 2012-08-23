#!/bin/sh

make && \
rm -rf lcd_driver 2> /dev/null && \
mkdir lcd_driver && \
echo "== Character Display Library ==

How to use it:

Include "char_display.h" whereever you want to use the LCD functions.
Link your own object files together with the library (add "-lchar_display" to
the linker command).
E.g. call the linker in the following way:

avr-gcc main.o -mmcu=atmega16 -lchar_display -L. -o application.elf

Make sure that the header and library files are in the compilers and linkers
search path. E.g. in the same directory as your other source files ("-L." adds
the current directory to the linkers library search path).

= Example Application =

build the example.c with the following commands:

avr-gcc -mmcu=atmega16 -Os -Wall -c -o example.o example.c
avr-gcc example.o -mmcu=atmega16 -lchar_display -L. -o application.elf" > lcd_driver/readme.txt && \
echo "#include \"char_display.h\"

int main() {
	lcdInit(&PORTA);

	lcdWrite(\"Hallo Du!\");
	lcdGoto(1, 5);
	lcdWrite(\"Test\");

	lcdRefresh();

	while (1) {
		
	}
}" > lcd_driver/example.c && \
cp src/char_display.h lcd_driver/ && \
avr-ar rcs lcd_driver/libchar_display.a build/char_display.o && \
avr-strip -S lcd_driver/libchar_display.a && \
tar -pczf lcd_driver.tar.gz lcd_driver && \
rm -rf lcd_driver 2> /dev/null

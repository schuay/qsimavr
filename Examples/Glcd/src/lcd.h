#ifndef LCD_H
#define LCD_H

/**
 * @file lcd.h
 *
 * Responsible for the 16x2 LCD.
 */

/**
 * Initializes the 16x2 character LCD to
 * 4 bit data length, 2 display lines, 5x11 font.
 * The display is enabled, cleared, and the cursor
 * is set to the top left corner.
 * Interrupts must be enabled.
 */
void lcd_init(void);

/**
 * Prints c to (row, col).
 */
void lcd_putchar(char c, uint8_t row, uint8_t col);

/**
 * Prints s to (row, col) char by char,
 * until either '\0' in s or the EOL is reached.
 * s is located in program space.
 */
void lcd_putstr_P(const char *s, uint8_t row, uint8_t col);

/**
 * Clears the display.
 */
void lcd_clear(void);

#endif /* LCD_H */

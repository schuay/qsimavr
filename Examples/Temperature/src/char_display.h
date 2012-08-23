/*
 * Character LCD (WH1602B)
 *
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	14.03.2011
 * modified:06.05.2011
 *		- made port configurable
 */

/*
 * Note: Changes in this file have no effect, because
 * the code in the library is already compiled!
 */

#ifndef CHAR_DISPLAY_H_
#define CHAR_DISPLAY_H_

#include <avr/io.h>

// interface
extern void lcdInit(volatile uint8_t *portAddress);
extern void lcdClear(void);
extern void lcdWrite(char *string);
extern void lcdGoto(uint8_t line, uint8_t col);
extern void lcdRefresh(void);

// internals
#define	RS			PB6
#define	RW			PB7
#define	EN			PB4
#define	DATA_MASK	0x0f

#define LINES		2
#define LINE_WIDTH	16

#endif /* CHAR_DISPLAY_H_ */

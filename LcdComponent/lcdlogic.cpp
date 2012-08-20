/* *************************************************************************
 *  Copyright 2012 Jakob Gruber                                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ************************************************************************* */



#include "lcdlogic.h"

#include <avr_ioport.h>

#define WIDTH (16)
#define HEIGHT (2)

LcdLogic::LcdLogic(QObject *parent) :
    ComponentLogic(parent)
{
}

void LcdLogic::wire(avr_t *avr)
{
    this->avr = avr;

    hd44780_init(avr, &hd44780, WIDTH, HEIGHT, this, LcdLogic::displayChanged);

    /* Connect data lines to Port C, 4-7 (bidirectional). */
    for (int i = 0; i < 4; i++) {
        avr_irq_t * iavr = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 4 + i);
        avr_irq_t * ilcd = hd44780.irq + IRQ_HD44780_D4 + i;
        // AVR -> LCD
        avr_connect_irq(iavr, ilcd);
        // LCD -> AVR
        avr_connect_irq(ilcd, iavr);
    }

    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 2),
                    hd44780.irq + IRQ_HD44780_RS);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 3),
                    hd44780.irq + IRQ_HD44780_E);
    /* RW is set to GND. */

    connected = true;
}

void LcdLogic::unwire()
{
    if (!connected) {
        return;
    }

    for (int i = 0; i < 4; i++) {
        avr_irq_t * iavr = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 4 + i);
        avr_irq_t * ilcd = hd44780.irq + IRQ_HD44780_D4 + i;

        avr_unconnect_irq(iavr, ilcd);
        avr_unconnect_irq(ilcd, iavr);
    }

    avr_unconnect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 2),
                    hd44780.irq + IRQ_HD44780_RS);
    avr_unconnect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), 3),
                    hd44780.irq + IRQ_HD44780_E);

    avr_free_irq(hd44780.irq, IRQ_HD44780_COUNT);

    connected = false;
}

static inline QString constructLine(const char *begin, uint8_t shift)
{
    const uint8_t alen = qMin(WIDTH, HD44780_ROW_LENGTH - shift);
    QByteArray a(begin + shift, alen);
    QByteArray b(begin, WIDTH - alen);

    return a.replace('\0', ' ') + b.replace('\0', ' ');
}

void LcdLogic::displayChanged(void *instance, const hd44780_t *hd44780)
{
    LcdLogic *p = (LcdLogic *)instance;
    const char *c = (const char *)hd44780->vram;
    emit p->textChanged(constructLine(c, hd44780->shift),
                        constructLine(c + HD44780_ROW2_START, hd44780->shift));
}

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



#include "glcdlogic.h"

#include <avr_ioport.h>

#define _BV(bit) (1 << bit)

enum {
    IRQ_GLCD_CS1,   /**< Low active. */
    IRQ_GLCD_CS2,   /**< Low active. */
    IRQ_GLCD_RS,
    IRQ_GLCD_RW,
    IRQ_GLCD_E,

    IRQ_GLCD_D0,
    IRQ_GLCD_D1,
    IRQ_GLCD_D2,
    IRQ_GLCD_D3,
    IRQ_GLCD_D4,
    IRQ_GLCD_D5,
    IRQ_GLCD_D6,
    IRQ_GLCD_D7,

    IRQ_GLCD_RST,   /**< Low active. */

    IRQ_GLCD_COUNT
};

static const char *irq_names[] = {
    "<glcd.CS1",
    "<glcd.CS2",
    "<glcd.RS",
    "<glcd.RW",
    "<glcd.E",
    "=glcd.D0",
    "=glcd.D1",
    "=glcd.D2",
    "=glcd.D3",
    "=glcd.D4",
    "=glcd.D5",
    "=glcd.D6",
    "=glcd.D7",
    "<glcd.RST"
};

GlcdLogic::GlcdLogic()
{
    avr = NULL;
    irq = NULL;

    pinstate = 0;
}

void GlcdLogic::connect(avr_t *avr)
{
    this->avr = avr;

    chip1.connect(avr);
    chip2.connect(avr);

    irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_GLCD_COUNT, irq_names);
    for (int i = 0; i < IRQ_GLCD_COUNT; i++) {
        avr_irq_register_notify(irq + i, GlcdLogic::pinChangedHook, this);
    }

    /* Wire data pins. */
    for (int i = 0; i < 8; i++) {
        avr_irq_t *irq_avr = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('A'), i);
        avr_irq_t *irq_lcd = irq + IRQ_GLCD_D0 + i;

        avr_connect_irq(irq_avr, irq_lcd);
        avr_connect_irq(irq_lcd, irq_avr);
    }

    /* And all others. */
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 2), irq + IRQ_GLCD_CS1);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 3), irq + IRQ_GLCD_CS2);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 4), irq + IRQ_GLCD_RS);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 5), irq + IRQ_GLCD_RW);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 6), irq + IRQ_GLCD_E);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 7), irq + IRQ_GLCD_RST);
}

void GlcdLogic::disconnect()
{
    avr_free_irq(irq, IRQ_GLCD_COUNT);
    irq = NULL;
    avr = NULL;

    chip1.disconnect();
    chip2.disconnect();
}

void GlcdLogic::pinChangedHook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    GlcdLogic *p = (GlcdLogic *)param;
    p->pinChanged(irq, value);
}

void GlcdLogic::pinChanged(avr_irq_t *irq, uint32_t value)
{
    /** Pins are only processed by the GLCD on falling E edges. */
    bool fallingE = (irq->irq == IRQ_GLCD_E && value == 0 && (pinstate & _BV(IRQ_GLCD_E)) != 0);

    pinstate = (pinstate & ~_BV(irq->irq)) | (value << irq->irq);

    if (fallingE) {
        qDebug("%s: 0x%04x", __PRETTY_FUNCTION__, pinstate);
    }
}

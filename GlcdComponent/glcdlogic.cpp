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
#include <sim_time.h>

#define _BV(bit) (1 << bit)
#define DATA_PORT ('A')

static const char *irq_names[] = {
    "=glcd.D0",
    "=glcd.D1",
    "=glcd.D2",
    "=glcd.D3",
    "=glcd.D4",
    "=glcd.D5",
    "=glcd.D6",
    "=glcd.D7",
    "<glcd.RW",
    "<glcd.RS",
    "<glcd.RST",
    "<glcd.E",
    "<glcd.CS1",
    "<glcd.CS2",
};

#define NS_PER_SEC (1000000000UL)
#define NS_E_HIGH_LOW (475UL)     /**< Includes rise/fall time. */

GlcdLogic::GlcdLogic()
{
    avr = NULL;
    irq = NULL;

    pinESet = false;

    QObject::connect(&chip1, SIGNAL(transmit(uint8_t)),
                     this, SLOT(transmit(uint8_t)), Qt::DirectConnection);
    QObject::connect(&chip2, SIGNAL(transmit(uint8_t)),
                     this, SLOT(transmit(uint8_t)), Qt::DirectConnection);
    QObject::connect(&chip1, SIGNAL(pageChanged(QPoint,uint8_t)),
                     this, SLOT(chipPageChanged(QPoint,uint8_t)), Qt::DirectConnection);
    QObject::connect(&chip2, SIGNAL(pageChanged(QPoint,uint8_t)),
                     this, SLOT(chipPageChanged(QPoint,uint8_t)), Qt::DirectConnection);
}

void GlcdLogic::wireHook(avr_t *avr)
{
    this->avr = avr;

    chip1.connect(avr);
    chip2.connect(avr);

    touchscreen.wire(avr);

    /* Calculate timing cycles. */
    cyclesELowHigh = (avr->frequency * NS_E_HIGH_LOW) / NS_PER_SEC;

    /* Reset internal state. */
    lastEChange = avr->cycle;
    lastReset = avr->cycle;
    reentrant = false;

    /* Allocated IRQs and register callbacks. */
    irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_GLCD_COUNT, irq_names);

    avr_irq_register_notify(irq + IRQ_GLCD_E, GlcdLogic::pinChangedHook, this);
    avr_irq_register_notify(irq + IRQ_GLCD_RST, GlcdLogic::pinChangedHook, this); /* TODO */

    /* Wire data pins. */
    for (int i = 0; i < 8; i++) {
        avr_irq_t *irq_avr = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(DATA_PORT), i);
        avr_irq_t *irq_lcd = irq + IRQ_GLCD_D0 + i;
        avr_connect_irq(irq_lcd, irq_avr);
    }

    /* And all others. */
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 6), irq + IRQ_GLCD_E);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 7), irq + IRQ_GLCD_RST);

    /* Initialize the VCD file. */
    avr_vcd_init(avr, "glcd.vcd", &vcdFile, 5);
    for (int i = 0; i < IRQ_GLCD_RW; i++) {
        avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(DATA_PORT), i),
                           1, irq_names[i]);
    }
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 2),
                       1, irq_names[IRQ_GLCD_CS1]);
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 3),
                       1, irq_names[IRQ_GLCD_CS2]);
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 4),
                       1, irq_names[IRQ_GLCD_RS]);
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 5),
                       1, irq_names[IRQ_GLCD_RW]);
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 6),
                       1, irq_names[IRQ_GLCD_E]);
    avr_vcd_add_signal(&vcdFile,avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('E'), 7),
                       1, irq_names[IRQ_GLCD_RST]);
}

void GlcdLogic::unwireHook()
{
    avr_free_irq(irq, IRQ_GLCD_COUNT);
    avr_vcd_close(&vcdFile);

    irq = NULL;
    avr = NULL;

    chip1.disconnect();
    chip2.disconnect();

    touchscreen.unwire();
}

void GlcdLogic::pinChangedHook(struct avr_irq_t *irq, uint32_t value, void *param)
{
    GlcdLogic *p = (GlcdLogic *)param;
    p->pinChanged(irq, value);
}

void GlcdLogic::pinChanged(avr_irq_t *irq, uint32_t value)
{
    if (reentrant) {
        return;
    }

    /* We only care about E, the rest is reconstructed. */
    if (irq->irq != IRQ_GLCD_E) {
        return;
    }

    /* Timing checks. */
    if (lastEChange + cyclesELowHigh > avr->cycle) {
        qDebug("GLCD: Minimum E high/low level width exceeded.");
    }
    lastEChange = avr->cycle;

    /* Update our internal pin state. */
    bool pinEChanged = (value != 0) != pinESet;
    pinESet = (value != 0);

    uint16_t pinstate = reconstructPins();

    /* Process command if E just changed. */
    if (pinEChanged) {
        reentrant = true;
        if (!(pinstate & _BV(IRQ_GLCD_CS1))) {
            chip1.processCommand(pinstate);
        }
        if (!(pinstate & _BV(IRQ_GLCD_CS2))) {
            chip2.processCommand(pinstate);
        }
        reentrant = false;
    }
}

uint16_t GlcdLogic::reconstructPins()
{
    avr_ioport_state_t state;
    if (avr_ioctl(avr, AVR_IOCTL_IOPORT_GETSTATE(DATA_PORT), &state) != 0) {
        qDebug("GLCD: avr_ioctl failed");
    }
    const uint8_t porta = state.port;
    if (avr_ioctl(avr, AVR_IOCTL_IOPORT_GETSTATE('E'), &state) != 0) {
        qDebug("GLCD: avr_ioctl failed");
    }
    const uint8_t porte = state.port;

    uint16_t pins = porta;
    pins |= ((porte >> 2) & 1) << IRQ_GLCD_CS1;
    pins |= ((porte >> 3) & 1) << IRQ_GLCD_CS2;
    pins |= ((porte >> 4) & 1) << IRQ_GLCD_RS;
    pins |= ((porte >> 5) & 1) << IRQ_GLCD_RW;
    pins |= ((porte >> 6) & 1) << IRQ_GLCD_E;
    pins |= ((porte >> 7) & 1) << IRQ_GLCD_RST;

    return pins;
}

void GlcdLogic::transmit(uint8_t data)
{
    avr_ioport_state_t state;
    if (avr_ioctl(avr, AVR_IOCTL_IOPORT_GETSTATE(DATA_PORT), &state) != 0) {
        qDebug("GLCD: avr_ioctl failed");
    }

    for (int i = 0; i < 8; i++) {
        if (state.ddr & (1 << i)) {
            qDebug("GLCD: Data port pin %d set to output, cannot write", i);
            continue;
        }
        avr_raise_irq(irq + IRQ_GLCD_D0 + i, (data >> i) & 0x01);
    }
}

void GlcdLogic::chipPageChanged(QPoint coords, uint8_t value)
{
    QPoint offset;
    if (qobject_cast<NT7108 *>(sender()) == &chip2) {
        offset.setX(NT7108_WIDTH);
    }

    emit pageChanged(coords + offset, value);
}

void GlcdLogic::pressed(QPoint coord)
{
    touchscreen.pressed(coord);
}

void GlcdLogic::released()
{
    touchscreen.released();
}

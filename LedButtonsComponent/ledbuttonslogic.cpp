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



#include "ledbuttonslogic.h"

#include <avr_ioport.h>
#include <stdio.h>

/* TODO
 * Buttons should respect DDRx and pullup settings.
 */

struct CallbackData {
    LedButtonsLogic *instance;
    QChar port;
};

const char ports[] = "ABCDEFGHJKL";

LedButtonsLogic::LedButtonsLogic(QObject *parent) :
    ComponentLogic(parent)
{
}

void LedButtonsLogic::wire(avr_t *avr)
{
    this->avr = avr;

    /* Every port has 8 pins. */
    const int count = strlen(ports) * 8;

    /* Construct the port names. */
    int nameLength = sizeof("PORTA0");
    const char *names[count];
    char _names[count * nameLength];
    char *s = _names;
    for (int i = 0; i < count; i++) {
        int port = i / 8;
        int pin = i % 8;

        snprintf(s, nameLength, "PORT%c%d", ports[port], pin);
        names[i] = s;
        s += nameLength;
    }

    irq = avr_alloc_irq(&avr->irq_pool, 0, count, names);

    for (int i = 0; i < count; i++) {
        int port = i / 8;
        int pin = i % 8;

        avr_connect_irq(irq + i, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(ports[port]), pin));

        /* We need this helper struct to keep track of 1) which port the IRQ came from,
         * and 2) which class instance to notify. */
        CallbackData *d = new CallbackData;
        d->instance = this;
        d->port = 'A' + port;
        callbackData.append(d);

        avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(ports[port]), pin),
                                LedButtonsLogic::pinChanged, d);
    }

    connected = true;
}

void LedButtonsLogic::unwire()
{
    if (!connected) {
        return;
    }

    const int count = strlen(ports) * 8;

    for (int i = 0; i < count; i++) {
        int port = i / 8;
        int pin = i % 8;

        avr_irq_unregister_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(ports[port]), pin),
                                  LedButtonsLogic::pinChanged, callbackData[i]);
        delete callbackData[i];
    }
    callbackData.clear();
    avr_free_irq(irq, count);

    irq = NULL;

    connected = false;
}

void LedButtonsLogic::buttonPressed(QChar port, uint8_t pin)
{
    avr_raise_irq(irq + (port.toAscii() - 'A') * 8 + pin, 0);
}

void LedButtonsLogic::buttonReleased(QChar port, uint8_t pin)
{
    avr_raise_irq(irq + (port.toAscii() - 'A') * 8 + pin, 1);
}

void LedButtonsLogic::pinChanged(struct avr_irq_t * irq, uint32_t value, void * param)
{
    CallbackData *d = (CallbackData *)param;
    emit d->instance->ledChanged(d->port, irq->irq % 8, value == 1);
}

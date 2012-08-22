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



#include "touchscreen.h"

#include <string.h>

#include "nt7108.h"

#define POINT_NEUTRAL (QPoint(0, NT7108_HEIGHT))
#define VOLTAGE (2700)

Touchscreen::Touchscreen()
{
}

void Touchscreen::wire(avr_t *avr)
{
    this->avr = avr;
    this->coord = POINT_NEUTRAL;

    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER),
                            Touchscreen::outTriggerHook,
                            this);
}

void Touchscreen::unwire()
{
    avr_irq_unregister_notify(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, ADC_IRQ_OUT_TRIGGER),
                              Touchscreen::outTriggerHook,
                              this);
    this->avr = NULL;
}

void Touchscreen::pressed(const QPoint &coord)
{
    this->coord = coord;
}

void Touchscreen::released()
{
    this->coord = POINT_NEUTRAL;
}

void Touchscreen::outTriggerHook(struct avr_irq_t *, uint32_t value, void *param)
{
    Touchscreen *t = (Touchscreen *)param;
    avr_adc_mux_t mux;
    memcpy(&mux, &value, sizeof(mux));
    t->outTrigger(mux);
}

/* TODO: handle driver inputs. If none are on, return 0. If both are on, return a random value.
 * Otherwise, return the actual calculated voltage as below.
 */

void Touchscreen::outTrigger(avr_adc_mux_t mux)
{
    uint32_t val = 0;

    switch(mux.src) {

    /* X axis. */
    case ADC_IRQ_ADC0: val = coord.x() * VOLTAGE / (NT7108_WIDTH * 2); break;

    /* Y axis. */
    case ADC_IRQ_ADC1: val = (NT7108_HEIGHT - coord.y()) * VOLTAGE / (NT7108_HEIGHT); break;

    /* Ignore all other ADC requests. */
    default: return;
    }

    avr_raise_irq(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, mux.src), val);
}

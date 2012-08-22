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

Touchscreen::Touchscreen()
{
}

void Touchscreen::wire(avr_t *avr)
{
    this->avr = avr;

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
}

void Touchscreen::released()
{

}

void Touchscreen::outTriggerHook(struct avr_irq_t *, uint32_t value, void *param)
{
    Touchscreen *t = (Touchscreen *)param;
    avr_adc_mux_t mux;
    memcpy(&mux, &value, sizeof(mux));
    t->outTrigger(mux);
}

void Touchscreen::outTrigger(avr_adc_mux_t mux)
{
    static int val = 3000;
    avr_raise_irq(avr_io_getirq(avr, AVR_IOCTL_ADC_GETIRQ, mux.src), val);
    val -= 30;
}

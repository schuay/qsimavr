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



#include "temperaturelogic.h"

#include <avr_ioport.h>

enum {
    IRQ_TEMP_DQ,    /**< Data pin, incoming. */
    IRQ_TEMP_OUT,   /**< Data pin, outgoing (for debug purposes). */
    IRQ_TEMP_DDR,   /**< Data direction. */
    IRQ_TEMP_COUNT
};

static const char *irq_names[] = {
    "<temp.data",
    ">temp.data",
    "<temp.ddr",
};

#define PORT    ('G')
#define PIN     (0)

TemperatureLogic::TemperatureLogic()
{
    connect(&ds1820, SIGNAL(setPin()), this, SLOT(setPin()));
}

void TemperatureLogic::wire(avr_t *avr)
{
    this->avr = avr;
    output = false;
    reentrant = false;
    port = 0;

    ds1820.wire(avr);

    /* Note that different IRQs are connected to in- and output directions.
     * IRQ_TEMP_DQ receives signals from simavr (including our own output).
     * IRQ_TEMP_OUT sends signals to simavr (and is oblivious to other changes of the source IRQ).
     */
    irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_TEMP_COUNT, irq_names);
    avr_irq_register_notify(irq + IRQ_TEMP_DQ, TemperatureLogic::pinChangedHook, this);
    avr_irq_register_notify(irq + IRQ_TEMP_DDR, TemperatureLogic::ddrChangedHook, this);
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), PIN), irq + IRQ_TEMP_DQ);
    avr_connect_irq(irq + IRQ_TEMP_OUT, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), PIN));
    avr_connect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), IOPORT_IRQ_DIRECTION_ALL), irq + IRQ_TEMP_DDR);

    avr_vcd_init(avr, "temperature.vcd", &vcdFile, 10000);
    avr_vcd_add_signal(&vcdFile, irq + IRQ_TEMP_DQ, 1, irq_names[IRQ_TEMP_DQ]);
    avr_vcd_add_signal(&vcdFile, irq + IRQ_TEMP_OUT, 1, irq_names[IRQ_TEMP_OUT]);
    avr_vcd_add_signal(&vcdFile, irq + IRQ_TEMP_DDR, 1, irq_names[IRQ_TEMP_DDR]);

    if (vcdEnabled) {
        avr_vcd_start(&vcdFile);
    }

    connected = true;
}

void TemperatureLogic::unwire()
{
    if (!connected) {
        return;
    }

    avr_unconnect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), PIN), irq + IRQ_TEMP_DQ);
    avr_unconnect_irq(irq + IRQ_TEMP_OUT, avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), PIN));
    avr_unconnect_irq(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ(PORT), IOPORT_IRQ_DIRECTION_ALL), irq + IRQ_TEMP_DDR);
    avr_irq_unregister_notify(irq + IRQ_TEMP_DQ, TemperatureLogic::pinChangedHook, this);
    avr_irq_unregister_notify(irq + IRQ_TEMP_DDR, TemperatureLogic::ddrChangedHook, this);
    avr_free_irq(irq, IRQ_TEMP_COUNT);

    avr_vcd_close(&vcdFile);

    connected = false;
}

void TemperatureLogic::ddrChangedHook(avr_irq_t *, uint32_t value, void *param)
{
    TemperatureLogic *p = (TemperatureLogic *)param;
    p->ddrChanged((value >> PIN) & 1);
}

void TemperatureLogic::ddrChanged(uint32_t value)
{
    bool output = (value == 1);
    if (this->output == output) {
        return;
    }
    this->output = output;

    if (output) {
        ds1820.pinChanged(port);
        setPin();
    } else {
        /* The pin has just been switched to input and the ds1820 takes control. */
        ds1820.pinChanged(1);
        setPin();
    }
}

void TemperatureLogic::pinChangedHook(avr_irq_t *, uint32_t value, void *param)
{
    TemperatureLogic *p = (TemperatureLogic *)param;
    p->pinChanged(value);
}

void TemperatureLogic::pinChanged(uint32_t value)
{
    if (reentrant) {
        return;
    }

    if (!output) {
        /* Schedule immediate setting of pin. We can't do this by IRQ because
         * the nested IRQ change would be immediately overwritten.
         * This assumes that there is no internal pull-up set.
         */
        port = value;
        avr_cycle_timer_register(avr, 0, setPinHook, this);
        return;
    }

    ds1820.pinChanged(value);
}

avr_cycle_count_t TemperatureLogic::setPinHook(avr_t *, avr_cycle_count_t, void *param)
{
    TemperatureLogic *p = (TemperatureLogic *)param;
    p->setPin();
    return 0;
}

void TemperatureLogic::setPin()
{
    reentrant = true;
    avr_raise_irq(irq + IRQ_TEMP_OUT, ds1820.pinLevel());
    reentrant = false;
}

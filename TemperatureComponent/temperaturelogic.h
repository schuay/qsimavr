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



#ifndef TEMPERATURELOGIC_H
#define TEMPERATURELOGIC_H

#include <component.h>

#include "ds1820.h"

class TemperatureLogic : public ComponentLogic
{
    Q_OBJECT

public:
    TemperatureLogic();

protected:
    void wireHook(avr_t *avr);
    void unwireHook();

private:
    static void ddrChangedHook(struct avr_irq_t *irq, uint32_t value, void *param);
    void ddrChanged(uint32_t value);

    static void pinChangedHook(struct avr_irq_t *irq, uint32_t value, void *param);
    void pinChanged(uint32_t value);

    static avr_cycle_count_t setPinHook(struct avr_t *, avr_cycle_count_t, void *param);

private slots:
    void setPin();

private:
    avr_t *avr;
    avr_irq_t *irq;

    DS1820 ds1820;

    uint8_t port; /**< Keeps track of port settings (restored when DDR switched to output). */
    bool output;
    bool reentrant;
};

#endif // TEMPERATURELOGIC_H

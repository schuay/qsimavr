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



#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <QPoint>
#include <avr_adc.h>
#include <sim_avr.h>

class Touchscreen
{
public:
    Touchscreen();

    void wire(avr_t *avr);
    void unwire();

    void pressed(const QPoint &coord);
    void released();

private:
    void outTrigger(avr_adc_mux_t mux);
    static void outTriggerHook(struct avr_irq_t *irq, uint32_t value, void *param);

    bool drivePinsValid(unsigned long adc);

private:
    avr_t *avr;

    QPoint coord;
};

#endif // TOUCHSCREEN_H

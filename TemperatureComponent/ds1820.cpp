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



#include "ds1820.h"

#include <sim_time.h>

DS1820::DS1820(QObject *parent) :
    QObject(parent)
{
}

void DS1820::wire(avr_t *avr)
{
    this->avr = avr;
    lastChange = 0;
    level = 1;
}

void DS1820::pinChanged(uint8_t level)
{
    uint32_t duration = avr_cycles_to_usec(avr, avr->cycle - lastChange);
    lastChange = avr->cycle;

    if (level == 1 && duration > 480) {
        this->level = 0;
        avr_cycle_timer_register_usec(avr, 15 + 60 + 10, DS1820::timerHook, this);
        emit setPin();
    }
}

avr_cycle_count_t DS1820::timerHook(avr_t *, avr_cycle_count_t, void *param)
{
    DS1820 *p = (DS1820 *)param;
    p->timer();
    return 0;
}

void DS1820::timer()
{
    level = 1;
    emit setPin();
}

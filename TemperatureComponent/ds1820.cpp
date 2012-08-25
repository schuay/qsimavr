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

#define FUDGE (10)
#define MASTER_RESET_MIN (480 - FUDGE)
#define MASTER_WRITE0_MIN (60 - FUDGE)
#define MASTER_WRITE0_MAX (120 + FUDGE)
#define MASTER_WRITE1_MAX (15 + FUDGE)
#define DS1820_RESET_WAIT_MIN (15 - FUDGE)
#define DS1820_RESET_WAIT_MAX (60 + FUDGE)
#define DS1820_PRESENCE_MIN (60 - FUDGE)
#define DS1820_PRESENCE_MAX (240 + FUDGE)

DS1820::DS1820(QObject *parent) :
    QObject(parent)
{
}

void DS1820::wire(avr_t *avr)
{
    this->avr = avr;
    lastChange = 0;
    level = 1;
    state = IDLE;
}

void DS1820::pinChanged(uint8_t level)
{
    if (this->level == level) {
        return;
    }

    const uint32_t duration = avr_cycles_to_usec(avr, avr->cycle - lastChange);
    lastChange = avr->cycle;
    this->level = level;

    const bool low = (level == 0);
    if (low) {
        return;
    }

    /* RESET works during all states. */
    if (duration > MASTER_RESET_MIN) {
        in = 0;
        incount = 0;
        state = RESET_WAIT;
        wait(DS1820_RESET_WAIT_MIN);
        return;
    }

    switch (state) {

    case ROM_COMMAND:

        if (MASTER_WRITE0_MIN < duration && duration < MASTER_WRITE0_MAX) { /* WRITE 0. */
            in |= 0 << incount;
            incount++;
        } else if (duration < MASTER_WRITE1_MAX) { /* WRITE 0. */
            in |= 1 << incount;
            incount++;
        } else {
            qDebug("%s: unrecognized low interval %d us", __PRETTY_FUNCTION__, duration);
        }
        break;

    default:
        qDebug("%s: state not handled", __PRETTY_FUNCTION__);
    }

    if (incount == 8) {
        qDebug("%s received ROM command 0x%02x", __PRETTY_FUNCTION__, in);
        incount = 0;
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
    switch (state) {

    case RESET_WAIT:
        wait(DS1820_PRESENCE_MAX);
        state = PRESENCE_PULSE;
        level = 0;
        emit setPin();
        break;

    case PRESENCE_PULSE:
        state = ROM_COMMAND;
        level = 1;
        emit setPin();
        break;

    default:
        qDebug("%s: state not handled", __PRETTY_FUNCTION__);
    }
}

void DS1820::wait(uint32_t usec)
{
    avr_cycle_timer_cancel(avr, DS1820::timerHook, this);
    avr_cycle_timer_register_usec(avr, usec, DS1820::timerHook, this);
}

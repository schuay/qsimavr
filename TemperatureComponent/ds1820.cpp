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
#define DS1820_READ1_MIN (15 + FUDGE)

#define ROM_READ_ROM (0x33)
#define ROM_MATCH_ROM (0x55)
#define ROM_SEARCH_ROM (0xf0)
#define ROM_ALARM_SEARCH (0xec)
#define ROM_SKIP_ROM (0xcc)

/** DS18S20 family code: 0x10; CRC: 0x37. */
#define ROM_ID (0x370008022e49fb10)
#define ROM_ID_BITS (64)

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
        qDebug("%s: RESET", __PRETTY_FUNCTION__);
        in = 0;
        incount = 0;
        out = 0;
        outcount = 0;
        state = RESET_WAIT;
        wait(DS1820_RESET_WAIT_MIN);
        return;
    }

    switch (state) {

    case ROM_COMMAND:

        in |= read(duration) << incount;
        incount++;

        if (incount == 8) {
            romCommand();
        }

        break;

    case SEARCH_ROM:

        if (outcount > 0) { /* One of the two write slots per bit. */
            write(out & 1);
            outcount--;
            out >>= 1;
        } else { /* A read slot. */
            if ((read(duration) ^ (ROM_ID >> incount)) & 1) {
                state = IDLE;
                incount = 0;
                in = 0;
            }

            incount++;

            if (incount == ROM_ID_BITS) {
                state = FUNCTION_COMMAND;
                incount = 0;
                in = 0;
            } else {
                out = (ROM_ID >> incount) & 1;
                out |= (out ^ 1) << 1;
                outcount = 2;
            }
        }

        break;

    default:
        qDebug("%s: state not handled", __PRETTY_FUNCTION__);
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

    case SEARCH_ROM:
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

void DS1820::romCommand()
{
    switch (in) {

    case ROM_READ_ROM:
        qDebug("%s: READ ROM", __PRETTY_FUNCTION__);
        break;

    case ROM_MATCH_ROM:
        qDebug("%s: MATCH ROM", __PRETTY_FUNCTION__);
        break;

    case ROM_SEARCH_ROM:
        qDebug("%s: SEARCH ROM", __PRETTY_FUNCTION__);
        state = SEARCH_ROM;
        out = 0;
        out |= ROM_ID & 1;
        out |= (out ^ 1) << 1;
        outcount = 2;
        break;

    case ROM_ALARM_SEARCH:
        qDebug("%s: ALARM SEARCH", __PRETTY_FUNCTION__);
        break;

    case ROM_SKIP_ROM:
        qDebug("%s: SKIP ROM", __PRETTY_FUNCTION__);
        break;

    default:
        qDebug("%s: unrecognized ROM command 0x%02x", __PRETTY_FUNCTION__, in);
    }

    incount = 0;
    in = 0;
}

void DS1820::write(uint8_t bit)
{
    if (bit != 0) {
        return; /* Do nothing == 1. */
    }

    wait(DS1820_READ1_MIN);
    level = 0;
    emit setPin();
}

uint8_t DS1820::read(uint32_t duration) const
{
    if (MASTER_WRITE0_MIN < duration && duration < MASTER_WRITE0_MAX) { /* WRITE 0. */
        return 0;
    } else if (duration < MASTER_WRITE1_MAX) { /* WRITE 0. */
        return 1;
    } else {
        qDebug("%s: unrecognized low interval %d us", __PRETTY_FUNCTION__, duration);
        return 0;
    }
}

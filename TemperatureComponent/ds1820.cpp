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

#include "crc8.h"

#if 0
#define DEBUG(...) do { qDebug(__VA_ARGS__); } while (0)
#else
#define DEBUG(...)
#endif

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
#define DS1820_RECALL_MIN (5)
#define DS1820_CONVERT_MIN (5)

#define ROM_READ_ROM (0x33)
#define ROM_MATCH_ROM (0x55)
#define ROM_SEARCH_ROM (0xf0)
#define ROM_ALARM_SEARCH (0xec)
#define ROM_SKIP_ROM (0xcc)

#define FUN_CONVERT_T (0x44)
#define FUN_WRITE_SCRATCHPAD (0x4e)
#define FUN_READ_SCRATCHPAD (0xbe)
#define FUN_COPY_SCRATCHPAD (0x48)
#define FUN_RECALL_E (0xb8)
#define FUN_READ_POWER_SUPPLY (0xb4)

/** DS18S20 family code: 0x10; CRC: 0x37. */
#define ROM_ID (0x370008022e49fb10)
#define ROM_ID_BITS (64)

enum {
    SCRATCHPAD_TEMP_LSB = 0,
    SCRATCHPAD_TEMP_MSB,
    SCRATCHPAD_TH_REG,
    SCRATCHPAD_TL_REG,
    SCRATCHPAD_RESERVED1,
    SCRATCHPAD_RESERVED2,
    SCRATCHPAD_COUNT_REMAIN,
    SCRATCHPAD_COUNT_PER_C,
    SCRATCHPAD_CRC,
    SCRATCHPAD_COUNT
};

#define EEPROM_BYTES (2)

#define BITS_PER_BYTE (8)

DS1820::DS1820(QObject *parent) :
    QObject(parent)
{
    scratchpad.fill(0x00, SCRATCHPAD_COUNT);

    setScratchpad(SCRATCHPAD_TEMP_LSB, 0xaa);
    setScratchpad(SCRATCHPAD_TEMP_MSB, 0x00);
    setScratchpad(SCRATCHPAD_TH_REG, 0x00);
    setScratchpad(SCRATCHPAD_TL_REG, 0x00);
    setScratchpad(SCRATCHPAD_RESERVED1, 0xff);
    setScratchpad(SCRATCHPAD_RESERVED2, 0xff);
    setScratchpad(SCRATCHPAD_COUNT_REMAIN, 0x0c);
    setScratchpad(SCRATCHPAD_COUNT_PER_C, 0x10);

    eeprom.fill(0x00, EEPROM_BYTES);
}

void DS1820::setScratchpad(uint8_t index, uint8_t data)
{
    if (index >= SCRATCHPAD_COUNT || scratchpad.at(index) == data) {
        return;
    }

    scratchpad[index] = data;
    scratchpad[SCRATCHPAD_CRC] = crc8((uint8_t *)scratchpad.data(), SCRATCHPAD_COUNT - 1);

    emit scratchpadChanged(scratchpad);
}

void DS1820::setEeprom(uint8_t index, uint8_t data)
{
    if (index >= EEPROM_BYTES || eeprom.at(index) == data) {
        return;
    }

    eeprom[index] = data;

    emit eepromChanged(eeprom);
}

void DS1820::setScratchpad(QByteArray data)
{
    scratchpad = data;
    scratchpad[SCRATCHPAD_CRC] = crc8((uint8_t *)scratchpad.data(), SCRATCHPAD_COUNT - 1);

    emit scratchpadChanged(scratchpad);
}

void DS1820::setEeprom(QByteArray data)
{
    eeprom = data;
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
        DEBUG("%s: RESET", __PRETTY_FUNCTION__);
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

    case FUNCTION_COMMAND:

        in |= read(duration) << incount;
        incount++;

        if (incount == 8) {
            functionCommand();
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

    case MATCH_ROM:

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
        }

        break;

    case READ_SCRATCHPAD:

        write((scratchpad[outcount / BITS_PER_BYTE] >> (outcount % BITS_PER_BYTE)) & 1);
        outcount++;

        break;

    case WRITE_SCRATCHPAD: {

        uint8_t index = SCRATCHPAD_TH_REG + incount / BITS_PER_BYTE;
        uint8_t bit = incount % BITS_PER_BYTE;

        if (bit == BITS_PER_BYTE - 1 && index == SCRATCHPAD_TL_REG) {
            incount = 0;
            state = IDLE;
        }

        setScratchpad(index, (scratchpad[index] & ~(1 << bit)) | (read(duration) << bit));

        incount++;
        }
        break;

    case RECALL_E:
    case CONVERT:

        /* Assume instantaneous transfer, signal success immediately. */
        write(1);

        break;

    default:
        DEBUG("%s: state not handled", __PRETTY_FUNCTION__);
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
    case RECALL_E:
    case CONVERT:
        level = 1;
        emit setPin();
        break;

    case READ_SCRATCHPAD:
        if (outcount == BITS_PER_BYTE * SCRATCHPAD_COUNT) {
            outcount = 0;
            state = IDLE;
        }

        level = 1;
        emit setPin();
        break;

    default:
        DEBUG("%s: state not handled", __PRETTY_FUNCTION__);
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
        DEBUG("%s: READ ROM", __PRETTY_FUNCTION__);
        break;

    case ROM_MATCH_ROM:
        DEBUG("%s: MATCH ROM", __PRETTY_FUNCTION__);
        state = MATCH_ROM;
        break;

    case ROM_SEARCH_ROM:
        DEBUG("%s: SEARCH ROM", __PRETTY_FUNCTION__);
        state = SEARCH_ROM;
        out = 0;
        out |= ROM_ID & 1;
        out |= (out ^ 1) << 1;
        outcount = 2;
        break;

    case ROM_ALARM_SEARCH:
        DEBUG("%s: ALARM SEARCH", __PRETTY_FUNCTION__);
        break;

    case ROM_SKIP_ROM:
        DEBUG("%s: SKIP ROM", __PRETTY_FUNCTION__);
        state = FUNCTION_COMMAND;
        break;

    default:
        DEBUG("%s: unrecognized ROM command 0x%02x", __PRETTY_FUNCTION__, in);
    }

    incount = 0;
    in = 0;
}

void DS1820::functionCommand()
{
    switch (in) {
    case FUN_CONVERT_T:
        DEBUG("%s: CONVERT T", __PRETTY_FUNCTION__);
        state = CONVERT;
        break;

    case FUN_WRITE_SCRATCHPAD:
        DEBUG("%s: WRITE SCRATCHPAD", __PRETTY_FUNCTION__);
        state = WRITE_SCRATCHPAD;
        break;

    case FUN_READ_SCRATCHPAD:
        DEBUG("%s: READ SCRATCHPAD", __PRETTY_FUNCTION__);
        state = READ_SCRATCHPAD;
        outcount = 0;
        break;

    case FUN_COPY_SCRATCHPAD:
        DEBUG("%s: COPY SCRATCHPAD", __PRETTY_FUNCTION__);
        setEeprom(0, scratchpad[SCRATCHPAD_TH_REG]);
        setEeprom(1, scratchpad[SCRATCHPAD_TL_REG]);
        break;

    case FUN_RECALL_E:
        DEBUG("%s: RECALL E", __PRETTY_FUNCTION__);
        setScratchpad(SCRATCHPAD_TH_REG, eeprom[0]);
        setScratchpad(SCRATCHPAD_TL_REG, eeprom[1]);
        state = RECALL_E;
        break;

    case FUN_READ_POWER_SUPPLY:
        DEBUG("%s: READ POWER SUPPLY", __PRETTY_FUNCTION__);

        /* Nothing to do, high == external power. */
        break;

    default:
        DEBUG("%s: unrecognized function command 0x%02x", __PRETTY_FUNCTION__, in);
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
        DEBUG("%s: unrecognized low interval %d us", __PRETTY_FUNCTION__, duration);
        return 0;
    }
}

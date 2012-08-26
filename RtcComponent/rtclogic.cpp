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



#include "rtclogic.h"

#include <QDateTime>
#include <sim_cycle_timers.h>
#include <sim_time.h>
#include <avr_twi.h>

#define RTC_ADDR (0xd0)     /**< 1101 000x. */
#define RTC_MASK (0xfe)     /**< 1111 1110. */
#define ADDR_MASK (0x3f)    /**< 0011 1111 (wrap from 0x3f to 0x00). */
#define SRAM_SIZE (64)      /**< Size in bytes (including clock registers). */

typedef struct {

    /* 0x00 Seconds register. */

    uint8_t seconds :7;
    uint8_t clockHalt :1;   /**< 0: Oscillator enabled, 1: disabled. */

    /* 0x01 Minutes register. */

    uint8_t minutes :7;
    uint8_t :1;

    /* 0x02 Hours register. */

    uint8_t hours :6;
    uint8_t hour_mode :1;   /**< 0: 24h, 1: 12h. */
    uint8_t :1;

    /* 0x03 Day register. */

    uint8_t day :3;
    uint8_t :5;

    /* 0x04 Date register. */

    uint8_t date :6;
    uint8_t :2;

    /* 0x05 Month register. */

    uint8_t month :5;
    uint8_t :3;

    /* 0x06 Year register. */

    uint8_t year :8;

    /* 0x07 Control register. */

    uint8_t rs :2;          /**< Rate Select: set to 11 by default. */
    uint8_t :2;
    uint8_t sqwe :1;        /**< Square-Wave Enable: set to 0 by default. */
    uint8_t :2;
    uint8_t out :1;         /**< Output Control: set to 0 by default. */

    /* 0x08 - 0x3f SRAM */
} ds1307_time_mem_t;

RtcLogic::RtcLogic() : twi(this), aptr(0)
{
    /* Reset to initial state:
     *
     * Clock Halt set to 1
     * Rate Select set to 1
     *
     * MM/DD/YY DOW HH:MM:SS
     * 01/01/00  01 00:00:00 */

    ds1307_time_mem_t time;
    memset(&time, 0, sizeof(time));

    time.clockHalt = 1;
    time.day = 1;
    time.date = 1;
    time.month = 1;
    time.rs = 1;

    sram.fill(0x00, SRAM_SIZE);
    sram.replace(0, sizeof(time), (const char *)&time, sizeof(time));
}

/**
 * Converts a binary coded decimal to normal representation.
 * The number can have at most two digits. The lower nibble
 * represents 10^0, the high nibble 10^1.
 * Since the higher nibble may also represent further unrelated
 * information, it is masked using mask.
 */
static inline uint8_t fromBCD(uint8_t from, uint8_t mask)
{
    return ((from >> 4) & mask) * 10 + (from & 0b1111);
}

/**
 * Converts a number to binary coded decimal representation.
 * from must be between 0 and 99 (inclusive).
 */
static inline uint8_t toBCD(uint8_t from)
{
    return ((from / 10) << 4) + (from % 10);
}

void RtcLogic::incrementSeconds()
{
    ds1307_time_mem_t *time = (ds1307_time_mem_t *)sram.data();

    if (time->clockHalt) {
        return;
    }

    /* Increment seconds and carry through to years if necessary. */
    const uint8_t seconds = fromBCD(time->seconds, 0b111) + 1;
    time->seconds = toBCD(seconds % 60);
    uint8_t carry = seconds / 60;

    if (!carry) {
        return;
    }

    const uint8_t minutes = fromBCD(time->minutes, 0b111) + carry;
    time->minutes = toBCD(minutes % 60);
    carry = minutes / 60;

    if (!carry) {
        return;
    }

    if (time->hour_mode) {
        /* 12h AM/PM mode. The AM/PM bit is located at bit 5 (high == PM). */
        const uint8_t hours = (fromBCD(time->hours, 0b1) % 12) + 1;
        const uint8_t pm_mask = 1 << 5;
        uint8_t pm = time->hours & pm_mask;
        if (hours == 1) {   /* Rollover 12 -> 1. */
            pm = pm ^ pm_mask;
            carry = (pm == 0);
        }
        time->hours = toBCD(hours) | pm;
    } else {
        const uint8_t hours = fromBCD(time->hours, 0b11) + 1;
        time->hours = toBCD(hours % 24);
        carry = hours / 24;
    }

    if (!carry) {
        return;
    }

    /* Wrap up all remaining date calculations using QDate. */
    const uint8_t year = fromBCD(time->year, 0b1111);
    const uint8_t month = fromBCD(time->month, 0b1);
    const uint8_t date = fromBCD(time->date, 0b11);
    const QDate qtoday = QDate::fromString(
                QString("%1-%2-%3").arg(year).arg(month).arg(date),
                "yy-M-d");
    const QDate qtomorrow = qtoday.addDays(1);

    const uint8_t day = (fromBCD(time->day, 0b0) % 7) + 1;
    time->day = toBCD(day);
    time->date = toBCD(qtomorrow.day());
    time->month = toBCD(qtomorrow.month());
    time->year = toBCD(qtomorrow.year());
}

#define USECS_PER_SEC (1000 * 1000)
avr_cycle_count_t RtcLogic::incrementSecondsHook(avr_t *avr, avr_cycle_count_t when, void *param)
{
    RtcLogic *p = (RtcLogic *)param;
    p->incrementSeconds();
    return when + avr_usec_to_cycles(avr, USECS_PER_SEC);
}

void RtcLogic::resetHook()
{
    avr_cycle_timer_register_usec(avr, USECS_PER_SEC, RtcLogic::incrementSecondsHook, this);
}

void RtcLogic::wireHook(avr_t *avr)
{
    this->avr = avr;

    twi.wireHook(avr);

    avr_vcd_init(avr, "eeprom.vcd", &vcdFile, 10000);
    avr_vcd_add_signal(&vcdFile,
        avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_STATUS), 8 /* bits */ ,
        "TWSR" );
}

void RtcLogic::unwireHook()
{
    twi.unwireHook();

    avr_vcd_close(&vcdFile);
}

uint8_t RtcLogic::incrementAddress()
{
    uint8_t p = aptr;
    aptr = (aptr + 1) & ADDR_MASK;
    return p;
}

uint8_t RtcLogic::transmitByte()
{
    return sram[incrementAddress()];
}

bool RtcLogic::matchesAddress(uint8_t address)
{
    return ((address & RTC_MASK) == (RTC_ADDR & RTC_MASK) || /* Self. */
            (address & 0xfe) == 0x00); /* General call. */
}

void RtcLogic::received(const QByteArray &data)
{
    aptr = data[0] & ADDR_MASK;

    for (int i = 1; i < data.size(); i++) {
        sram[incrementAddress()] = data[i];
    }

}

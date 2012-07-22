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

void RtcLogic::connect(avr_t *avr)
{
    twi.connect(avr);
}

void RtcLogic::disconnect()
{
    twi.disconnect();
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

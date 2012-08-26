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



#ifndef RTCLOGIC_H
#define RTCLOGIC_H

#include <component.h>
#include <twicomponent.h>

class RtcLogic : public ComponentLogic, public TwiSlave
{
public:
    RtcLogic();

    uint8_t transmitByte();
    bool matchesAddress(uint8_t address);
    void received(const QByteArray &data);

protected:
    void wireHook(avr_t *avr);
    void unwireHook();

private:
    /** Increments the address pointer and returns its original value. */
    uint8_t incrementAddress();

    /** Increments the time stored in sram.
     *  Called once per second (based on avr cycle calculations). */
    void incrementSeconds();
    static avr_cycle_count_t incrementSecondsHook(avr_t *avr, avr_cycle_count_t when, void *param);

    static void resetHook(avr_io_t *io);

private:
    TwiComponent twi;

    QByteArray sram;
    uint8_t aptr;   /**< Address pointer. */

    avr_t *avr;

    /** We register as an IO module to receive reset events. */
    struct rtc_io_t {
        avr_io_t io;
        RtcLogic *instance;
    } io;
};

#endif // RTCLOGIC_H

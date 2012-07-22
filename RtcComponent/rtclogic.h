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

class RtcLogic : public QObject, public ComponentLogic, public TwiSlave
{
public:
    RtcLogic();

    void connect(avr_t *avr);
    void disconnect();

    uint8_t transmitByte();
    bool matchesAddress(uint8_t address);
    void received(const QByteArray &data);

private:
    uint8_t incrementAddress();

private:
    TwiComponent twi;

    QByteArray sram;
    uint8_t aptr;   /**< Address pointer. */
};

#endif // RTCLOGIC_H

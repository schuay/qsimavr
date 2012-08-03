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



#include "nt7108.h"

#define _BV(bits) (1 << bits)

NT7108::NT7108()
{
}

void NT7108::connect(avr_t *avr)
{
    this->avr = avr;
}

void NT7108::disconnect()
{
}

void NT7108::processCommand(uint16_t pins)
{
    uint8_t highestSetBit = IRQ_GLCD_RS;
    while (!(pins & _BV(highestSetBit))) {
        highestSetBit--;
    }

    switch (highestSetBit) {

    /* Read/write display data. Read if RW is set. */
    case IRQ_GLCD_RS:

    /* Status read. The only valid command if busy.
     * Busy 0 On/Off Reset 0 0 0 0 */
    case IRQ_GLCD_RW:

    /* Set page: 1 0 1 1 1 A A A
     * or
     * Display start line: 1 1 A A A A A */
    case IRQ_GLCD_D7:

    /* Set address: 0 1 A A A A A A */
    case IRQ_GLCD_D6:

    /* Display on/off: 0 0 1 1 1 1 1 On/Off */
    case IRQ_GLCD_D5:

    default:
        qDebug("GLCD: Invalid pin state 0x%04x, ignoring.", pins);
        return;
    }
}

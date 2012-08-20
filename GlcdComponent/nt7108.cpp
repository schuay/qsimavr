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

#define NS_DATA_DELAY (320UL)
#define NS_PER_SEC (1000000000UL)

#if 0
#define DEBUG(...) do { qDebug(__VA_ARGS__); } while (0)
#else
#define DEBUG(...)
#endif

NT7108::NT7108()
{    
    on = false;
    ram.fill(0, NT7108_HEIGHT * NT7108_WIDTH);
    yaddr = 0;
    xaddr = 0;
    zaddr = 0;
}

void NT7108::connect(avr_t *avr)
{
    this->avr = avr;
    cycleDataDelay = (avr->frequency * NS_DATA_DELAY) / NS_PER_SEC;
}

void NT7108::disconnect()
{
}

void NT7108::processCommand(uint16_t pins)
{
    /* Reads are handled by us on rising E edges,
     * writes on falling edges. */
    const bool pinESet = ((pins & _BV(IRQ_GLCD_E)) != 0);
    const bool pinRWSet = ((pins & _BV(IRQ_GLCD_RW)) != 0);
    if (pinESet != pinRWSet) {
        return;
    }

    /* Detect the highest set bit. */
    uint8_t highestSetBit = IRQ_GLCD_RS;
    while (!(pins & _BV(highestSetBit))) {
        highestSetBit--;
    }


    switch (highestSetBit) {

    /* Read/write display data. Read if RW is set. */
    case IRQ_GLCD_RS:
        if (pins & _BV(IRQ_GLCD_RW)) {
            readDisplayData();
        } else {
            writeDisplayData((uint8_t)pins);
        }
        break;

    /* Status read. The only valid command if busy.
     * Busy 0 On/Off Reset 0 0 0 0 */
    case IRQ_GLCD_RW:
        readStatus();
        break;

    /* Set page: 1 0 1 1 1 A A A
     * or
     * Display start line: 1 1 A A A A A */
    case IRQ_GLCD_D7:
        if (pins & _BV(IRQ_GLCD_D6)) {
            setStartLine(pins & __extension__ 0b00111111);
        } else {
            setPage(pins & __extension__ 0b00000111);
        }
        break;

    /* Set address: 0 1 A A A A A A */
    case IRQ_GLCD_D6:
        setAddress(pins & __extension__ 0b00111111);
        break;

    /* Display on/off: 0 0 1 1 1 1 1 On/Off */
    case IRQ_GLCD_D5:
        if ((pins & __extension__ 0b00111110) != __extension__ 0b00111110) {
            qDebug("GLCD: Invalid pin state 0x%04x, ignoring.", pins);
        } else {
            displayOnOff(pins & __extension__ 0b00000001);
        }
        break;

    default:
        qDebug("GLCD: Invalid pin state 0x%04x, ignoring.", pins);
        return;
    }
}

avr_cycle_count_t NT7108::transmitHook(struct avr_t *, avr_cycle_count_t, void *param)
{
    NT7108 *p = (NT7108 *)param;
    emit p->transmit(p->buffer);
    return 0;
}

void NT7108::readDisplayData()
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, output);

    /* Needs a dummy read. The read command simultaneously moves the requested contents
     * to a buffer (which corresponds to latching into the output register) and moves the
     * previous contents of the buffer onto the pins. */

    buffer = output;
    output = ram[incrementAddress()];

    avr_cycle_timer_register(avr, cycleDataDelay, transmitHook, this);
}

void NT7108::writeDisplayData(uint8_t data)
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, data);
    uint8_t aptr = incrementAddress();
    ram[aptr] = data;
    emit pageChanged(QPoint(aptr % NT7108_WIDTH, aptr / NT7108_WIDTH), data);
}

void NT7108::readStatus()
{
    DEBUG("GLCD: %s", __PRETTY_FUNCTION__);

    /* TODO */
    uint8_t busy = 0 << 7;
    uint8_t onoff = 0 << 5;
    uint8_t reset = 0 << 4;

    buffer = (busy | onoff | reset);
    avr_cycle_timer_register(avr, cycleDataDelay, transmitHook, this);
}

void NT7108::setStartLine(uint8_t line)
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, line);
    zaddr = line;
}

void NT7108::setPage(uint8_t page)
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, page);
    xaddr = page;
}

void NT7108::setAddress(uint8_t address)
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, address);
    yaddr = address;
}

void NT7108::displayOnOff(uint8_t on)
{
    DEBUG("GLCD: %s (0x%02x)", __PRETTY_FUNCTION__, on);
    this->on = (on == 1);
}

uint8_t NT7108::incrementAddress()
{
    /* TODO: find out how this really increments. */
    uint8_t addr = xaddr * NT7108_WIDTH + yaddr;
    yaddr = (yaddr + 1) % 64;
    return addr;
}

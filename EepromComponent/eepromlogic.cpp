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



#include "eepromlogic.h"

#include <avr_twi.h>
#include <stdio.h>

#define EEPROM_SIZE (1024 / 8)  /**< 1 Kbit. */
#define EEPROM_ADDR (0xa0)      /**< 1010xxxx. */
#define EEPROM_MASK (0xf0)      /**< 11110000. */
#define ADDRESS_MASK (0x7F)     /**< 0111 1111 (127). */

EepromLogic::EepromLogic() : twi(this), addressPointer(0)
{
    eeprom.fill(0xff, EEPROM_SIZE);
}

void EepromLogic::wireHook(avr_t *avr)
{
    twi.wireHook(avr);

    avr_vcd_init(avr, "eeprom.vcd", &vcdFile, 10000);
    avr_vcd_add_signal(&vcdFile,
        avr_io_getirq(avr, AVR_IOCTL_TWI_GETIRQ(0), TWI_IRQ_STATUS), 8 /* bits */ ,
        "TWSR" );

    if (vcdEnabled) {
        avr_vcd_start(&vcdFile);
    }

    connected = true;
}

void EepromLogic::unwireHook()
{
    if (!connected) {
        return;
    }

    twi.unwireHook();
    avr_vcd_close(&vcdFile);

    connected = false;
}

uint8_t EepromLogic::incrementAddress()
{
    uint8_t p = addressPointer;
    addressPointer = (addressPointer + 1) & ADDRESS_MASK;
    return p;
}

uint8_t EepromLogic::transmitByte()
{
    fprintf(stderr, "EEPROM: Read byte 0x%02x at 0x%02x\n",
            (uint8_t)eeprom[addressPointer], addressPointer);
    return eeprom[incrementAddress()];
}

bool EepromLogic::matchesAddress(uint8_t address)
{
    return ((address & EEPROM_MASK) == (EEPROM_ADDR & EEPROM_MASK) || /* Self. */
            (address & 0xfe) == 0x00);
}

void EepromLogic::received(const QByteArray &data)
{
    /* TODO: limit writes to page. */

    fprintf(stderr, "EEPROM: Set address 0x%02x\n", data[0]);
    addressPointer = data[0];

    /* Only last 8 bytes of data are read. */
    int start = data.size() - 8;
    if (start < 1) {
        start = 1;
    }

    for (int i = start; i < data.size(); i++) {
        fprintf(stderr, "EEPROM: Write byte 0x%02x at 0x%02x\n",
                (uint8_t)data[i], addressPointer);
        eeprom[incrementAddress()] = data[i];
    }
}

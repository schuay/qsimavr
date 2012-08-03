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



#ifndef NT7108_H
#define NT7108_H

#include <QObject>
#include <sim_avr.h>

enum {
    IRQ_GLCD_CS1 = 0,   /**< Low active. */
    IRQ_GLCD_CS2,       /**< Low active. */
    IRQ_GLCD_E,

    IRQ_GLCD_D0,
    IRQ_GLCD_D1,
    IRQ_GLCD_D2,
    IRQ_GLCD_D3,
    IRQ_GLCD_D4,
    IRQ_GLCD_D5,
    IRQ_GLCD_D6,
    IRQ_GLCD_D7,

    IRQ_GLCD_RW,
    IRQ_GLCD_RS,
    IRQ_GLCD_RST,       /**< Low active. */

    IRQ_GLCD_COUNT
};

class NT7108
{
public:
    NT7108();

    void connect(avr_t *avr);
    void disconnect();

    /**
     * Called when the GLCD chip has received a falling E edge.
     * Timing checks are done, and all we need to do is execute the command.
     * Pins is a bitfield of the current pin state.
     */
    void processCommand(uint16_t pins);

private:
    avr_t *avr;
};

#endif // NT7108_H

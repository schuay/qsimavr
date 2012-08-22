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
#include <QPoint>
#include <sim_avr.h>

#define NT7108_HEIGHT (64)
#define NT7108_WIDTH (64)
#define NT7108_PX_PER_PAGE (8)

enum {
    IRQ_GLCD_D0 = 0,
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
    IRQ_GLCD_E,

    IRQ_GLCD_CS1,       /**< Low active. */
    IRQ_GLCD_CS2,       /**< Low active. */

    IRQ_GLCD_COUNT
};

class NT7108 : public QObject
{
    Q_OBJECT
public:
    NT7108();

    void connect(avr_t *avr);
    void disconnect();

    /**
     * Called when the GLCD chip has received an E edge.
     * Timing checks are done, and all we need to do is execute the command.
     * Pins is a bitfield of the current pin state.
     */
    void processCommand(uint16_t pins);

signals:
    void transmit(uint8_t data);

    /**
     * Signalled whenever data is written to the display ram.
     * The x coordinate represents the horizontal position (pixel-wise, 0-64)
     * and the y coordinate the vertical position (page-wise, 0-7).
     */
    void pageChanged(QPoint coords, uint8_t value);

private:
    void readDisplayData();
    void writeDisplayData(uint8_t data);
    void readStatus();
    void setStartLine(uint8_t line);
    void setPage(uint8_t page);
    void setAddress(uint8_t address);
    void displayOnOff(uint8_t on);

    /** Returns an index into ram, and increments xaddr and yaddr appropriately. */
    int incrementAddress();

    /** Called by the simavr cycle timer system once the 320 ns data delay have expired. */
    static avr_cycle_count_t transmitHook(struct avr_t *, avr_cycle_count_t, void *param);

private:
    avr_t *avr;
    avr_cycle_count_t cycleDataDelay;   /**< How many cycles to delay between E rising edge and data write. */

    bool on;
    QByteArray ram;

    uint16_t pinstate;   /**< Buffers the command pins at the rising edge and data pins at falling edge. */

    uint8_t output; /**< Output register, see display read. */
    uint8_t buffer; /**< Transmit buffer. Used to buffer output for transmitHook. */

    uint8_t yaddr;  /**< Horizontal address, 0-64. */
    uint8_t xaddr;  /**< Vertical address, 0-7. */
    uint8_t zaddr;  /**< The display data RAM displayed at top of screen. */
};

#endif // NT7108_H

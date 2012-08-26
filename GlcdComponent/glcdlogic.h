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



#ifndef GLCDLOGIC_H
#define GLCDLOGIC_H

#include <component.h>
#include <QObject>

#include "nt7108.h"
#include "touchscreen.h"

class GlcdLogic : public ComponentLogic
{
    Q_OBJECT

public:
    GlcdLogic();

protected:
    void wireHook(avr_t *avr);
    void unwireHook();

public slots:
    void pressed(QPoint coord);
    void released();

signals:
    void pageChanged(QPoint coords, uint8_t value);

private slots:
    void transmit(uint8_t data);
    void chipPageChanged(QPoint coords, uint8_t value);

private:
    uint16_t reconstructPins();

    void pinChanged(struct avr_irq_t * irq, uint32_t value);
    static void pinChangedHook(struct avr_irq_t *irq, uint32_t value, void *param);

private:
    avr_t *avr;
    avr_irq_t *irq;

    bool pinESet;

    /**
     * The GLCD contains two identical controller chips;
     * chip1 handles the left side of the display, chip2 the right.
     */
    NT7108 chip1;
    NT7108 chip2;

    Touchscreen touchscreen;

    /**
     * Timings.
     * The read/write timings given in the datasheet probably apply
     * to each chip separately, but without more information on how exactly this works,
     * let's be pessimistic and track them globally.
     *
     * We are interested in E high and low level width (450 ns + 25 ns rise time).
     */
    avr_cycle_count_t lastReset, lastEChange;
    avr_cycle_count_t cyclesELowHigh;

    /**
     * If we are already in an IRQ handler, do not handle it a second time.
     * This can only occur by pin changes triggered from chip1 or chip2, which
     * we are not interested in.
     */
    bool reentrant;
};

#endif // GLCDLOGIC_H

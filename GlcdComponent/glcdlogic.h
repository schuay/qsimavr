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

#include "nt7108.h"

class GlcdLogic : public ComponentLogic
{
public:
    GlcdLogic();

    void connect(avr_t *avr);
    void disconnect();

private:
    void pinChanged(struct avr_irq_t * irq, uint32_t value);
    static void pinChangedHook(struct avr_irq_t *irq, uint32_t value, void *param);

private:
    avr_t *avr;
    avr_irq_t *irq;

    /**
     * The GLCD contains two identical controller chips;
     * chip1 handles the left side of the display, chip2 the right.
     */
    NT7108 chip1;
    NT7108 chip2;
};

#endif // GLCDLOGIC_H

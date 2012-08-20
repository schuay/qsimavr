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



#ifndef LCDLOGIC_H
#define LCDLOGIC_H

#include <component.h>
#include <QObject>

#include "hd44780.h"

class LcdLogic : public ComponentLogic
{
    Q_OBJECT

public:
    explicit LcdLogic(QObject *parent = 0);

    void connect(avr_t *avr);
    void disconnect();

signals:
    void textChanged(QString line1, QString line2);
    /* TODO: Brightness, cursor, shift, ... */

private:
    static void displayChanged(void *instance, const hd44780_t *vram);

    avr_t *avr;
    struct hd44780_t hd44780;
};

#endif // LCDLOGIC_H

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



#ifndef LEDBUTTONSLOGIC_H
#define LEDBUTTONSLOGIC_H

#include <component.h>
#include <QList>
#include <QObject>
#include <sim_avr.h>
#include <stdint.h>

class CallbackData;

class LedButtonsLogic : public ComponentLogic
{
    Q_OBJECT

public:
    explicit LedButtonsLogic(QObject *parent = 0);

    void connect(avr_t *avr);
    void disconnect();

signals:
    void ledChanged(QChar port, uint8_t pin, bool on);

public slots:
    void buttonPressed(QChar port, uint8_t pin);
    void buttonReleased(QChar port, uint8_t pin);

private:
    static void pinChanged(struct avr_irq_t * irq, uint32_t value, void * param);

    avr_t *avr;
    avr_irq_t *irq;

    QList<CallbackData *> callbackData;
};

#endif // LEDBUTTONSLOGIC_H

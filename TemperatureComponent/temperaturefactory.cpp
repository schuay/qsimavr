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



#include "temperaturefactory.h"

#include "temperaturegui.h"
#include "temperaturelogic.h"

Component TemperatureFactory::create()
{
    QSharedPointer<TemperatureGui> gui = QSharedPointer<TemperatureGui>(new TemperatureGui());
    QSharedPointer<TemperatureLogic> logic = QSharedPointer<TemperatureLogic>(new TemperatureLogic());
    Component component = { gui, logic };

    QObject::connect(logic.data(), SIGNAL(scratchpadChanged(QByteArray)),
                     gui.data(), SLOT(scratchpadChanged(QByteArray)), Qt::QueuedConnection);
    QObject::connect(logic.data(), SIGNAL(eepromChanged(QByteArray)),
                     gui.data(), SLOT(eepromChanged(QByteArray)), Qt::QueuedConnection);

    return component;
}

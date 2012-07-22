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



#include "ledbuttonfactory.h"

#include <QMetaType>

#include "ledbuttonsgui.h"
#include "ledbuttonslogic.h"

LedButtonFactory::LedButtonFactory()
{
}

Component LedButtonFactory::create()
{
    QSharedPointer<LedButtonsGui> gui = QSharedPointer<LedButtonsGui>(new LedButtonsGui());
    QSharedPointer<LedButtonsLogic> logic = QSharedPointer<LedButtonsLogic>(new LedButtonsLogic());
    Component component = { gui, logic };

    qRegisterMetaType<uint8_t>("uint8_t");

    QObject::connect(gui.data(), SIGNAL(buttonPressed(QChar,uint8_t)),
                     logic.data(), SLOT(buttonPressed(QChar,uint8_t)), Qt::QueuedConnection);
    QObject::connect(gui.data(), SIGNAL(buttonReleased(QChar,uint8_t)),
                     logic.data(), SLOT(buttonReleased(QChar,uint8_t)), Qt::QueuedConnection);
    QObject::connect(logic.data(), SIGNAL(ledChanged(QChar,uint8_t,bool)),
                     gui.data(), SLOT(ledChanged(QChar,uint8_t,bool)), Qt::QueuedConnection);

    return component;
}

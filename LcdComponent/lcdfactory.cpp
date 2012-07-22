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



#include "lcdfactory.h"

#include "lcdgui.h"
#include "lcdlogic.h"

LcdFactory::LcdFactory()
{
}

Component LcdFactory::create()
{
    QSharedPointer<LcdGui> gui = QSharedPointer<LcdGui>(new LcdGui());
    QSharedPointer<LcdLogic> logic = QSharedPointer<LcdLogic>(new LcdLogic());
    Component component = { gui, logic };

    QObject::connect(logic.data(), SIGNAL(textChanged(QString,QString)),
                     gui.data(), SLOT(textChanged(QString,QString)));

    return component;
}

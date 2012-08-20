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



#include "glcdfactory.h"

#include <QMetaType>

#include "glcdlogic.h"
#include "glcdgui.h"

GlcdFactory::GlcdFactory()
{
}

Component GlcdFactory::create()
{
    QSharedPointer<GlcdGui> gui = QSharedPointer<GlcdGui>(new GlcdGui());
    QSharedPointer<GlcdLogic> logic = QSharedPointer<GlcdLogic>(new GlcdLogic());
    Component component = { gui, logic };

    qRegisterMetaType<uint8_t>("uint8_t");

    QObject::connect(logic.data(), SIGNAL(pageChanged(QPoint, uint8_t)),
                     gui.data(), SLOT(pageChanged(QPoint, uint8_t)), Qt::QueuedConnection);

    return component;
}

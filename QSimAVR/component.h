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



#ifndef COMPONENT_H
#define COMPONENT_H

#include <QWidget>
#include <QSharedPointer>
#include <sim_avr.h>

template<typename Interface>
class Factory {
public:
    virtual Interface create() = 0;
    virtual ~Factory() { }
};

class ComponentGui
{
public:
    virtual void show() = 0;
    virtual QWidget *widget() = 0;
    virtual ~ComponentGui() { }
};

class ComponentLogic
{
public:
    virtual void connect(avr_t *avr) = 0;
    virtual void disconnect() = 0;
    virtual ~ComponentLogic() { }
};

struct Component
{
    QSharedPointer<ComponentGui> gui;
    QSharedPointer<ComponentLogic> logic;
};

typedef Factory<Component> ComponentFactory;

typedef ComponentFactory *(*RegisterFunction)();

#define PUBLISH_FNAME "registerPlugin"
#define PUBLISH_PLUGIN(factory) \
    extern "C" { \
        ComponentFactory *registerPlugin() { return new factory; } \
    }

#endif // COMPONENT_H

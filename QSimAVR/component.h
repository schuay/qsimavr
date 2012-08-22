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

/**
 * The factory is responsible for constructing all component parts
 * and connecting them.
 */
template<typename Interface>
class Factory {
public:
    Factory() { }
    virtual Interface create() = 0;
    virtual ~Factory() { }

private:
    Q_DISABLE_COPY(Factory)
};

/**
 * The GUI part of components. It handles both display of the component
 * state and a way of configuring the component. It usually communicates
 * with the logic part using queued signals (since they live in different
 * threads).
 */
class ComponentGui
{
public:
    ComponentGui() { }

    virtual QWidget *widget() = 0;
    virtual ~ComponentGui() { }

private:
    Q_DISABLE_COPY(ComponentGui)
};

/**
 * The logic part of components. It lives in the SimAVR thread.
 */
class ComponentLogic : public QObject
{
public:
    ComponentLogic() : connected(false) { }
    ComponentLogic(QObject *parent) : QObject(parent), connected(false) { }

    virtual void wire(avr_t *avr) = 0;
    virtual void unwire() = 0;
    virtual ~ComponentLogic() { }

protected:
    bool connected;

private:
    Q_DISABLE_COPY(ComponentLogic)
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

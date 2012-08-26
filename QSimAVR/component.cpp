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




#include "component.h"

ComponentLogic::ComponentLogic(QObject *parent) :
    QObject(parent),
    connected(false),
    vcdEnabled(false)
{
    memset(&io, 0, sizeof(io));
    io.instance = this;
    io.io.kind = "component";
    io.io.reset = ComponentLogic::resetHookPrivate;
}

void ComponentLogic::wire(avr_t *avr)
{
    avr_register_io(avr, &io.io);

    wireHook(avr);

    if (vcdEnabled) {
        avr_vcd_start(&vcdFile);
    }

    connected = true;
}

void ComponentLogic::unwire()
{
    if (!connected) {
        return;
    }

    unwireHook();

    connected = false;
}

void ComponentLogic::resetHookPrivate(avr_io_t *io)
{
    component_io_t *p = (component_io_t *)io;
    p->instance->resetHook();
}

void ComponentLogic::enableVcd(bool vcdEnabled)
{
    if (this->vcdEnabled == vcdEnabled) {
        return;
    }

    this->vcdEnabled = vcdEnabled;
    if (connected) {
        if (vcdEnabled) {
            avr_vcd_start(&vcdFile);
        } else {
            avr_vcd_stop(&vcdFile);
        }
    }
}

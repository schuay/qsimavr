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



#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QList>
#include <QMdiArea>

#include "component.h"
#include "simavr.h"

class PluginManager
{
public:
    PluginManager();

    /**
     * Loads all plugins from PLUGINDIR.
     */
    void load();

    void connectGui(QMdiArea *mdiArea);
    void connectSim(QSharedPointer<SimAVR> sim);

private:
    /**
     * Attempts to load a plugin from filename.
     */
    void load(const QString &filename);

private:
    QList<Component> plugins;
};

#endif // PLUGINMANAGER_H

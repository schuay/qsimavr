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



#include "pluginmanager.h"

#include <QDir>
#include <QLibrary>

#include "QsLog.h"

#define PLUGINDIR ("plugins")

PluginManager::PluginManager()
{
}

void PluginManager::load()
{
    /* Load all plugins in the startup folder for now.
     * For the general concept idea, see
     * http://blog.nuclex-games.com/tutorials/cxx/plugin-architecture/
     */

    QLOG_TRACE() << "Searching for available plugins";
    QDir dir(PLUGINDIR);
    foreach (const QString &filename, dir.entryList(QDir::Files)) {
        load(dir.absoluteFilePath(filename));
    }
}

void PluginManager::load(const QString &filename)
{
    QLibrary lib(filename);

    QLOG_TRACE() << "Attempting to load" << lib.fileName();

    RegisterFunction registerPlugin = (RegisterFunction)lib.resolve(PUBLISH_FNAME);
    if (!registerPlugin) {
        QLOG_WARN() << lib.errorString();
        return;
    }

    QSharedPointer<ComponentFactory> factory(registerPlugin());
    plugins.append(factory->create());

    QLOG_TRACE() << "Loaded plugin from" << lib.fileName();
}

void PluginManager::connectGui(QMdiArea *mdiArea)
{
    foreach(const Component &plugin, plugins) {
        if (plugin.gui.isNull()) {
            continue;
        }

        QWidget *widget = plugin.gui->widget();
        mdiArea->addSubWindow(widget);
        widget->show();
    }
}

void PluginManager::connectSim(QSharedPointer<SimAVR> sim)
{
    foreach(const Component &plugin, plugins) {
        sim->registerPlugin(plugin.logic);
    }
}

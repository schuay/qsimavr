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
#include <QMdiSubWindow>

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
    ComponentListEntry entry = { lib.fileName(), factory->create(), true, true };
    plugins.append(entry);

    QLOG_TRACE() << "Loaded plugin from" << lib.fileName();
}

void PluginManager::connectGui(QMdiArea *mdiArea)
{
    foreach(const ComponentListEntry &plugin, plugins) {
        if (plugin.component.gui.isNull()) {
            continue;
        }

        QWidget *widget = plugin.component.gui->widget();
        QMdiSubWindow *w = mdiArea->addSubWindow(
                    widget, Qt::CustomizeWindowHint | Qt::WindowTitleHint);

        /* Disable both the sizable window borders and the default system menu
         * (which still contains a 'Size' entry). */
        w->setFixedSize(w->sizeHint());
        w->setSystemMenu(NULL);;

        widget->show();
    }
}

void PluginManager::connectSim(avr_t *avr)
{
    foreach(const ComponentListEntry &plugin, plugins) {
        plugin.component.logic->connect(avr);
    }
}

void PluginManager::disconnectSim(avr_t *)
{
    foreach(const ComponentListEntry &plugin, plugins) {
        plugin.component.logic->disconnect();
    }
}

int PluginManager::count() const
{
   return plugins.count() ;
}

QString PluginManager::name(int index) const
{
    return plugins[index].name;
}

bool PluginManager::enabled(int index) const
{
    return plugins[index].enabled;
}

bool PluginManager::vcd(int index) const
{
    return plugins[index].vcd;
}

void PluginManager::setEnabled(int index, bool enabled)
{
    plugins[index].enabled = enabled;
}

void PluginManager::setVcd(int index, bool vcd)
{
    plugins[index].vcd = vcd;
}

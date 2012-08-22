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

class PluginManager : public QObject
{
    Q_OBJECT

public:
    PluginManager();
    ~PluginManager();

    /**
     * Loads all plugins from PLUGINDIR.
     */
    void load(QThread *t, QMdiArea *mdiArea);

    int count() const;
    QString name(int index) const;
    bool enabled(int index) const;
    bool vcd(int index) const;

    void setEnabled(int index, bool enabled);
    void setVcd(int index, bool vcd);


public slots:
    void connectSim(avr_t *avr);
    void disconnectSim(avr_t *avr);

private:
    /**
     * Attempts to load a plugin from filename.
     */
    void load(QThread *t, QMdiArea *mdiArea, const QString &filename);

private:

    struct ComponentListEntry
    {
        QString name;
        Component component;
        QMdiSubWindow *window;
        bool enabled;
        bool vcd;
    };

    QList<ComponentListEntry> plugins;

    /** Keep the current avr instance around so we can reconnect,
     *  plugins at any time. */
    avr_t *avr;
};

#endif // PLUGINMANAGER_H

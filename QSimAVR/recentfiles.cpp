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



#include "recentfiles.h"

#include <QSettings>

#define RECENT_FILES_PREFIX "RecentFiles"
#define RECENT_FILES_ENTRY "Filename"

RecentFiles::RecentFiles(QList<QAction *> actions)
{
    this->actions = actions;

    /* Initialize all actions to invisible. */
    foreach (QAction *action, actions) {
        action->setVisible(false);
    }

    /* Load values. */

    QSettings settings;

    const int size = settings.beginReadArray(RECENT_FILES_PREFIX);
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);

        QString recent = settings.value(RECENT_FILES_ENTRY, QString()).toString();
        if (recent.isNull()) {
            return;
        }

        files.append(recent);
    }
    settings.endArray();

    refreshActions();
}

void RecentFiles::refreshActions()
{
    for (int i = 0; i < actions.size(); i++) {
        if (i >= files.size()) {
            actions[i]->setVisible(false);
            continue;
        }

        actions[i]->setVisible(true);
        actions[i]->setData(files[i]);
        actions[i]->setText(files[i]);
    }
}

void RecentFiles::addRecentFile(const QString &filename)
{
    files.removeAll(filename);

    if (files.size() == actions.size()) {
        files.removeLast();
    }

    files.push_front(filename);
    refreshActions();

    /* Load values. */

    QSettings settings;

    settings.beginWriteArray(RECENT_FILES_PREFIX);
    for (int i = 0; i < files.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue(RECENT_FILES_ENTRY, files[i]);
    }
    settings.endArray();
    settings.sync();
}

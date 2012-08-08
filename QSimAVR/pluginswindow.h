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



#ifndef PLUGINSWINDOW_H
#define PLUGINSWINDOW_H

#include <QDialog>
#include <QScopedPointer>

#include "pluginmanager.h"
#include "plugintablemodel.h"

namespace Ui {
class PluginsWindow;
}

class PluginsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit PluginsWindow(QWidget *parent, PluginManager *manager);
    ~PluginsWindow();
    
private:
    Ui::PluginsWindow *ui;

    QScopedPointer<PluginTableModel> model;
};

#endif // PLUGINSWINDOW_H

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



#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QSharedPointer>

#include "pluginmanager.h"
#include "recentfiles.h"
#include "simavr.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
signals:
    void stopSimulation();

private slots:
    /**
     * Attempts to load a firmware file chosen by the user into the SimAVR instance.
     */
    void loadFirmware();

    void openRecentFile();
    void configurePlugins();

    void simulationStateChanged(SimulationState state);
    void restartSimulation();

protected:
    void closeEvent(QCloseEvent *event);

private: /* Methods. */
    void setupToolbar();
    void setupMenu();
    void setupStatusbar();
    void createSim();
    void terminateSim();
    void loadFirmware(const QString &filename);

private: /* Variables. */
    Ui::MainWindow *ui;

    PluginManager pluginManager;
    QSharedPointer<SimAVR> sim;
    QSharedPointer<RecentFiles> recent;

    QAction *pauseAction;
    QAction *restartAction;
    QAction *attachAction;

    QLabel *cycleLabel;
    QLabel *stateLabel;
};

#endif // MAINWINDOW_H

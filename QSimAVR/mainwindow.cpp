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



#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <assert.h>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QMdiSubWindow>
#include <QSettings>

#include "pluginswindow.h"
#include "QsLog.h"

#define KEY_GEOMETRY ("Geometry")

/* TODO
 * Separate logic and ui(?)
 * Add settings for mmcu, frequency.
 * Add plugin management (enable/disable plugins at runtime).
 * Add standard execution control: (re)start/pause/step.
 * Add status feedback (loaded firmware, simulation state and stats).
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QCoreApplication::setApplicationName("QSimAVR");
    QCoreApplication::setOrganizationName("QSimAVR");

    setupToolbar();
    setupMenu();
    setupStatusbar();

    createSim();

    pluginManager.load(sim.data(), ui->mdiArea);

    QSettings settings;
    restoreGeometry(settings.value(KEY_GEOMETRY).toByteArray());
}

void MainWindow::setupToolbar()
{
    QAction *action = new QAction("Load", this);
    connect(action, SIGNAL(triggered()), this, SLOT(loadFirmware()));
    ui->toolBar->addAction(action);

    pauseAction = new QAction("Start/Stop", this);
    ui->toolBar->addAction(pauseAction);

    restartAction = new QAction("Restart", this);
    ui->toolBar->addAction(restartAction);

    attachAction = new QAction("Toggle GDB", this);
    ui->toolBar->addAction(attachAction);
}

void MainWindow::setupStatusbar()
{
    stateLabel = new QLabel(this);
    ui->statusBar->addPermanentWidget(stateLabel, 1);
}

void MainWindow::setupMenu()
{
    connect(ui->actionLoad_Firmware, SIGNAL(triggered()), this, SLOT(loadFirmware()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure_Plugins, SIGNAL(triggered()), this, SLOT(configurePlugins()));

    /* Setup recent file actions. */
    QList<QAction *> actions;
    actions << ui->actionRecent0 << ui->actionRecent1
            << ui->actionRecent2 << ui->actionRecent3 << ui->actionRecent4;

    foreach (QAction *action, actions) {
        connect(action, SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    recent = QSharedPointer<RecentFiles>(new RecentFiles(actions));
}

void MainWindow::createSim()
{
    sim = QSharedPointer<SimAVR>(new SimAVR());

    qRegisterMetaType<SimulationState>("SimulationState");

    connect(pauseAction, SIGNAL(triggered()),
            sim.data(), SLOT(pauseSimulation()), Qt::QueuedConnection);
    connect(restartAction, SIGNAL(triggered()),
            this, SLOT(restartSimulation()));
    connect(attachAction, SIGNAL(triggered()),
            sim.data(), SLOT(attachGdb()), Qt::QueuedConnection);

    connect(this, SIGNAL(stopSimulation()),
            sim.data(), SLOT(stopSimulation()));

    connect(sim.data(), SIGNAL(simulationStateChanged(SimulationState)),
            this, SLOT(simulationStateChanged(SimulationState)), Qt::QueuedConnection);

    connect(sim.data(), SIGNAL(firmwareLoaded(avr_t*)),
            &pluginManager, SLOT(connectSim(avr_t*)), Qt::DirectConnection);
    connect(sim.data(), SIGNAL(firmwareUnloading(avr_t*)),
            &pluginManager, SLOT(disconnectSim(avr_t*)), Qt::DirectConnection);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(KEY_GEOMETRY, saveGeometry());
    QMainWindow::closeEvent(event);
}

void MainWindow::loadFirmware()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Firmware");
    loadFirmware(filename);
}

void MainWindow::loadFirmware(const QString &filename)
{
    if (filename.isNull()) {
        return;
    }

    terminateSim();

    sim->load(filename);
    recent->addRecentFile(filename);

    setWindowTitle(QString("QSimAVR - %1").arg(filename));
    QLOG_TRACE() << "Loaded firmware from" << filename;

    sim->start();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        loadFirmware(action->data().toString());
    }
}

void MainWindow::configurePlugins()
{
    QScopedPointer<PluginsWindow> w(new PluginsWindow(this, &pluginManager));
    w->exec();
}

void MainWindow::terminateSim()
{
    if (!sim->isRunning()) {
        return;
    }

    emit stopSimulation();
    if (sim->wait(10000)) {
        return;
    }

    sim->terminate();
    sim->wait();
}

void MainWindow::simulationStateChanged(SimulationState state)
{
    QString stateName;
    switch (state) {
    case Running: stateName = "Running"; break;
    case Paused: stateName = "Paused"; break;
    case Done: stateName = "Done"; break;
    default: assert(0);
    }

    stateLabel->setText(QString("Simulation %1.").arg(stateName));
}

void MainWindow::restartSimulation()
{
    terminateSim();
    sim->start();
}

MainWindow::~MainWindow()
{
    terminateSim();
    delete ui;
}

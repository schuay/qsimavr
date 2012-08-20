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



#ifndef SIMAVR_H
#define SIMAVR_H

#include <QThread>
#include <sim_avr.h>
#include <sim_elf.h>
#include <sim_irq.h>

enum SimulationState {
    Running,
    Paused,
    Done
};

class SimAVR : public QThread
{
    Q_OBJECT

public:
    SimAVR();
    virtual ~SimAVR();

    void load(const QString &filename);
    void run();

signals:
    void simulationStateChanged(SimulationState state);

    /* The following firmware signals are used by the plugin manager
     * to load/unload plugins. */
    void firmwareUnloading(avr_t *avr);
    void firmwareLoaded(avr_t *avr);

public slots:
    void pauseSimulation();
    void stopSimulation();
    void attachGdb();

private:
    void attachGdbInternal();

private:
    avr_t *avr;
    elf_firmware_t firmware;

    bool gdb;
};

#endif // SIMAVR_H

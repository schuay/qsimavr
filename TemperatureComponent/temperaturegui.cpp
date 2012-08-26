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



#include "temperaturegui.h"
#include "ui_temperaturegui.h"

TemperatureGui::TemperatureGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TemperatureGui)
{
    ui->setupUi(this);

    scratchpadEdit = new QHexEdit(this);
    scratchpadEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scratchpadEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->vlScratchpad->addWidget(scratchpadEdit);

    eepromEdit = new QHexEdit(this);
    eepromEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    eepromEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->vlEeprom->addWidget(eepromEdit);
}

TemperatureGui::~TemperatureGui()
{
    delete ui;
}

void TemperatureGui::scratchpadChanged(QByteArray data)
{
    scratchpadEdit->setData(data);
}

void TemperatureGui::eepromChanged(QByteArray data)
{
    eepromEdit->setData(data);
}

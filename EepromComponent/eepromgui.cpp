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



#include "eepromgui.h"
#include "ui_eepromgui.h"

EepromGui::EepromGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EepromGui)
{
    ui->setupUi(this);

    hexEdit = new QHexEdit(this);
    hexEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    hexEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->verticalLayout->addWidget(hexEdit);

    connect(hexEdit, SIGNAL(dataChanged()), this, SLOT(dataChangedInternal()));
}

EepromGui::~EepromGui()
{
    delete ui;
}

void EepromGui::onDataChange(QByteArray data)
{
    hexEdit->setData(data);
}

void EepromGui::dataChangedInternal()
{
    emit dataChanged(hexEdit->data());
}

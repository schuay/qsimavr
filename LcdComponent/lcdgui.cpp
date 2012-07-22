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



#include "lcdgui.h"
#include "ui_lcdgui.h"

/* TODO
 * Switch to QGraphicsView display.
 * Add cursor simulation.
 * Add display shift emulation.
 * Silence verbose hd44780 output (or redirect it).
 */

LcdGui::LcdGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LcdGui)
{
    ui->setupUi(this);
}

LcdGui::~LcdGui()
{
    delete ui;
}

void LcdGui::show()
{
    QWidget::show();
}

void LcdGui::textChanged(QString line1, QString line2)
{
    ui->textBrowser->setText(QString("%1\n%2").arg(line1, line2));
}

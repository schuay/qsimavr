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



#ifndef LCDGUI_H
#define LCDGUI_H

#include <component.h>
#include <QWidget>

namespace Ui {
class LcdGui;
}

class LcdGui : public QWidget, public ComponentGui
{
    Q_OBJECT
    
public:
    explicit LcdGui(QWidget *parent = 0);
    ~LcdGui();

    void show();
    QWidget *widget() { return this; }


public slots:
    void textChanged(QString line1, QString line2);
    
private:
    Ui::LcdGui *ui;
};

#endif // LCDGUI_H

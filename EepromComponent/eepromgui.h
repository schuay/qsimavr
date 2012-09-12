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



#ifndef EEPROMGUI_H
#define EEPROMGUI_H

#include <component.h>
#include <qhexedit.h>
#include <QWidget>

namespace Ui {
class EepromGui;
}

class EepromGui : public QWidget, public ComponentGui
{
    Q_OBJECT
    
public:
    explicit EepromGui(QWidget *parent = 0);
    ~EepromGui();

    QWidget *widget() { return this; }

signals:
    void dataChanged(QByteArray data);

public slots:
    void onDataChange(QByteArray data);

private slots:
    void dataChangedInternal();
    
private:
    Ui::EepromGui *ui;

    QHexEdit *hexEdit;
};

#endif // EEPROMGUI_H

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



#ifndef LEDBUTTONSGUI_H
#define LEDBUTTONSGUI_H

#include <QMap>
#include <QPushButton>
#include <QWidget>

#include <component.h>

namespace Ui {
class LedButtonsGui;
}

struct ButtonData;

class LedButtonsGui : public QWidget, public ComponentGui
{
    Q_OBJECT

public:
    explicit LedButtonsGui(QWidget *parent = 0);
    ~LedButtonsGui();

    void show();
    QWidget *widget() { return this; }

signals:
    void buttonPressed(QChar port, uint8_t pin);
    void buttonReleased(QChar port, uint8_t pin);

public slots:
    void ledChanged(QChar port, uint8_t pin, bool on);

private slots:
    void buttonPressed();
    void buttonReleased();

private:
    Ui::LedButtonsGui *ui;

    QVector<QPushButton *> buttons;
    QMap<QPushButton *, ButtonData *> buttonData;
};

#endif // LEDBUTTONSGUI_H

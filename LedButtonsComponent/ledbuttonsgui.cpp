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



#include "ledbuttonsgui.h"
#include "ui_ledbuttonsgui.h"

struct ButtonData {
    QChar port;
    int pin;
};

#define SETUP_BUTTON(_port, _pin) { \
    QPushButton *button = ui->port ## _port ## _pin; \
    \
    connect(button, SIGNAL(pressed()), this, SLOT(buttonPressed())); \
    connect(button, SIGNAL(released()), this, SLOT(buttonReleased())); \
    \
    ButtonData *d = new ButtonData; \
    d->port = QString( # _port ).at(0); \
    d->pin = _pin; \
    \
    buttons << button; \
    buttonData[button] = d; \
    }

LedButtonsGui::LedButtonsGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LedButtonsGui)
{
    ui->setupUi(this);

    SETUP_BUTTON(A, 0); SETUP_BUTTON(A, 1); SETUP_BUTTON(A, 2); SETUP_BUTTON(A, 3);
    SETUP_BUTTON(A, 4); SETUP_BUTTON(A, 5); SETUP_BUTTON(A, 6); SETUP_BUTTON(A, 7);

    SETUP_BUTTON(B, 0); SETUP_BUTTON(B, 1); SETUP_BUTTON(B, 2); SETUP_BUTTON(B, 3);
    SETUP_BUTTON(B, 4); SETUP_BUTTON(B, 5); SETUP_BUTTON(B, 6); SETUP_BUTTON(B, 7);

    SETUP_BUTTON(C, 0); SETUP_BUTTON(C, 1); SETUP_BUTTON(C, 2); SETUP_BUTTON(C, 3);
    SETUP_BUTTON(C, 4); SETUP_BUTTON(C, 5); SETUP_BUTTON(C, 6); SETUP_BUTTON(C, 7);

    SETUP_BUTTON(D, 0); SETUP_BUTTON(D, 1); SETUP_BUTTON(D, 2); SETUP_BUTTON(D, 3);
    SETUP_BUTTON(D, 4); SETUP_BUTTON(D, 5); SETUP_BUTTON(D, 6); SETUP_BUTTON(D, 7);

    SETUP_BUTTON(E, 0); SETUP_BUTTON(E, 1); SETUP_BUTTON(E, 2); SETUP_BUTTON(E, 3);
    SETUP_BUTTON(E, 4); SETUP_BUTTON(E, 5); SETUP_BUTTON(E, 6); SETUP_BUTTON(E, 7);

    SETUP_BUTTON(F, 0); SETUP_BUTTON(F, 1); SETUP_BUTTON(F, 2); SETUP_BUTTON(F, 3);
    SETUP_BUTTON(F, 4); SETUP_BUTTON(F, 5); SETUP_BUTTON(F, 6); SETUP_BUTTON(F, 7);

    SETUP_BUTTON(G, 0); SETUP_BUTTON(G, 1); SETUP_BUTTON(G, 2); SETUP_BUTTON(G, 3);
    SETUP_BUTTON(G, 4); SETUP_BUTTON(G, 5); SETUP_BUTTON(G, 6); SETUP_BUTTON(G, 7);

    SETUP_BUTTON(H, 0); SETUP_BUTTON(H, 1); SETUP_BUTTON(H, 2); SETUP_BUTTON(H, 3);
    SETUP_BUTTON(H, 4); SETUP_BUTTON(H, 5); SETUP_BUTTON(H, 6); SETUP_BUTTON(H, 7);

    SETUP_BUTTON(J, 0); SETUP_BUTTON(J, 1); SETUP_BUTTON(J, 2); SETUP_BUTTON(J, 3);
    SETUP_BUTTON(J, 4); SETUP_BUTTON(J, 5); SETUP_BUTTON(J, 6); SETUP_BUTTON(J, 7);

    SETUP_BUTTON(K, 0); SETUP_BUTTON(K, 1); SETUP_BUTTON(K, 2); SETUP_BUTTON(K, 3);
    SETUP_BUTTON(K, 4); SETUP_BUTTON(K, 5); SETUP_BUTTON(K, 6); SETUP_BUTTON(K, 7);

    SETUP_BUTTON(L, 0); SETUP_BUTTON(L, 1); SETUP_BUTTON(L, 2); SETUP_BUTTON(L, 3);
    SETUP_BUTTON(L, 4); SETUP_BUTTON(L, 5); SETUP_BUTTON(L, 6); SETUP_BUTTON(L, 7);
}

LedButtonsGui::~LedButtonsGui()
{
    delete ui;
}

void LedButtonsGui::show()
{
    QWidget::show();
}

void LedButtonsGui::buttonPressed()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    ButtonData *d = buttonData[b];
    emit buttonPressed(d->port, d->pin);
}

void LedButtonsGui::buttonReleased()
{
    QPushButton *b = qobject_cast<QPushButton *>(sender());
    ButtonData *d = buttonData[b];
    emit buttonReleased(d->port, d->pin);
}

#define SETUP_SLOT(_port, _portChar, _pin) \
    void LedButtonsGui::port ## _port ## _pin ## Pressed() { emit buttonPressed(_portChar, _pin); } \
    void LedButtonsGui::port ## _port ## _pin ## Released() { emit buttonReleased(_portChar, _pin); } \

void LedButtonsGui::ledChanged(QChar port, uint8_t pin, bool on)
{
    QPushButton *b = buttons[(port.toAscii() - 'A') * 8 + pin];
    b->setStyleSheet(on ? "background-color: rgb(255, 0, 4);" : "");
}

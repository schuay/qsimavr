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



#ifndef GLCDGUI_H
#define GLCDGUI_H

#include <component.h>
#include <QWidget>

#include "glcdgraphicsscene.h"

namespace Ui {
class GlcdGui;
}

class GlcdGui : public QWidget, public ComponentGui
{
    Q_OBJECT
    
public:
    explicit GlcdGui(QWidget *parent = 0);
    ~GlcdGui();

    QWidget *widget();

public slots:
    void pageChanged(QPoint coord, uint8_t value);
    
private:
    GlcdGraphicsScene scene;

    Ui::GlcdGui *ui;
};

#endif // GLCDGUI_H

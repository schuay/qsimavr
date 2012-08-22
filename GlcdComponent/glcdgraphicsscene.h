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



#ifndef GLCDGRAPHICSSCENE_H
#define GLCDGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QVector>
#include <stdint.h>

class GlcdGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GlcdGraphicsScene(QObject *parent);

    void setPage(const QPoint &coord, uint8_t value);

signals:
    void pressed(QPoint coord);
    void released();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QVector<QGraphicsRectItem *> pixels;
};

#endif // GLCDGRAPHICSSCENE_H

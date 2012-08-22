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



#include "glcdgraphicsscene.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

#include "nt7108.h"

#define WIDTH (128)
#define HEIGHT (64)
#define SCALE (2)

#define X(index) ((index) % WIDTH)
#define Y(index) ((index) / WIDTH)
#define INDEX(x, y) ((y) * WIDTH + (x))

#define BRUSH_OFF (QBrush(Qt::green))
#define BRUSH_ON  (QBrush(Qt::black))

GlcdGraphicsScene::GlcdGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
    setSceneRect(0, 0, WIDTH * SCALE, HEIGHT * SCALE);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        pixels.append(addRect(X(i) * SCALE,
                              Y(i) * SCALE,
                              SCALE,
                              SCALE,
                              QPen(Qt::NoPen),
                              QBrush(Qt::green)));
    }
}

void GlcdGraphicsScene::setPage(const QPoint &coord, uint8_t value)
{
    for (int i = 0; i < NT7108_PX_PER_PAGE; i++) {
        bool on = ((value & (1 << i)) != 0);
        pixels[INDEX(coord.x(), coord.y() * NT7108_PX_PER_PAGE + i)]->setBrush(on ? BRUSH_ON : BRUSH_OFF);
    }
}

void GlcdGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint coord(event->scenePos().toPoint() / SCALE);
    emit pressed(coord);
    event->accept();
}

void GlcdGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint coord(event->scenePos().toPoint() / SCALE);
    emit pressed(coord);
    event->accept();
}

void GlcdGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit released();
    event->accept();
}

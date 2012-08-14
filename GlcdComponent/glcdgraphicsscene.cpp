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

#define WIDTH (128)
#define HEIGHT (64)
#define SCALE (2)

#define X(index) (index % WIDTH)
#define Y(index) (index / WIDTH)

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

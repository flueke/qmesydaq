/***************************************************************************
 *   Copyright (C) 2013-2020 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef LINE_H
#define LINE_H

#include <QGraphicsPathItem>
#include <graphicsscene.h>
#include <QRectF>
#include <QPointF>
#include <QPainterPath>

class Item;

class Line : public QGraphicsPathItem
{
public:
    enum {Type = UserType+1};
    int type() const
        { return Type;}
    Line(Item* startingItem, Item* endingItem, QGraphicsItem* parent = 0);
    Line(Line* line);
    Item* startingItem() const
        { return startItem;}
    QPointF getStartPoint()
        {return startPoint;}
    Item* endingItem() const
        { return endItem;}
    QPointF getEndPoint()
        {return endPoint;}
    int getVerticalBreak()
        {return verticalBreak;}
    virtual void updatePosition();
    void setVerticalBreak(int y);
    void updateFromTo();
    QPainterPath shape() const;
//    QRectF boundingRect() const;

protected:
    QPointF startPoint;
    QPointF endPoint;
    Item* endItem;
    Item* startItem;
    int verticalBreak;
    void updateBreakPoint();
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = 0);

private:
};

#endif // LINE_H

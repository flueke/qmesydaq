/***************************************************************************
 *   Copyright (C) 2013-2019 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include "templine.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <QDebug>

TempLine::TempLine(Item *startingItem, Item *endingItem, QGraphicsPathItem *parent)
    :Line(startingItem,endingItem,parent)
{
    firstItem = startingItem;
    setZValue(10000.0);
}

void TempLine::updateFromTo(Item *item, bool rank)
{
    if (rank) // the item is the new start item
    {
        endItem = firstItem;
        startItem = item;
        endItem->removeLine(this);
        startItem->addLine(this);
    }
    else
        endItem = item;

    Line::updateFromTo();

    if (!startItem->getConnectedLines().isEmpty())
        startItem->updateLines();
    if (!endItem->getConnectedLines().isEmpty())
        endItem->updateLines();
}

void TempLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    setPen(QPen(Qt::red,2,Qt::DashLine,Qt::FlatCap,Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing,false);

    QGraphicsPathItem::paint(painter, option, widget);
}
void TempLine::updatePosition(QPointF mPos)
{
    endPoint = mPos;

    if (endItem != 0)
    {
        startItem->removeLine(this);
        Item *tempItem = startItem;
        startItem = firstItem;
        startItem->addLine(this);
        startPoint = QPointF(startItem->x()+50, startItem->y()+50);
        tempItem->updateLines();
        endItem=0;
    }

    updatePosition();
}

void TempLine::updatePosition()
{
    int oldBreak = verticalBreak;

    updateBreakPoint();

    QPainterPath path(startPoint);
    path.lineTo(QPointF(startPoint.x(), verticalBreak));
    path.lineTo(QPointF(endPoint.x(), verticalBreak));
    path.lineTo(endPoint);

    setPath(path);

    if(oldBreak != verticalBreak)
        startingItem()->updateLines();
}

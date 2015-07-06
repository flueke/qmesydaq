/***************************************************************************
 *   Copyright (C) 2013-2015 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include <QLineF>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QVector>
#include <qmath.h>

#include <QDebug>

#include "line.h"
#include "item.h"


Line::Line(Item* startingItem, Item* endingItem, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
{
    startItem = startingItem;
    startPoint = QPointF(startItem->x()+50, startItem->y()+50);
    startItem->addLine(this);
    endItem = endingItem;
    setFlags(ItemIsSelectable);
    setZValue(-1000.0);

    if (endingItem != 0)
    {
        endItem->addLine(this);
        startItem->addConnections(endItem);
        endItem->changeConnection();
        endItem->setHigherTierItem(startItem);
        endItem->setHigherTierLine(this);
        updatePosition();
    }
}

Line::Line(Line* line)
    :QGraphicsPathItem()
{
    startItem = line->startItem;
    startPoint = QPointF(startItem->x()+50, startItem->y()+50);
    startItem->addLine(this);
    endItem = line->endItem;
    endItem->addLine(this);
    startItem->addConnections(endItem);
    endItem->changeConnection();
    endItem->setHigherTierItem(startItem);
    endItem->setHigherTierLine(this);
    updatePosition();
    setFlags(ItemIsSelectable);
    setZValue(-1000.0);
}

void Line::updateBreakPoint()
{


    int y = startPoint.y()+80;

    foreach(Line* line, startItem->getConnectedLines())
    {
        if(line->startingItem() == startItem
                && line->getEndPoint().y()-80<y)
        {
            y = line->getEndPoint().y()-80;
        }
    }

    if (y != verticalBreak)
        foreach(Line* line, startItem->getConnectedLines())
        {
            if (line->startingItem() == startItem
                    && line->getVerticalBreak() != y)
            {
                line->setVerticalBreak(y);
            }
        }
}

void Line::updatePosition()
{
    if(endItem != 0 && startItem->isVisible() && endItem->isVisible())
        show();
    else
        hide();

    int oldBreak = verticalBreak;

    updateFromTo();
    updateBreakPoint();

    QPainterPath path(startPoint);
    path.lineTo(QPointF(startPoint.x(), verticalBreak));
    path.lineTo(QPointF(endPoint.x(), verticalBreak));
    path.lineTo(endPoint);

    setPath(path);

    if (oldBreak != verticalBreak)
        startItem->updateLines();
}

void Line::setVerticalBreak(int y)
{
    verticalBreak = y;
}


void Line::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                 QWidget*)
{
    QPen pen(Qt::black,2,Qt::SolidLine,Qt::FlatCap,Qt::RoundJoin);
    if (option->state & QStyle::State_Selected)
    {
        pen.setStyle(Qt::DashLine);
    }
    painter->setPen(pen);

    painter->setRenderHint(QPainter::Antialiasing,false);

    painter->drawPath(path());
}

void Line::updateFromTo()
{
    startPoint = QPointF(startItem->x()+50, startItem->y()+50);
    endPoint = QPointF(endItem->x()+50, endItem->y()+50);
}

QPainterPath Line::shape() const
{
    QVector<QPointF> points;
    QPointF offset1(1,0);
    QPointF offset2(-1,1);
    points << startPoint + offset1
           << startPoint - offset1
           << QPointF(startPoint.x(), verticalBreak) + offset2
           << QPointF(endPoint.x(),verticalBreak) + offset2
           << endPoint - offset1
           << endPoint + offset1
           << QPointF(endPoint.x(),verticalBreak) - offset2
           << QPointF(startPoint.x(),verticalBreak) - offset2
           << startPoint + offset1;

    QPolygonF lineShape(points);
    QPainterPath path;
    path.addPolygon(lineShape);

    return path;
}




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

#ifndef TEMPLINE_H
#define TEMPLINE_H

#include "line.h"

class Item;

class TempLine : public Line
{
public:
    enum {Type = UserType +2};
    int type() const
        { return Type;}
    TempLine(Item *startingItem, Item *endingItem =NULL,
             QGraphicsPathItem *parent =NULL);

    void updateFromTo(Item *item, bool rank);
    void updatePosition(QPointF mPos);
    void updatePosition();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    Item* getFirstItem()
        { return firstItem;}

private:
    Item *firstItem;
};

#endif // TEMPLINE_H

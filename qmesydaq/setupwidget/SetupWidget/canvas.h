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

#ifndef CANVAS_H
#define CANVAS_H

#include <graphicsscene.h>
#include <templine.h>
#include <QList>

class QGraphicsItem;
class MCPD_8;
class MPSD_8;
class Channel;
class PC;
class AddTreeDialog;

class Canvas : public GraphicsScene
{
    Q_OBJECT

public:
    Canvas(QObject *parent=NULL);
    void adjustPositions(QList<Item *> itemList, int highestTier);

signals:
    void center(QGraphicsItem *);

public slots:
    void sortItems();
    void addMultipleItems();
    void copy();
    void paste();

private slots:
    PC *createPC();
    Channel *createChannel();
    MCPD_8 *createMCPD();
    MPSD_8 *createMPSD();

    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    int lowestY;
    int offset;
    int numberOfChannels;
    int offsetStep;
    QList <Item* > clipBoardItemList;
    QMap <int, int> clipboardLineMap;


};

#endif // CANVAS_H

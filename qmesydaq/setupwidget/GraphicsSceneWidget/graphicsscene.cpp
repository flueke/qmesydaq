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

#include "graphicsscene.h"
#include "templine.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QLineF>
#include <QList>
#include <qmath.h>
#include <QApplication>
#include <QInputDialog>
#include <QPlatformCursor>

#include <QDebug>
#include <QGraphicsView>
#include <QBitmap>

GraphicsScene::GraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    tempLine = 0;
    setMode(dragMode);
}

void GraphicsScene::setImagePath(const QString &path)
{
    imagePath = path;
    setMode(itemMode);
}

Item* GraphicsScene::createItem()
{
    Item *myItem = new Item(nextItemType, imagePath, "");
    return myItem;
}

void GraphicsScene::setMode(Mode mode)
{
    QApplication::restoreOverrideCursor();
    if (mode == lineMode)
    {
        QPixmap pixmap(35,30);
        QPixmap *pic = &pixmap;
        QPolygon polygon;
        polygon << QPoint(0,0) << QPoint(9,0) << QPoint(0,9);
        pic->fill();
        pic->setMask(pic->createHeuristicMask());
        QPainter painter(pic);

        painter.drawPixmap(5,0,QPixmap(":/base/line").scaled
                                        (30,30,Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));

        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawPolygon(polygon);

        painter.end();

        QApplication::setOverrideCursor(QCursor(pixmap,0,0));
    }
    else if(mode == itemMode)
    {
        QPixmap pixmap(102,102);
        QPixmap *pic = &pixmap;
        QPolygon polygon;
        polygon << QPoint(0,0) << QPoint(10,0) << QPoint(0,10);
        pic->fill();
        pic->setMask(pic->createHeuristicMask());
        QPen pen(Qt::red);
        pen.setWidth(2);
        QPainter painter(pic);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing);

        QRectF rect(1,1,100,100);

        painter.drawRoundedRect(rect, 100*6/rect.width(), 100*6/rect.height());

        painter.drawPixmap(QPoint(rect.width()/2-31,2),QPixmap(imagePath), rect);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawPolygon(polygon);

        painter.end();

        QApplication::setOverrideCursor(QCursor(pixmap,0,0));
    }
    myMode = mode;
    clearSelection();
}

void GraphicsScene::enableLineMode()
{
    setMode(lineMode);
}

void GraphicsScene::enableItemMode(int type, QString path)
{
    nextItemType = type;
    imagePath = path;
    setMode(itemMode);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::RightButton)
    {
        if (myMode == dragMode)
        {
            setMode(lineMode);
        }
        else
        {
            setMode(dragMode);
        }
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
        if (myMode == itemMode)
        {
            Item *myItem = createItem();
            addItem(myItem);
            myItem->setPos(mouseEvent->scenePos());
            emit sceneModified();
            setMode(dragMode);
        }
        else if (myMode == lineMode)
        {
            if (Item *item = dynamic_cast <Item *> (itemAt(mouseEvent->scenePos())))
            {
                if(!item->isFullyConnected())
                {
                    tempLine = new TempLine(item);
                    tempLine->updatePosition(mouseEvent->scenePos());
                    addItem(tempLine);
                }
            }
        }
        else if (myMode == dragMode)
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else return;
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (myMode == lineMode && tempLine != 0)
    {
        Item *item(firstItemUnderCursor(mouseEvent->scenePos()));
        if (item == 0)
        {
                tempLine->updatePosition(mouseEvent->scenePos());
        }
        else  if (item->getType() == tempLine->getFirstItem()->getType()+1
                  && !item->isConnected()) //unconnected lower Tier item
        {
            tempLine->updateFromTo(item,false);
        }
        else if (item->getType() == tempLine->getFirstItem()->getType()-1) //higher tier Items
        {
            if(tempLine->endingItem() == 0
                    && !item->isFullyConnected()
                    && !tempLine->getFirstItem()->isConnected()) //item has less then max subitems
            {
                tempLine->updateFromTo(item,true);
            }
            else if (item != tempLine->startingItem()) //item is "full" and not connected to line
            {
                tempLine->updatePosition(mouseEvent->scenePos());
            }
        }
        else
        {
            tempLine->updatePosition(mouseEvent->scenePos());
        }
    }
    else if (myMode == dragMode)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (tempLine != 0 && myMode == lineMode)
    {
        tempLine->startingItem()->removeLine(tempLine);

        if (tempLine->endingItem()!=0)
        {
            Item *startItem = tempLine->startingItem();
            Item *endItem = tempLine->endingItem();

            Line *line = new Line(startItem, endItem);
            addItem(line);

            emit sceneModified();
            startItem->update(startItem->getBoundingRect().width()-16,0,16,16);
        }
        else
        {
            tempLine->startingItem()->updateLines();
        }

        removeItem(tempLine);
        tempLine=0;
    }
    if (myMode == dragMode)
        QApplication::setOverrideCursor(Qt::ArrowCursor);

    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

Item* GraphicsScene::firstItemUnderCursor(QPointF position)
{
    QList<QGraphicsItem *> items = this->items(position);
    Item *item = NULL;

    if (!items.isEmpty())
        foreach (QGraphicsItem *qItem, items)
            if(qItem->type() != Line::Type
                    && qItem != tempLine
                    && qItem != tempLine->getFirstItem())
            {
                item = dynamic_cast <Item *> (qItem);
                break;
            }

    return item;
}


void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton && myMode == dragMode)
    {
        QList<QGraphicsItem *> ItemsAtPosition = items(mouseEvent->scenePos());
        if (ItemsAtPosition.count() > 0 &&
                ItemsAtPosition.first()->type() == Item::Type)
        {
            Item *myItem =
                    dynamic_cast <Item *> (ItemsAtPosition.first());
            QRect rect =myItem->mapRectToScene(myItem->getBoundingRect().width()-16,0,16,16).toRect();
            QPoint click = mouseEvent->scenePos().toPoint();

            if (rect.contains(click))
                mouseEvent->ignore();
            else
                emit itemDoubleClicked(myItem);
        }
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GraphicsScene::setType(int Type)
{
    nextItemType = Type;
}


void GraphicsScene::sortItems()
{
    emit itemExpansionChanged();
    emit sceneModified();
}






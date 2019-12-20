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

#include "item.h"
#include "line.h"
#include "templine.h"
#include "graphicsscene.h"

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QPointF>


#include <QDebug>


Item::Item(int &type, QString path, QString toolTip, QGraphicsItem *parent)
    : QGraphicsPixmapItem(path, parent)
{
    connectedToHigher = 0;
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    myInfo = toolTip;
    myType = type;
    myID = itemCount++;
    myName = QString("%1").arg(myID);
    treeExpansionRect = new QRect(boundingRect().width()-16,0,16,16);
    subTreeVisibility = true;
    fullyConnected = false;
    maxNumberOfSubitems = 8;
    picPath = path;
}

Item::Item(Item *item)
    : QGraphicsPixmapItem(item->picPath)
{
    connectedToHigher = 0;
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    myInfo = item->myInfo;
    myType = item->myType;
    myID = itemCount++;
    myName = item->myName;
    treeExpansionRect = item->treeExpansionRect;
    subTreeVisibility = item->subTreeVisibility;
    fullyConnected = false;
    maxNumberOfSubitems = item->maxNumberOfSubitems;
    picPath = item->picPath;
    setVisible(item->isVisible());

}

QRectF Item::boundingRect() const
{
    qreal penWidth = 2;
    return QRectF(0, 0, 100-penWidth/2,100-penWidth/2);
}

void Item::addLine(Line *line)
{
    myLines.append(line);
    if(line->type() != TempLine::Type
            && myLines.size()==maxNumberOfSubitems)
        fullyConnected = true;
}

void Item::addConnections(Item *item)
{
    if (myLowerTierItems.isEmpty())
        myLowerTierItems.append(item);
    else
    {
        int id = item->getID();
        int i;

        for (i=0; i<myLowerTierItems.size(); i++)
        {
            if (id < myLowerTierItems.at(i)->getID())
                break;
        }
        myLowerTierItems.insert(i, item);
    }
}

bool Item::isConnectedto(Item *item)
{
    return myLowerTierItems.contains(item);
}


QVariant Item::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        foreach (Line *line, myLines)
        {
            line->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void Item::removeConnectedItem(Item *item)
{
    int index = myLowerTierItems.indexOf(item);

    if (index != -1)
        myLowerTierItems.removeAt(index);
}

void Item::removeLine(Line *line)
{
    int index = myLines.indexOf(line);

    if (index != -1)
        myLines.removeAt(index);
    if (myLowerTierItems.size()<maxNumberOfSubitems)
        fullyConnected = false;

    updateLines();
    update(boundingRect().width()-16,0,16,16);
}

void Item::removeLines()
{
    foreach (Line *line, myLines)
    {
        line->startingItem()->removeLine(line);
        line->endingItem()->removeLine(line);
        this->scene()->removeItem(line);
        delete line;
    }
}

int Item::roundness(double size) const
{
    const int Diameter = 6;
    return 100*Diameter/int(size);
}

void Item::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QPen pen(Qt::black);
    if(option->state & QStyle::State_Selected)
    {
        pen.setStyle(Qt::DashDotLine);
    }
    pen.setWidth(2);
    painter->setBrush(Qt::white);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect = boundingRect();

    painter->drawRoundedRect(rect, roundness(rect.width()), roundness(rect.height()));
    painter->drawPixmap(QPoint(rect.width()/2-32,1),pixmap(), boundingRect());

    if (!myLowerTierItems.isEmpty())
    {
        painter->setPen(QPen(Qt::black,2,Qt::SolidLine,Qt::FlatCap,Qt::RoundJoin));

        QPointF upperRightCorner[3] = {
            QPointF(rect.width()-16,0),
            QPointF(rect.width()-16,16),
            QPointF(rect.width(),16)};
        painter->drawPolyline(upperRightCorner,3);
        painter->drawLine(rect.width()-14,8,rect.width()-2,8);

        if (!myLowerTierItems.first()->isVisible())
            painter->drawLine(rect.width()-8,2,rect.width()-8,14);
    }

    QString text = myName;
    QString text1;
    QString text2;
    if(text.length()>9)
    {
        if (text.contains(" "))
        {
            text1 = text.left(text.indexOf(" "));
            text2 = text.right(text.length()-text.indexOf(" "));
        }
        else
        {
            text1 = text.left(text.length()/2);
            text2 = text.right(text.length()-text.length()/2);
        }
        int textOffsetX = (boundingRect().width()/2
                           -painter->fontMetrics().width(text2)/2);
        int textOffsetY = boundingRect().height()-5;

        painter->drawText(textOffsetX,textOffsetY,text2);

        textOffsetX = (boundingRect().width()/2
                       -painter->fontMetrics().width(text1)/2);
        textOffsetY = (boundingRect().height()-1
                       -painter->fontMetrics().height());

        painter->drawText(textOffsetX,textOffsetY,text1);
    }
    else
    {
        int textOffsetX = boundingRect().width()/2-painter->fontMetrics().width(text)/2;
        int textOffsetY = boundingRect().height()-3-painter->fontMetrics().height()/2;

        painter->drawText(textOffsetX,textOffsetY,text);
    }
}

void Item::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPoint click = event->pos().toPoint();

    setAcceptDrops(true);

    GraphicsScene *scene = dynamic_cast <GraphicsScene *> (this->scene());

    if(event->button() == Qt::LeftButton
            && treeExpansionRect->contains(click)
            && !myLowerTierItems.isEmpty()
            && scene->getMode() == 0)
    {
        if(myLowerTierItems.first()->isVisible())
            hideTree();
        else
            showTree();

        changeSubTreeVisibility();

        scene->sortItems();

        update(boundingRect().width()-16,0,16,16);
    }
    else
        QGraphicsItem::mouseReleaseEvent(event);
}

void Item::hideTree()
{
    foreach(Item *item, myLowerTierItems)
    {
        if(item->getLowerTierItemList().isEmpty())
            item->hide();
        else
        {
            item->hideTree();
            item->hide();
        }
        item->updateLines();
    }
}

void Item::showTree()
{
    foreach(Item *item, myLowerTierItems)
    {
        if (item->getLowerTierItemList().isEmpty() || !item->subTreeIsVisible())
            item->show();
        else
        {
            item->showTree();
            item->show();
        }
        item->updateLines();
    }

}


void Item::updateLines()
{
    foreach(Line *line, myLines)
    {
        if (line->type() == Line::Type)
        {

            line->updatePosition();
        }
        else
        {
            TempLine *tempLine = qgraphicsitem_cast <TempLine *> (line);
            tempLine-> updatePosition();
        }

    }
}

void Item::changeConnection()
{
    connectedToHigher = !connectedToHigher;
}

void Item::changeSubTreeVisibility()
{
    subTreeVisibility = !subTreeVisibility;
}

QPainterPath Item::shape() const
{
    QRectF rect = boundingRect();

    QPainterPath path;
    path.addRoundedRect(rect, roundness(rect.width()), roundness(rect.height()));

    return path;
}

int Item::itemCount = 0;

void Item::changeToolTip()
{
    QString toolTip(QString("Name:\t%1\nID:\t%2\nInfo:\n%3")
                    .arg(myName).arg(myID).arg(myInfo));
    setToolTip(toolTip);
}

void Item::setInfo(QString info)
{
    myInfo = info;
}

void Item::setItemCount(int value)
{
    itemCount = value;
}

void Item::setID(int ID)
{
    myID = ID;
}

int Item::lowerTierListSize()
{
    return myLowerTierItems.size();
}

int Item::getWidth()
{
    return boundingRect().width();
}

int Item::getHeight()
{
    return boundingRect().height();
}

void Item::setHigherTierItem(Item *item)
{
    myHigherTierItem = item;
}

void Item::setHigherTierLine(Line* line)
{
    myHigherTierLine = line;
}

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

#include "canvas.h"
#include "item.h"
#include "pc.h"
#include "mcpd_8.h"
#include "mpsd_8.h"
#include "channel.h"
#include "addtreedialog.h"
#include "setupview.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QPointF>
#include <QApplication>
#include <QClipboard>
#include <QMapIterator>

#include <QDebug>

Canvas::Canvas(QObject* parent)
    : GraphicsScene(parent)
{
    offsetStep = 15;
    clipBoardItemList.clear();
    clipboardLineMap.clear();
}

void Canvas::addMultipleItems()
{
    QApplication::restoreOverrideCursor();

    AddTreeDialog addTreeDialog;

    QList <PC*> addedPCs;
    QList <MCPD_8*> addedMCPDs;
    QList <MPSD_8*> addedMPSDs;
    QList <Channel*> addedChannels;

    if (addTreeDialog.exec())
    {
        foreach(QGraphicsItem*  item,this->selectedItems())
            item->setSelected(false);

        int type = addTreeDialog.getTopLevelType();
        int amount = addTreeDialog.getAmount();

        while (type != 5)
        {
            nextItemType = type;

            for(int i = amount; i > 0; i--)
            {
                switch (type)
                {
                case 4:
                {
                    imagePath = ":/icons/channel";
                    Channel* myChannel = createChannel();
                    addItem(myChannel);
                    addedChannels.append(myChannel);
                    break;
                }
                case 3:
                {
                    imagePath = ":/icons/mpsd";
                    MPSD_8* myMPSD = createMPSD();
                    addItem(myMPSD);
                    addedMPSDs.append(myMPSD);
                    break;
                }
                case 2:
                {
                    imagePath = ":/icons/mcpd";
                    MCPD_8* myMCPD = createMCPD();
                    addItem(myMCPD);
                    addedMCPDs.append(myMCPD);
                    break;
                }
                case 1:
                {
                    imagePath = ":/icons/pc";
                    PC* myPC = createPC();
                    addItem(myPC);
                    addedPCs.append(myPC);
                    break;
                }
                }
            }

            switch(type)
            {
            case 1: amount *= addTreeDialog.getSubMCPDAmount();
                break;
            case 2: amount *= addTreeDialog.getSubMPSDAmount();
                break;
            case 3: amount *= addTreeDialog.getSubChannelAmount();
                break;
            }


            if(addTreeDialog.addFullTree())
                type += 1;
            else
                type=5;

        }
        if(!addedMPSDs.isEmpty())
            foreach(Channel* channel, addedChannels)
            {
                int mpsdIndex = addedChannels.indexOf(channel)/addTreeDialog.getSubChannelAmount();
                Line* myLine = new Line(addedMPSDs.at(mpsdIndex),channel);
                addItem(myLine);
            }
        if (!addedMCPDs.isEmpty())
            foreach(MPSD_8* mpsd, addedMPSDs)
            {
                int mcpdIndex = addedMPSDs.indexOf(mpsd)/addTreeDialog.getSubMPSDAmount();
                Line* myLine = new Line (addedMCPDs.at(mcpdIndex),mpsd);
                addItem(myLine);
            }
        if (!addedPCs.isEmpty())
            foreach(MCPD_8* mcpd, addedMCPDs)
            {
                int pcIndex = addedMCPDs.indexOf(mcpd)/addTreeDialog.getSubMCPDAmount();
                Line* myLine = new Line (addedPCs.at(pcIndex),mcpd);
                addItem(myLine);
            }

        if(!addedPCs.isEmpty())
            addedPCs.first()->setSelected(true);
        else if(!addedMCPDs.isEmpty())
            addedMCPDs.first()->setSelected(true);
        else if(!addedMPSDs.isEmpty())
            addedMPSDs.first()->setSelected(true);
        else if(!addedChannels.isEmpty())
            addedChannels.first()->setSelected(true);

        sortItems();
    }
}

PC* Canvas::createPC()
{
    PC* myPC = new PC(nextItemType, imagePath, "");
    return myPC;
}

Channel* Canvas::createChannel()
{
    Channel* myChannel = new Channel(nextItemType, imagePath, "");
    return myChannel;
}

MCPD_8* Canvas::createMCPD()
{
    MCPD_8* myMCPD = new MCPD_8("192.168.168",54321,nextItemType, imagePath, "");
    return myMCPD;
}

MPSD_8* Canvas::createMPSD()
{
    MPSD_8* myMPSD = new MPSD_8(nextItemType, imagePath, "");
    return myMPSD;
}

void Canvas::sortItems()
{
    offset = 0;
    numberOfChannels = 0;
    int highestTier = 4;

    QList <Item* > itemList;
    foreach(QGraphicsItem* qItem, items())
    {
        if(Item* item = dynamic_cast<Item*> (qItem))
        {
            if (!item->isConnected())
                itemList.prepend(item);
            if (item->getType() < highestTier)
                highestTier = item->getType();
            if (item->type()==Channel::Type)
                ++numberOfChannels;
        }
    }
    if (itemList.isEmpty())
        return;
    else
        adjustPositions(itemList,highestTier);


    setSceneRect(QRect(0,0,offset,lowestY));

    if(SetupView *myView = dynamic_cast <SetupView *>(views().first()))
    {
        if(myView->isZoomable())
        {
            myView->fitInView(sceneRect(),Qt::KeepAspectRatio);
            myView->setZoomfactor(myView->transform().m11());
        }

        if (selectedItems().isEmpty())
            emit center(itemList.first());
        else
            emit center(selectedItems().first());

    }


    emit sceneModified();
}

void Canvas::adjustPositions(QList<Item* > itemList, int highestTier)
{
    lowestY = 0;

    bool onlyChannelSubItems = false;

    if (numberOfChannels > 50)
    {
        onlyChannelSubItems = true;
        foreach(Item *item, itemList)
        {
            if(item->getType() != 4
                    || item->getHigherTierItem() != itemList.first()->getHigherTierItem())
            {
                onlyChannelSubItems = false;
            }
        }
    }

    foreach(Item* item, itemList)
    {
        int y = 50 + (item->getType()-highestTier)*item->getHeight() \
                + (item->getType()-highestTier)*70;


        if (item == itemList.first())
            offset += offsetStep;

        if (onlyChannelSubItems)
        {
            y += itemList.indexOf(item)*(item->getHeight()+10)-35;
            item->setPos(offset, y);



            if (item == itemList.last())
            {
                offset += offsetStep + item->getWidth();

                if (item->y()+item->getHeight() > lowestY)
                    lowestY = item->y()+item->getHeight()+50;
            }
        }
        else if (item->getLowerTierItemList().isEmpty()
                || !item->getLowerTierItemList().first()->isVisible())
        {
            offset += offsetStep*(4-item->getType());
            item->setPos(offset,y);

            if (item->y()+item->getHeight() > lowestY)
                lowestY = item->y()+item->getHeight()+50;

            offset += offsetStep +item->getWidth();
            if (offset > this->width())
                this->setSceneRect(0,0,offset,this->height());
        }
        else
        {
            QList <Item* > subItems = item->getLowerTierItemList();

            adjustPositions(subItems, highestTier);

            int x = subItems.first()->x() \
                    + (subItems.last()->x()-subItems.first()->x())/2;

            item->setPos(x,y);
        }
    }
}

void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
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
            switch(nextItemType)
            {
            case 1:
            {
                PC* myPC = createPC();
                addItem(myPC);
                myPC->setPos(mouseEvent->scenePos());
                break;
            }
            case 2:
            {
                MCPD_8* myMCPD = createMCPD();
                addItem(myMCPD);
                myMCPD->setPos(mouseEvent->scenePos());
                break;
            }
            case 3:
            {
                MPSD_8* myMPSD = createMPSD();
                addItem(myMPSD);
                myMPSD->setPos(mouseEvent->scenePos());
                break;
            }
            case 4:
            {
                Channel* myChannel = createChannel();
                addItem(myChannel);
                myChannel->setPos(mouseEvent->scenePos());
                break;
            }
            }

            emit sceneModified();
            setMode(dragMode);
        }
        else if (myMode == lineMode)
        {
            if (Item* item = dynamic_cast <Item* > (itemAt(mouseEvent->scenePos())))
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
            views().first()->setDragMode(QGraphicsView::ScrollHandDrag);
            QApplication::setOverrideCursor(Qt::ClosedHandCursor);
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else return;
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    if (myMode == dragMode)
    {
        views().first()->setDragMode(QGraphicsView::NoDrag);
        QApplication::restoreOverrideCursor();
    }
    GraphicsScene::mouseReleaseEvent(mouseEvent);
}

void Canvas::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton && myMode == dragMode)
    {
        QList<QGraphicsItem* > ItemsAtPosition = items(mouseEvent->scenePos());
        if (ItemsAtPosition.count() > 0)
            if(Item* myItem = dynamic_cast <Item* > (ItemsAtPosition.first()))
            {
                QRect rect =myItem->mapRectToScene(myItem->getBoundingRect() \
                                                   .width()-16,0,16,16).toRect();
                QPoint click = mouseEvent->scenePos().toPoint();

                if (rect.contains(click))
                    mouseEvent->ignore();
                else
                    emit itemDoubleClicked(myItem);
            }
    }
    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void Canvas::copy()
{
    clipBoardItemList.clear();
    clipboardLineMap.clear();

    QList <QGraphicsItem* > list = this->selectedItems();

    foreach(QGraphicsItem* qItem, list)
    {
        if(Item* item = dynamic_cast <Item* >(qItem))
        {
            clipBoardItemList.append(item);
        }
    }

    for(int i = 0; i<clipBoardItemList.size(); i++)
    {
        Item* item = clipBoardItemList.at(i);
        if((!item->getLowerTierItemList().isEmpty() \
                && !item->getLowerTierItemList().first()->isVisible()) \
                || !item->isVisible())
        {
            clipBoardItemList.append(item->getLowerTierItemList());
        }
        if(clipBoardItemList.contains(item->getHigherTierItem()))
        {
            clipboardLineMap.insert(clipBoardItemList.indexOf(item), \
                                    clipBoardItemList.indexOf(item->getHigherTierItem()));
        }
    }

}


void Canvas::paste()
{
    QList <Item *> insertList;
    if(!clipBoardItemList.isEmpty())
    {
        QPointF first = clipBoardItemList.first()->scenePos();

        foreach(Item* item, clipBoardItemList)
        {
            QPointF mouse = views().first() \
                    ->mapToScene(views().first() \
                                 ->mapFromGlobal(QCursor::pos())); // get scene coordinates of Cursor

            QPointF pos =  mouse + (item->pos() - first); //adjust Cursor position relative to item positions

            if(item->type()==Channel::Type)
            {
                Channel* channel = qgraphicsitem_cast <Channel* > (item);
                Channel* myChannel = new Channel(channel);
                myChannel->setPos(pos);
                addItem(myChannel);
                insertList.append(myChannel);
            }
            else if(item->type() == MPSD_8::Type)
            {
                MPSD_8* mpsd = qgraphicsitem_cast <MPSD_8* > (item);
                MPSD_8* myMpsd = new MPSD_8(mpsd);
                myMpsd->setPos(pos);
                addItem(myMpsd);
                insertList.append(myMpsd);
            }
            else if(item->type() == MCPD_8::Type)
            {
                MCPD_8* mcpd = qgraphicsitem_cast <MCPD_8* > (item);
                MCPD_8* myMcpd = new MCPD_8(mcpd);
                myMcpd->setPos(pos);
                addItem(myMcpd);
                insertList.append(myMcpd);
            }
            else if(item->type() == PC::Type)
            {
                PC* pc = qgraphicsitem_cast <PC* > (item);
                PC* myPc = new PC(pc);
                myPc->setPos(pos);
                addItem(myPc);
                insertList.append(myPc);
            }
        }

        QMapIterator<int, int> iterator(clipboardLineMap);
        while (iterator.hasNext())
        {
            iterator.next();
            int key = iterator.key();
            Line* myLine = new Line (insertList.at(clipboardLineMap.value(key)),
                                     insertList.at(key));
            myLine->updatePosition();
            addItem(myLine);
        }
    }
    sortItems();
    center(insertList.first());
}

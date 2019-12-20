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

#ifndef ITEM_H
#define ITEM_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QList>

class GraphicsScene;
class QRect;
class Line;

class Item : public QGraphicsPixmapItem
{
public:
    enum {Type = UserType+3};
    int type() const
         { return Type; }
    Item(int &type, QString path, QString toolTip, QGraphicsItem* parent=NULL);
    Item(Item* item);
    void addLine(Line* line);
    virtual void addConnections(Item* item);
    bool isConnectedto(Item* item);
    void removeConnectedItem(Item* item);
    void changeConnection();
    void changeSubTreeVisibility();
    void removeLine(Line* line);
    void removeLines();
    void updateLines();
    int getWidth();
    int getHeight();
    Item* getHigherTierItem()
         {return myHigherTierItem;}
    void setHigherTierItem(Item* item);
    Line* getHigherTierLine()
         {return myHigherTierLine;}
    void setHigherTierLine(Line* line);
    int getType()
         {return myType;}
    bool isConnected()
         {return connectedToHigher;}
    bool isFullyConnected()
         {return fullyConnected;}
    bool subTreeIsVisible()
         {return subTreeVisibility;}
    static int getItemCount()
         {return itemCount;}
    QList<Item* >getLowerTierItemList()
         {return myLowerTierItems;}
    QList<Line* >getConnectedLines()
         {return myLines;}
    int getID()
         {return myID;}
    QString getName()
         {return myName;}
    QString getInfo()
    {return myInfo;}
    void changeName(QString name)
         {myName = name;}
    int lowerTierListSize();
    void setInfo(QString info);
    QPainterPath shape() const;
    virtual void changeToolTip();
    QRectF getBoundingRect()
         {return boundingRect();}
    static void setItemCount(int value); //for loading
    void setID(int ID); //for loading

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void hideTree();
    void showTree();


    QRect* treeExpansionRect;

    virtual void paint(QPainter* , const QStyleOptionGraphicsItem* , QWidget* );
    QList<Line* > myLines;
    QList<Item* > myLowerTierItems;
    Item* myHigherTierItem;
    Line* myHigherTierLine;
    int myType;
    int myID;
    int maxNumberOfSubitems;
    QRectF boundingRect() const;
    int roundness(double size) const;
    bool connectedToHigher; //connected to a higher tier Item?
    bool subTreeVisibility;
    bool fullyConnected;
    static int itemCount;
    QString myName;
    QString myInfo;

private:
    QString picPath;


};

#endif // ITEM_H

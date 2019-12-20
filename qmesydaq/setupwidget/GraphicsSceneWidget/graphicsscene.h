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

#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QSettings>

#include "item.h"
#include "line.h"

class TempLine;


class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode {dragMode, itemMode, lineMode};
    GraphicsScene(QObject *parent = 0);
    void setImagePath(const QString &);

signals:
//    void keepLineButtonStateDown();
//    void changeLineButtonState();
    void itemCreated(Item *item);
    void itemDoubleClicked(Item *item);
    void itemExpansionChanged();
    void sceneModified();

public slots:
    void setMode(Mode mode);
    Mode getMode()
    {return myMode;}
    void enableLineMode();
    void enableItemMode(int type, QString path);
    void setType(int);
    void sortItems();
//    bool itemsUnderCursor(QPointF position);
    Item *firstItemUnderCursor(QPointF position);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent);
    Item *createItem();
    Mode myMode;
    QString imagePath;
    TempLine *tempLine;
    int nextItemType;

private:

};

#endif

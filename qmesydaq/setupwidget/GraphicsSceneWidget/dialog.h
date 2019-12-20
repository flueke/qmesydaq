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

#ifndef DIALOG_H
#define DIALOG_H

#include "dialogbase.h"

class GraphicsScene;
class QGraphicsView;
class Item;
class ItemAction;

class Dialog : public DialogBase
{
    Q_OBJECT

public:
    Dialog(QWidget *parent=NULL);
    QPixmap* getPixmap()
        {return pixmap;}

public slots:
    void changePixmap(const QString &);
    void deleteItem();
    bool okToContinue();
    void enterLineMode();
    void enterItemMode(int type, QString path);


protected slots:
    void saveToFile();
    void loadFromFile();
    void sceneChanged();
    void changeItemProperties(Item *item);


protected:
    void save(const QString &fileName);
    void load(const QString &fileName);
    void accept();
    void reject();

private:
    QAction *deleteAction;
    QAction *closeAction;
    QPixmap *pixmap;
    ItemAction *item1Action;
    ItemAction *item2Action;
    ItemAction *item3Action;
    ItemAction *item4Action;
    QGraphicsView *view;
    GraphicsScene *scene;
    Button *moduleButton;
    Button *atomButton;
    Button *networkButton;
    Button *pcButton;
    void additionalButtons();
    void additionalLayout();
    void additionalConnections();
    void additionalActions();
    void additionalMenus();
    void additionalToolbars();

};

#endif // DIALOG_H

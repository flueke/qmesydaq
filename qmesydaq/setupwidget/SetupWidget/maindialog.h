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

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <dialogbase.h>

class Canvas;
class SetupView;
class QGraphicsView;
class QGraphicsItem;
class Item;
class ItemAction;
class QComboBox;



class MainDialog : public DialogBase
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent=NULL);

public slots:
    void changePixmap(const QString &);
    void deleteItem();
    bool okToContinue();

protected slots:
    void enterLineMode();
    void enterItemMode(int type, QString path);
    void saveToFile();
    void loadFromFile();
    void changeItemProperties(Item *item);
    void centerView(QGraphicsItem *item);
    void resetWidget();
    void resetCursor(int);
    void markSubTree();
    void sceneChanged();
    void setSelected(Item *item);
    void setZoomFactor(QString factor);
    void setZoomBox(double zoom);

protected:
    void save(const QString &fileName);
    void load(const QString &fileName);


private:
    QAction *deleteAction;
    QAction *resetAction;
    QAction *markAction;
    QAction *closeAction;
    QAction *sortAction;
    QAction *resetZoomAction;
    QAction *clearOnLoad;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *zoomLockAction;
    QAction *onlySubtreeAction;
    ItemAction *pcAction;
    ItemAction *mcpdAction;
    ItemAction *mpsdAction;
    ItemAction *channelAction;
    QAction *multiItemAction;
    QPixmap *pixmap;
    SetupView *view;
    Canvas *scene;
    QComboBox *zoomBox;
    void additionalConnections();
    void additionalLayout();
    void additionalMenus();
    void addittionalToolbars();
    void additionalActions();
    void accept();
    void reject();

};

#endif // MAINDIALOG_H

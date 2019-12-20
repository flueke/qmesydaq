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

#include "dialog.h"
#include "button.h"
#include "graphicsscene.h"
#include "inputdialog.h"
#include "itemaction.h"

#include <QGraphicsView>
#include <QFileDialog>
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QToolBar>



#include <QDebug>

Dialog::Dialog(QWidget *parent)
    : DialogBase(parent)
{
    pixmap = new QPixmap;
    scene = new GraphicsScene;
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setRenderHint(QPainter::HighQualityAntialiasing, true);
    scene -> setSceneRect(QRect(0,0,view->width(),view->height()));

    setWindowTitle("[*]GraphicsSceneWidget");

    additionalButtons();

    additionalConnections();
    additionalActions();
    additionalToolbars();
    additionalMenus();
    additionalLayout();
}

void Dialog::additionalButtons()
{

}

void Dialog::additionalLayout()
{
    lowerLayout->addWidget(view);

    upperLayout->addWidget(itemToolbar,1,0);

//    upperLeftLayout->addWidget(networkButton,0,0);
//    upperLeftLayout->addWidget(pcButton,0,1);
//    upperLeftLayout->addWidget(moduleButton,1,0);
//    upperLeftLayout->addWidget(atomButton,1,1);
}

void Dialog::additionalConnections()
{
//    connect(networkButton, SIGNAL(clicked(const QString)),
//            this, SLOT(changePixmap(const QString)));
//    connect(moduleButton, SIGNAL(clicked(const QString)),
//           this, SLOT(changePixmap(const QString)));
//    connect(pcButton, SIGNAL(clicked(const QString)),
//            this, SLOT(changePixmap(const QString)));
//    connect(atomButton, SIGNAL(clicked(const QString)),
//            this, SLOT(changePixmap(const QString)));

//    connect(networkButton, SIGNAL(changeType(int)),
//            scene, SLOT(setType(int)));
//    connect(moduleButton, SIGNAL(changeType(int)),
//            scene, SLOT(setType(int)));
//    connect(pcButton, SIGNAL(changeType(int)),
//            scene, SLOT(setType(int)));
//    connect(atomButton, SIGNAL(changeType(int)),
//            scene, SLOT(setType(int)));

//    connect(lineButton, SIGNAL(clicked()),
//            scene, SLOT(lineButtonClicked()));

//    connect(scene, SIGNAL(keepLineButtonStateDown()),
//            lineButton, SLOT(setStateDown()));
//    connect(scene, SIGNAL(changeLineButtonState()),
//            lineButton, SLOT(setStateUp()));

    connect(scene, SIGNAL(itemDoubleClicked(Item*)),
            this, SLOT(changeItemProperties(Item*)));
    connect(scene, SIGNAL(sceneModified()),
            this, SLOT(sceneChanged()));
}

void Dialog::additionalActions()
{
    saveAction = new QAction("Save",this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveToFile()));
    this->addAction(saveAction);

    loadAction = new QAction("Load",this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFromFile()));
    this->addAction(loadAction);

    deleteAction = new QAction("Delete",this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
    this->addAction(deleteAction);

    closeAction = new QAction("Close", this);
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(accept()));
    this->addAction(closeAction);

    lineAction = new QAction(QIcon(":/base/line"),"Line",this);
    lineAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    lineAction->setToolTip(QString("Connects 2 Items - %1")
                             .arg(lineAction->shortcut().toString()));
    connect(lineAction,SIGNAL(triggered()), this, SLOT(enterLineMode()));
    this->addAction(lineAction);

    item1Action = new ItemAction(1, ":/GSW/network",this);
    item1Action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    item1Action->setToolTip(QString("Creates a Network Item - %1")
                            .arg(item1Action->shortcut().toString()));
    connect(item1Action, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(item1Action);

    item2Action = new ItemAction(2, ":/GSW/pc", this);
    item2Action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    item2Action->setToolTip(QString("Creates a PC Item - %1")
                            .arg(item2Action->shortcut().toString()));
    connect(item2Action, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(item2Action);


    item3Action = new ItemAction(3, ":/GSW/module",this);
    item3Action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    item3Action->setToolTip(QString("Creates a Module Item - %1")
                            .arg(item3Action->shortcut().toString()));
    connect(item3Action, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(item3Action);

    item4Action = new ItemAction(4, ":/GSW/atom",this);
    item4Action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
    item4Action->setToolTip(QString("Creates an Atom Item - %1")
                            .arg(item4Action->shortcut().toString()));
    connect(item4Action, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(item4Action);
}

void Dialog::additionalToolbars()
{
    itemToolbar = new QToolBar("Items");
    itemToolbar->addAction(item1Action);
    itemToolbar->addAction(item2Action);
    itemToolbar->addAction(item3Action);
    itemToolbar->addAction(item4Action);
    itemToolbar->addSeparator();
    itemToolbar->addAction(lineAction);
}

void Dialog::additionalMenus()
{
    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    fileMenu->addAction(closeAction);
    editMenu->addAction(deleteAction);
}

void Dialog::changePixmap(const QString &path)
{
    pixmap->load(path,0,Qt::AutoColor);
    scene ->setImagePath(path);
}

void Dialog::saveToFile()
{
    scene->setMode(GraphicsScene::dragMode);
    QString path = QFileDialog::getSaveFileName(this, "Save");

    if(!path.isEmpty())
        save(path);
}

void Dialog::loadFromFile()
{
    if (okToContinue())
    {
        scene->setMode(GraphicsScene::dragMode);
        QString filename = QFileDialog::getOpenFileName(this, "Open");
        if(filename != NULL)
            load(filename);
    }
}

void Dialog::save(const QString &filename)
{
    QSettings sceneset(filename, QSettings::IniFormat, this);
    sceneset.clear();

    QApplication::setOverrideCursor(Qt::WaitCursor);


    QList <QGraphicsItem *> itemList = scene->items();
    if(itemList.count()>0)
    {
        Item *myCountItem = qgraphicsitem_cast <Item *> (itemList.at(0));
        sceneset.setValue("IDCount", myCountItem->getItemCount());
        int i = 1;
        QList<QString> listOfItems;

        foreach (QGraphicsItem *item, itemList)
        {
            if (Item *myItem = qgraphicsitem_cast <Item *> (item))
            {
                listOfItems.append(QString("%1").arg(myItem->getID()));
                sceneset.beginGroup(QString("%1").arg(myItem->getID()));
                sceneset.setValue("ToolTip", myItem->toolTip());
                sceneset.setValue("Type", myItem->getType());
                QPointF p = myItem->pos();
                sceneset.setValue("PositionX", p.rx());
                sceneset.setValue("PositionY", p.ry());
                sceneset.setValue("Name", myItem->getName());
                sceneset.setValue("ToolTip", myItem->toolTip());
                QList<Item *> myList = myItem->getLowerTierItemList();
                sceneset.endGroup();
                foreach (Item *item, myList)
                {
                    sceneset.beginGroup("Connections");
                    sceneset.setValue(QString("Connection_%1").arg(i), myItem->getID());
                    sceneset.setValue(QString("connectedTo_%1").arg(i), item->getID());
                    i++;
                    sceneset.endGroup();
                }
            }
        }
        sceneset.setValue("NumberOfConnections", --i);
        sceneset.setValue("NumberOfItems", listOfItems.size());
        sceneset.beginGroup("ListOfItems");
        int j=0;
        foreach(QString string, listOfItems)
        {
            sceneset.setValue(QString("%1").arg(j++), string);
        }
    }
    setWindowModified(false);
    QApplication::restoreOverrideCursor();
}

void Dialog::load(const QString &fileName)
{
    QList <QString> imagePath;
    imagePath.append(":/GSW/network");
    imagePath.append(":/GSW/pc");
    imagePath.append(":/GSW/module");
    imagePath.append(":/GSW/atom");
    scene->clear();

    QSettings sceneset(fileName, QSettings::IniFormat, this);
    QList <int> listOfItems;
    QMap <int, Item *> listOfPointers;
    for(int i(sceneset.value("NumberOfItems").toInt()-1); i>=0; i--)
    {
        listOfItems.append(sceneset.value(QString("ListOfItems/%1").arg(i)).toInt());
    }

    foreach(int ID, listOfItems)
    {
        sceneset.beginGroup(QString("%1").arg(ID));
        int type = sceneset.value("Type").toInt();
        QString toolTip = sceneset.value("ToolTip").toString();
        Item *myItem = new Item(type, imagePath.at(type-1), toolTip);
        scene->addItem(myItem);
        myItem->setPos(sceneset.value("PositionX").toDouble() ,sceneset.value("PositionY").toDouble());
        myItem->setID(ID);
        myItem->changeName(sceneset.value("Name").toString());
        sceneset.endGroup();
        listOfPointers.insert(ID, myItem);
        myItem->setItemCount(sceneset.value("IDCount").toInt());
    }


    for(int i = 1; i <= sceneset.value("NumberOfConnections").toInt(); i++)
    {
        sceneset.beginGroup("Connections");
        Item *startingItem = listOfPointers.value(sceneset.value(QString("Connection_%1").arg(i)).toInt());
        Item *endingItem = listOfPointers.value(sceneset.value(QString("ConnectedTo_%1").arg(i)).toInt());
        Line *myLine = new Line(startingItem, endingItem);
        scene->addItem(myLine);
        sceneset.endGroup();
    }
    setWindowModified(false);
    setWindowTitle(QString("%1[*] - GraphicssceneWidget")
                   .arg(QFileInfo(fileName).fileName()));
}

void Dialog::deleteItem()
{
     foreach (QGraphicsItem *item, scene->selectedItems())
     {
         if (item->type() == Line::Type)
         {
             Line *line = qgraphicsitem_cast<Line *>(item);
             line->startingItem()->removeConnectedItem(line->endingItem());
             line->startingItem()->removeLine(line);
             line->startingItem()->update(84,0,16,16);
             line->endingItem()->changeConnection();
             line->endingItem()->removeLine(line);
             scene->removeItem(item);
             delete item;
         }
     }

     foreach (QGraphicsItem *item, scene->selectedItems())
     {
          if (item->type() == Item::Type)
          {
              Item *object = qgraphicsitem_cast<Item *>(item);
              foreach (Line *line, object->getConnectedLines())
              {
                  if(object == line->endingItem())
                      line->startingItem()->removeConnectedItem(object);
              }
              object->getHigherTierItem()->update(0,84,16,16);
              object->removeLines();
              foreach (Item *item, object->getLowerTierItemList())
              {
                  item->changeConnection();
              }
              scene->removeItem(item);
              delete item;
          }
      }
 }

void Dialog::changeItemProperties(Item *item)
{
    InputDialog inputdialog(item->getName(), item->getInfo(), this);

    inputdialog.show();

    if(inputdialog.exec())
    {
        QString name = inputdialog.getNewName();
        QString info = inputdialog.getNewToolTip();

        while (info.startsWith("\n"))
            info.remove(0,1);

        item->changeName(name);
        item->setInfo(info);
        item->changeToolTip();
        this->setWindowModified(true);
    }
}

void Dialog::sceneChanged()
{
    this->setWindowModified(true);
}

bool Dialog::okToContinue()
{
    if(isWindowModified())
    {
        int decission = QMessageBox::warning(this,"GraphicsSceneWidget",
                                             "The document has been modified.\n"
                                             "Do you want to save your changes?",
                                             QMessageBox::Yes | QMessageBox::No
                                             | QMessageBox::Cancel);
        if (decission == QMessageBox::Yes)
        {
            saveToFile();
        }
        else if (decission == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

void Dialog::accept()
{
    if(okToContinue())
    {
        QDialog::accept();
    }
}

void Dialog::reject()
{
    if(okToContinue())
    {
        QDialog::reject();
    }
}

void Dialog::enterLineMode()
{
    scene->enableLineMode();
}

void Dialog::enterItemMode(int type, QString path)
{
    scene->enableItemMode(type,path);
}



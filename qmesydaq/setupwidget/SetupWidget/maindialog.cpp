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

#include "maindialog.h"
#include "canvas.h"
#include "setupview.h"
#include "pc.h"
#include "mcpd_8.h"
#include "mpsd_8.h"
#include "channel.h"
#include "pcinputdialog.h"
#include "mcpd_8inputdialog.h"
#include "mpsd_8inputdialog.h"
#include "channelinputdialog.h"

#include <inputdialog.h>
#include <itemaction.h>
#include <button.h>
#include <QPixmap>
#include <QFileDialog>
#include <QDebug>
#include <item.h>
#include <QSettings>
#include <QFileDialog>
#include <QMap>
#include <QHash>
#include <QtAlgorithms>
#include <QMenu>
#include <QToolBar>
#include <QMessageBox>
#include <QComboBox>
#include <QStringList>
#include <QApplication>

MainDialog::MainDialog(QWidget *parent)
    :DialogBase(parent)
{
    pixmap = new QPixmap;
    scene = new Canvas;
    view = new SetupView(this);
    scene->setSceneRect(QRect(0,0,view->width(),view->height()));
    view->setScene(scene);
    zoomBox = new QComboBox;
    QStringList zoomStages;
    zoomStages << ("100%") << ("25%") << ("50%") << ("75%") << ("100%")
               << ("125%") << ("150%") << ("175%") << ("200%");

    zoomBox->addItems(zoomStages);
    zoomBox->setCurrentIndex(0);

    setWindowTitle("[*]SetupWidget");

    additionalConnections();
    additionalActions();
    addittionalToolbars();
    additionalMenus();
    additionalLayout();
}

void MainDialog::additionalConnections()
{
    connect(scene, SIGNAL(itemDoubleClicked(Item*)),
            this, SLOT(changeItemProperties(Item*)));

    connect(scene, SIGNAL(center(QGraphicsItem*)),
            this, SLOT(centerView(QGraphicsItem*)));

    connect(scene, SIGNAL(itemExpansionChanged()),
            scene, SLOT(sortItems()));
    connect(scene, SIGNAL(sceneModified()),
            this, SLOT(sceneChanged()));
    connect(zoomBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(setZoomFactor(QString)));
    connect(view, SIGNAL(zoomChanged(double)),
            this, SLOT(setZoomBox(double)));
    connect(zoomBox, SIGNAL(highlighted(int)),
            this, SLOT(resetCursor(int)));
}

void MainDialog::additionalLayout()
{
    lowerLayout->addWidget(view);
    upperLayout->addWidget(itemToolbar,1,0);
}

void MainDialog::additionalActions()
{
    saveAction = new QAction(QIcon(":/icons/save"),"Save",this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveToFile()));
    this->addAction(saveAction);

    loadAction = new QAction(QIcon(":/icons/load"),"Load",this);
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFromFile()));
    this->addAction(loadAction);

    deleteAction = new QAction("Delete",this);
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
    this->addAction(deleteAction);

    resetAction = new QAction("New",this);
    resetAction->setShortcut(QKeySequence::New);
    connect(resetAction, SIGNAL(triggered()), this, SLOT(resetWidget()));
    this->addAction(resetAction);

    markAction = new QAction("Mark Subtree",this);
    markAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(markAction, SIGNAL(triggered()), this, SLOT(markSubTree()));
    this->addAction(markAction);

    closeAction = new QAction("Close", this);
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(accept()));
    this->addAction(closeAction);

    sortAction = new QAction("Sort", this);
    sortAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));
    connect(sortAction, SIGNAL(triggered()), scene, SLOT(sortItems()));
    this->addAction(sortAction);

    lineAction = new QAction(QIcon(":/icons/line"),"Line",this);
    lineAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    lineAction->setToolTip(QString("Connects 2 Items - %1")
                             .arg(lineAction->shortcut().toString()));
    connect(lineAction,SIGNAL(triggered()), this, SLOT(enterLineMode()));
    this->addAction(lineAction);

    pcAction = new ItemAction(1, ":/icons/pc",this);
    pcAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    pcAction->setToolTip(QString("Creates a Control PC- %1")
                            .arg(pcAction->shortcut().toString()));
    connect(pcAction, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(pcAction);

    mcpdAction = new ItemAction(2, ":/icons/mcpd", this);
    mcpdAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    mcpdAction->setToolTip(QString("Creates a MCPD - %1")
                            .arg(mcpdAction->shortcut().toString()));
    connect(mcpdAction, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(mcpdAction);


    mpsdAction = new ItemAction(3, ":/icons/mpsd",this);
    mpsdAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    mpsdAction->setToolTip(QString("Creates a MPSD - %1")
                            .arg(mpsdAction->shortcut().toString()));
    connect(mpsdAction, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(mpsdAction);

    channelAction = new ItemAction(4, ":/icons/channel",this);
    channelAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));
    channelAction->setToolTip(QString("Creates a Detector tube - %1")
                            .arg(channelAction->shortcut().toString()));
    connect(channelAction, SIGNAL(enableItemMode(int,QString)),
            this, SLOT(enterItemMode(int,QString)));
    this->addAction(channelAction);

    multiItemAction = new QAction(QIcon(":/icons/multi"), "Multiple Items", this);
    multiItemAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));
    multiItemAction->setToolTip((QString("Add multiple Items or Trees - %1")
                                 .arg(multiItemAction->shortcut().toString())));
    connect(multiItemAction, SIGNAL(triggered()), scene, SLOT(addMultipleItems()));
    this->addAction(multiItemAction);

    resetZoomAction = new QAction("Reset Zoom", this);
    resetZoomAction->setToolTip(QString("Reset Zoom to 100%"));
    connect(resetZoomAction, SIGNAL(triggered()), view, SLOT(resetZoom()));
    this->addAction(resetZoomAction);

    clearOnLoad = new QAction("Clear scene for loading", this);
    clearOnLoad->setToolTip("Clear the scene before loading files");
    clearOnLoad->setCheckable(true);
    clearOnLoad->setChecked(true);
    this->addAction(clearOnLoad);

    copyAction = new QAction("Copy", this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setToolTip((QString("Copy Selection - %1")
                            .arg(copyAction->shortcut().toString())));
    connect(copyAction, SIGNAL(triggered()), scene, SLOT(copy()));
    this->addAction(copyAction);

    pasteAction = new QAction("Paste", this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setToolTip((QString("Paste Clipboard to Scene - %1")
                             .arg(pasteAction->shortcut().toString())));
    connect(pasteAction, SIGNAL(triggered()), scene, SLOT(paste()));
    this->addAction(pasteAction);

    zoomLockAction = new QAction("Freeze zoomlevel", this);
    zoomLockAction->setToolTip("Keep Zoom at current value");
    zoomLockAction->setCheckable(true);
    zoomLockAction->setChecked(false);
    connect(zoomLockAction, SIGNAL(toggled(bool)), \
            view, SLOT(changeZoomLock(bool)));
    this->addAction(zoomLockAction);

    onlySubtreeAction = new QAction("Mark only Subtree", this);
    onlySubtreeAction->setToolTip("States weather 'Mark Subtree' keeps the top Item marked");
    onlySubtreeAction->setCheckable(true);
    onlySubtreeAction->setChecked(false);
    this->addAction(onlySubtreeAction);
}

void MainDialog::additionalMenus()
{
    fileMenu->addAction(resetAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(loadAction);
    editMenu->addAction(markAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    toolMenu->addAction(sortAction);
    toolMenu->addAction(multiItemAction);
    toolMenu->addAction(resetZoomAction);
    optionMenu->addAction(clearOnLoad);
    optionMenu->addAction(zoomLockAction);
    optionMenu->addAction(onlySubtreeAction);
}

void MainDialog::addittionalToolbars()
{
    itemToolbar = new QToolBar("Items");
    itemToolbar->addAction(pcAction);
    itemToolbar->addAction(mcpdAction);
    itemToolbar->addAction(mpsdAction);
    itemToolbar->addAction(channelAction);
    itemToolbar->addSeparator();
    itemToolbar->addAction(multiItemAction);
    itemToolbar->addSeparator();
    itemToolbar->addAction(lineAction);
    itemToolbar->addSeparator();
    itemToolbar->addWidget(zoomBox);
}

void MainDialog::changeItemProperties(Item *item)
{
    if(item->getType() == 1)//type 1 = pc
    {
        PC *myItem = dynamic_cast <PC *> (item);
        PCInputDialog inputdialog(myItem->getName(), myItem->getInfo(),
                                  QString("%1").arg(myItem->getModuleID()),this);

        if(inputdialog.exec())
        {
            QString info;
            if (!inputdialog.getNewToolTip().isEmpty())
            {
                info = (QString("\n%1").arg(inputdialog.getNewToolTip()));
                while (info.startsWith("\n\n"))
                    info.remove(0,1);
            }

            myItem->changeName(inputdialog.getNewName());
            myItem->setInfo(info);
            myItem->changePcID(inputdialog.getNewID());
            myItem->changeToolTip();
        }
        this->setWindowModified(true);

    }
    if(item->getType() == 2) //type 2 = mcpd
    {
        MCPD_8 *myItem = dynamic_cast <MCPD_8 *> (item);

        MCPD_8InputDialog inputdialog(myItem->getName(), myItem->getInfo(),
                                      myItem->getIP(), myItem->getPort(),
                                      myItem->getModuleID(), this);

        if(inputdialog.exec())
        {
            QString info;
            if (!inputdialog.getNewToolTip().isEmpty())
            {
                info = (QString("\n%1").arg(inputdialog.getNewToolTip()));
                while (info.startsWith("\n\n"))
                    info.remove(0,1);
            }

            myItem->changeName(inputdialog.getNewName());
            myItem->setInfo(info);
            myItem->changeIP(inputdialog.getNewIP());
            myItem->changePort(inputdialog.getNewPort());
            myItem->changeModuleID(inputdialog.getNewID());
            myItem->changeToolTip();
        }
        this->setWindowModified(true);
    }
    else if(item->getType() == 3) //type 3 = mpsd
    {
        MPSD_8 *myItem = dynamic_cast <MPSD_8 *> (item);
        MPSD_8InputDialog inputdialog(myItem->getName(), myItem->getInfo(),
                                      QString("%1").arg(myItem->getModuleID()),this);

        if(inputdialog.exec())
        {
            QString info;
            if (!inputdialog.getNewToolTip().isEmpty())
            {
                info = (QString("\n%1").arg(inputdialog.getNewToolTip()));
                while (info.startsWith("\n\n"))
                    info.remove(0,1);
            }

            myItem->changeName(inputdialog.getNewName());
            myItem->setInfo(info);
            myItem->changeModuleID(inputdialog.getNewID());
            myItem->changeToolTip();
        }
        this->setWindowModified(true);
    }
    else if(item->getType() == 4) // type 4 = channel
    {
        Channel *myItem = dynamic_cast <Channel *> (item);
        ChannelInputDialog inputdialog(myItem->getName(), myItem->getInfo(),
                                       QString("%1").arg(myItem->getModuleID()), this);

        if(inputdialog.exec())
        {
            QString info;
            if (!inputdialog.getNewToolTip().isEmpty())
            {
                info = (QString("\n%1").arg(inputdialog.getNewToolTip()));
                while (info.startsWith("\n\n"))
                    info.remove(0,1);
            }

            myItem->changeName( inputdialog.getNewName());
            myItem->setInfo(info);
            myItem->changeModuleID(inputdialog.getNewID());
            myItem->changeToolTip();
        }
        this->setWindowModified(true);
    }

}

void MainDialog::deleteItem()
{
    foreach (QGraphicsItem *item, scene->selectedItems())
    {
        if (item->type() == Line::Type)
        {
            Line *line = dynamic_cast<Line *>(item);
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
         if (Item *object = dynamic_cast<Item *>(item))
         {
             foreach (Line *line, object->getConnectedLines())
             {
                 if(object == line->endingItem())
                     line->startingItem()->removeConnectedItem(object);
             }

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



void MainDialog::changePixmap(const QString &path)
{
    pixmap->load(path,0,Qt::AutoColor);
    scene ->setImagePath(path);
}

void MainDialog::centerView(QGraphicsItem *item)
{
    view->centerOn(item);
}

void MainDialog::saveToFile()
{
    scene->setMode(GraphicsScene::dragMode);
    QString path = QFileDialog::getSaveFileName(this, "Save");

    if(!path.isEmpty())
        save(path);
}

void MainDialog::save(const QString &fileName)
{
    QSettings mesidaq(fileName, QSettings::IniFormat, this);
    mesidaq.clear();
    mesidaq.setValue("MS", 2706);
    mesidaq.setValue("custom", true);
    mesidaq.setValue("ItemCounter", Item::getItemCount());
    mesidaq.setValue("PCCounter", PC::getModuleCount());
    mesidaq.setValue("MCPDCounter", MCPD_8::getModuleCount());
    mesidaq.setValue("MPSDCounter", MPSD_8::getModuleCount());
    mesidaq.setValue("ChannelCounter", Channel::getModuleCount());

    QList<QGraphicsItem *> objectList = scene->items();
    QList<Item *> itemList;

    foreach (QGraphicsItem *object, objectList)
    {
        if(Item *myItem = dynamic_cast <Item *> (object))
            itemList.append(myItem);
    }


    if(itemList.count()>0)
    {
        Item *item = dynamic_cast <Item *> (itemList.first());
        mesidaq.setValue("ItemCount",item->getItemCount());
        int i = 1; //connectionCounter
        int j = 1;
        QList <PC *> listOfPcs;
        QList <MCPD_8 *> listOfMcpds;
        QList <MPSD_8 *> listOfMpsds;
        QList <Channel *> listOfChannels;

        foreach (Item *item, itemList)
            if (!item->isVisible())
            {
                mesidaq.setValue(QString("Invisibles/%1").arg(j),item->getID());
                j++;
            }
        mesidaq.setValue("NumberOfInvisibleItems", j);


        foreach (Item *item, itemList)
        if (PC *pc = dynamic_cast <PC *> (item))
                    listOfPcs.append(pc);
        else if(MCPD_8  *mcpd = dynamic_cast <MCPD_8 *> (item))
                    listOfMcpds.append(mcpd);
        else if(MPSD_8 *mpsd = dynamic_cast <MPSD_8 *> (item))
                    listOfMpsds.append(mpsd);
        else if(Channel *channel = dynamic_cast <Channel *> (item))
                    listOfChannels.append(channel);

        foreach (PC *pc, listOfPcs) //saving PCs
        {
            int id = pc->getModuleID();
            mesidaq.beginGroup(QString("PC-%1").arg(id));
            mesidaq.setValue("id", id);
            mesidaq.setValue("globalID", pc->getID());
            mesidaq.setValue("Info", pc->getInfo());
            mesidaq.setValue("Name", pc->getName());
            QList<Item *> myList = pc->getLowerTierItemList();
            mesidaq.endGroup();

            foreach(Item *item, myList)
            {
                    mesidaq.beginGroup("Connections");
                    mesidaq.setValue(QString("Connection_%1").arg(i),pc->getID());
                    mesidaq.setValue(QString("ConnectedTo_%1").arg(i),item->getID());
                    i++;
                    mesidaq.endGroup();
            }
        }

        foreach (MCPD_8 *mcpd, listOfMcpds) //saving MCPDs
        {
            int id = mcpd->getModuleID();
            mesidaq.beginGroup(QString("MCPD-%1").arg(id));
            mesidaq.setValue("id", id);
            mesidaq.setValue("globalID", mcpd->getID());
            mesidaq.setValue("ipAddress", mcpd->getIP());
            mesidaq.setValue("Port", mcpd->getPort());
            mesidaq.setValue("Info", mcpd->getInfo());
            mesidaq.setValue("Name", mcpd->getName());
            QList<Item *> myList = mcpd->getLowerTierItemList();
            mesidaq.endGroup();

            foreach(Item *item, myList)
            {
                mesidaq.beginGroup("Connections");
                mesidaq.setValue(QString("Connection_%1").arg(i),mcpd->getID());
                mesidaq.setValue(QString("ConnectedTo_%1").arg(i),item->getID());
                i++;
                mesidaq.endGroup();
            }
        }
        foreach (MPSD_8 *mpsd, listOfMpsds) //saving MPSDs
        {
            int id = mpsd->getModuleID();
            mesidaq.beginGroup(QString("MODULE-%1").arg(id));
            mesidaq.setValue("id", id);
            mesidaq.setValue("globalID", mpsd->getID());
            mesidaq.setValue("Info", mpsd->getInfo());
            mesidaq.setValue("Name", mpsd->getName());
            QList<Item *> myList = mpsd->getLowerTierItemList();
            mesidaq.endGroup();

            foreach(Item *item, myList)
            {
                mesidaq.beginGroup("Connections");
                mesidaq.setValue(QString("Connection_%1").arg(i),mpsd->getID());
                mesidaq.setValue(QString("ConnectedTo_%1").arg(i),item->getID());
                i++;
                mesidaq.endGroup();
            }
        }
        foreach (Channel *channel, listOfChannels) //saving Channels
        {
            int id = channel->getModuleID();
            mesidaq.beginGroup(QString("Channel-%1").arg(id));
            mesidaq.setValue("id", id);
            mesidaq.setValue("globalID", channel->getID());
            mesidaq.setValue("Info", channel->getInfo());
            mesidaq.setValue("Name", channel->getName());
            mesidaq.endGroup();
        }
        mesidaq.setValue("NumberOfConnections", i);

        mesidaq.setValue("ItemCounter", Item::getItemCount());
        mesidaq.setValue("PCCounter", PC::getModuleCount());
        mesidaq.setValue("MCPDCounter", MCPD_8::getModuleCount());
        mesidaq.setValue("MPSDCounter", MPSD_8::getModuleCount());
        mesidaq.setValue("ChannelCounter", Channel::getModuleCount());


        Item::setItemCount(mesidaq.value("ItemCounter").toInt());
        PC::setPcCount(mesidaq.value("PCCounter").toInt());
        MCPD_8::setMcpdCount(mesidaq.value("MCPDCounter").toInt());
        MPSD_8::setMpsdCount(mesidaq.value("MPSDCounter").toInt());
        Channel::setChannelCount(mesidaq.value("ChannelCounter").toInt());
    }
}

void MainDialog::loadFromFile()
{
    if(clearOnLoad->isChecked() && !okToContinue())
        return;
    else
    {
        scene->setMode(GraphicsScene::dragMode);
        QString filename = QFileDialog::getOpenFileName(this, "Open");
        if(filename != NULL)
            load(filename);
    }
}

void MainDialog::load(const QString &fileName)
{
    QSettings mesidaq(fileName, QSettings::IniFormat,this);


    int test = mesidaq.value("MS").toInt();
    bool custom = mesidaq.value("custom").toBool();
    if (custom && test != 2706)
        return;
    int currentItemCount = 0;
    int currentPcCount = 0;
    int currentMcpdCount = 0;
    int currentMpsdCount = 0;
    int currentChannelCount = 0;
    QList <PC*> addedPCs;
    QList <MCPD_8*> addedMCPDs;
    QList <MPSD_8*> addedMPSDs;
    QList <Channel*> addedChannels;

    if(clearOnLoad->isChecked())
    {
        resetWidget();
    }
    else //get the ID offsets for onTopLoading
    {
        currentItemCount = Item::getItemCount();
        currentPcCount = PC::getModuleCount();
        currentMcpdCount = MCPD_8::getModuleCount();
        currentMpsdCount = MPSD_8::getModuleCount();
        currentChannelCount = Channel::getModuleCount();
    }

    QList <QString> keys = mesidaq.allKeys(); //get all Keys from the INI file
    QList <QString> itemsToAdd;

    QMap <int, Item*> customAddedItems;
    QList <int> visibles; //get all visible item IDs
    for(int i = 1, j = mesidaq.value("NumberOfInvisibleItems").toInt();i<j;i++)
    {
        visibles.append(mesidaq.value(QString("Invisibles/%1").arg(i)).toInt());
    }

    foreach (QString string, keys)
    {
        string.resize(string.indexOf("/")); //cut of all the chars after the 1st "/"
        int j = mesidaq.value(QString("%1/id").arg(string)).toInt(); //get id
        string.prepend(QString("%1").arg(j)); //prepend id and zeros for sort algorithm
        if (j<10)
            string.prepend("0");
        if (j<100)
            string.prepend("0");

        if((string.contains("MCPD")
                || string.contains("MODULE")
                || string.contains("Channel")
                || string.contains("PC"))
                && !itemsToAdd.contains(string)) //create a unique item list
            itemsToAdd.append(string);
    }

    qSort(itemsToAdd.begin(), itemsToAdd.end()); //sort the unique item list

    foreach (QString string, itemsToAdd)
    {
        Item *myItem;
        string.remove(0,3); //strip the prepended id
        int moduleID = (mesidaq.value(QString("%1/id").arg(string)).toInt());

        if(string.contains("Channel"))
        {
            myItem = new Channel(4,":/icons/channel","");
            Channel *myChannel = dynamic_cast <Channel *> (myItem);
            myChannel->changeModuleID(moduleID+currentChannelCount);
            addedChannels.append(myChannel);
        }
        else if(string.contains("MODULE"))
        {
            myItem = new MPSD_8(3,":/icons/mpsd","");
            MPSD_8 *myMpsd = dynamic_cast <MPSD_8 *> (myItem);
            myMpsd->changeModuleID(moduleID+currentMpsdCount);
            addedMPSDs.append(myMpsd);
        }

        else if(string.contains("MCPD"))
        {
            QString ip = mesidaq.value(QString("%1/ipAddress").arg(string)).toString();
            int port = mesidaq.value(QString("%1/Port").arg(string)).toInt();
            myItem = new MCPD_8(ip, port, 2, ":/icons/mcpd","");
            MCPD_8 *myMcpd = dynamic_cast <MCPD_8 *> (myItem);
            myMcpd->changeModuleID(moduleID+currentMcpdCount);
            addedMCPDs.append(myMcpd);
        }
        else if(string.contains("PC"))
        {
            myItem = new PC(1,":/icons/pc","");
            PC *myPc = dynamic_cast <PC *> (myItem);
            myPc->changePcID(moduleID+currentPcCount);
            addedPCs.append(myPc);
        }

        if (custom) //set additional Data and globalIDs
        {
            int gID = mesidaq.value(QString("%1/globalID").arg(string)).toInt();
            myItem->setInfo(mesidaq.value(QString("%1/Info").arg(string)).toString());
            myItem->changeName(mesidaq.value(QString("%1/Name").arg(string)).toString());
            customAddedItems.insert(gID, myItem);
            myItem->setID(gID+currentItemCount);
            myItem->setZValue(myItem->getID());
        }
        myItem->changeToolTip();
        scene->addItem(myItem);
        if (visibles.contains(myItem->getID()))
            myItem->setVisible(false);
    }

    if(custom)
    {
        int numberOfConnections = mesidaq.value("NumberOfConnections").toInt();
        mesidaq.beginGroup("Connections");

        for(int i=1; i<numberOfConnections; i++)
        {
            int from = mesidaq.value(QString("Connection_%1").arg(i)).toInt();
            int to = mesidaq.value(QString("ConnectedTo_%1").arg(i)).toInt();

            Item *startingItem = customAddedItems.value(from);
            Item *endingItem = customAddedItems.value(to);

            Line *myLine = new Line(startingItem, endingItem);
            scene->addItem(myLine);
        }
        mesidaq.endGroup();

    }
    else
    {
        if(!addedMPSDs.isEmpty())
            foreach(Channel *channel, addedChannels)
            {
                int index = addedChannels.indexOf(channel);
                Line *myLine = new Line(addedMPSDs.at((index-index%8)/8),channel);
                scene->addItem(myLine);
            }

        if(!addedMCPDs.isEmpty())
            foreach(MPSD_8 *mpsd, addedMPSDs)
            {
                int index = addedMPSDs.indexOf(mpsd);
                Line *myLine = new Line(addedMCPDs.at((index-index%8)/8),mpsd);
                scene->addItem(myLine);
            }

        if(!addedPCs.isEmpty())
            foreach(MCPD_8 *mcpd, addedMCPDs)
            {
                int index = addedMCPDs.indexOf(mcpd);
                Line *myLine = new Line(addedPCs.at((index-index%8)/8),mcpd);
                scene->addItem(myLine);
            }
    }

    scene->sortItems();

    if (clearOnLoad->isChecked())
        setWindowModified(false);
    else
        setWindowModified(true);
}

void MainDialog::resetWidget()
{
    scene->clear();
    scene->setSceneRect(QRect(0,0,view->width()-10,view->height()-10));
    Item::setItemCount(0);
    PC::setPcCount(0);
    MCPD_8::setMcpdCount(0);
    MPSD_8::setMpsdCount(0);
    Channel::setChannelCount(0);
    view->setZoom(1.0);
}

void MainDialog::markSubTree()
{
    QList<QGraphicsItem *> qItems = scene->selectedItems();
    QList<Item *> items;

    foreach(QGraphicsItem *qItem, qItems)
        if (Item *myItem = dynamic_cast <Item *> (qItem))
            items.append(myItem);


    foreach(Item *item, items)
    {
        setSelected(item);
        if (onlySubtreeAction->isChecked()
                && !item->getLowerTierItemList().isEmpty())
            item->setSelected(false);
    }
}

void MainDialog::setSelected(Item *item)
{
    if(item->getLowerTierItemList().isEmpty())
        item->setSelected(true);
    else
    {
        foreach (Item *subItem, item->getLowerTierItemList())
            setSelected(subItem);
        item->setSelected(true);
    }
}

void MainDialog::sceneChanged()
{
    this->setWindowModified(true);
}

bool MainDialog::okToContinue()
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

void MainDialog::accept()
{
    if(okToContinue())
    {
        QDialog::accept();
    }
}

void MainDialog::reject()
{
    if(okToContinue())
    {
        QDialog::reject();
    }
}


void MainDialog::enterLineMode()
{
    scene->enableLineMode();
}

void MainDialog::enterItemMode(int type, QString path)
{
    scene->enableItemMode(type,path);
}

void MainDialog::setZoomFactor(QString factor)
{
    factor.chop(1);
    double zoom = factor.toDouble()/100;
    view->setZoom(zoom);
}

void MainDialog::setZoomBox(double zoom)
{
    zoomBox->setItemText(0,QString("%1%").arg(QString::number(zoom*100,'f',0)));
    zoomBox->setCurrentIndex(0);
}
void MainDialog::resetCursor(int)
{
    QApplication::restoreOverrideCursor();
}

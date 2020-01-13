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

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFrame>
#include <QFileDialog>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QApplication>

#include "dialogbase.h"
#include "button.h"

DialogBase::DialogBase(QWidget *parent)
    : QDialog(parent)
{
    createButtons();
    createActions();
    createMenubar();
    createConnections();
    createLayout();
}

void DialogBase::createLayout()
{

    upperLayout = new QGridLayout;
    upperLayout->addWidget(menuBar,0,0);

    lowerLayout = new QGridLayout;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(upperLayout);
    mainLayout->addLayout(lowerLayout);

    setLayout(mainLayout);
}

void DialogBase::createButtons()
{
}

void DialogBase::createActions()
{
    aboutQtAction = new QAction("About &Qt", this);
    aboutQtAction->setStatusTip("Show the Qt library's About box");
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void DialogBase::createConnections()
{

}

void DialogBase::createMenubar()
{
    menuBar = new QMenuBar;
    fileMenu = menuBar->addMenu("&File");
    editMenu = menuBar->addMenu("&Edit");
    toolMenu = menuBar->addMenu("&Tools");
    optionMenu = menuBar->addMenu("&Options");
    menuBar->addSeparator();
    helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction(aboutQtAction);

}


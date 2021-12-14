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

#ifndef DIALOGBASE_H
#define DIALOGBASE_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QAction>

class Button;
class QMenuBar;
class QMenu;
class QAction;
class QToolBar;

class DialogBase : public QDialog
{
    Q_OBJECT
public:
    DialogBase(QWidget *parent = 0);

signals:

public slots:

protected:
    void createLayout();
    void createActions();
    void createButtons();
    void createConnections();
    void createMenubar();
//    void createToolbar();
    QGridLayout *upperLayout;
    QGridLayout *lowerLayout;
    Button *lineButton;
    QAction *saveAction;
    QAction *loadAction;
    QAction *lineAction;
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;
    QToolBar *itemToolbar;

private:
    QDialogButtonBox *buttonBox;
    QAction *aboutQtAction;

};

#endif // DIALOGBASE_H

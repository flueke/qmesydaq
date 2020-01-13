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

#include <QApplication>
#include <QDesktopWidget>

#include "maindialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainDialog dialog;

    const unsigned short defaultWidth = app.desktop()->width()*3/4;
    const unsigned short defaultHeight = app.desktop()->height()*3/4;
    const unsigned short defaultX = app.desktop()->width()/2 - defaultWidth/2;
    const unsigned short defaultY = app.desktop()->height()/2 - defaultHeight/2;

    dialog.setGeometry(defaultX, defaultY, defaultWidth, defaultHeight);

    dialog.show();

    return app.exec();
}


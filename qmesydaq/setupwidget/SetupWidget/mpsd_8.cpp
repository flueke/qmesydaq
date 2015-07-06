/***************************************************************************
 *   Copyright (C) 2013-2015 by Stefan Rainow <stefan.rainow@frm2.tum.de>  *
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

#include "mpsd_8.h"

#include <item.h>
#include <QDebug>

MPSD_8::MPSD_8(int type, QString path, QString toolTip, QGraphicsItem *parent)
    :Item(type, path, toolTip, parent)
{
    changeName(QString("MPSD %1").arg(mpsdCount));
    myModuleID=mpsdCount++;
    changeToolTip();
    maxNumberOfSubitems = 8;
}

MPSD_8::MPSD_8(MPSD_8 *mpsd)
    :Item(mpsd)
{
    myName = mpsd->myName;
    myModuleID = mpsdCount++;
    maxNumberOfSubitems = 8;
    changeToolTip();
}

void MPSD_8::changeModuleID(int id)
{
    myModuleID = id;
}

void MPSD_8::changeToolTip()
{
    QString toolTip(QString("Name:\t%1\nID:\t%2\nInfo:%3%4")
                    .arg(myName).arg(myModuleID).arg(myInfo));
    setToolTip(toolTip);
}

int MPSD_8::mpsdCount = 0;

void MPSD_8::setMpsdCount(int count)
{
    mpsdCount = count;
}

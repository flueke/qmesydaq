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

#include "pc.h"

#include <item.h>
#include <QDebug>

PC::PC(int type, QString path, QString toolTip, QGraphicsItem *parent)
    :Item(type, path, toolTip, parent)
{
    changeName(QString("PC %1").arg(pcCount));
    myPcID = pcCount++;
    changeToolTip();
    maxNumberOfSubitems = 16;
}

PC::PC(PC *pc)
    :Item(pc)
{
    myName = pc->myName;
    myPcID = pcCount++;
    maxNumberOfSubitems = 16;
    changeToolTip();
}

void PC::changePcID(int id)
{
    myPcID = id;
}

void PC::changeToolTip()
{
    QString toolTip(QString("Name:\t%1\nID:\t%2\nInfo:%3").arg(myName).arg(myPcID)
                    .arg(myInfo));
    setToolTip(toolTip);
}

int PC::pcCount = 0;

void PC::setPcCount(int count)
{
    pcCount = count;
}

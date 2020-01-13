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

#include "mcpd_8.h"
#include "mpsd_8.h"

#include <item.h>
#include <QDebug>

MCPD_8::MCPD_8(QString ip, int port, int type, QString path, QString toolTip, QGraphicsItem *parent)
    :Item(type, path, toolTip, parent)
{
    changeName(QString("MCPD %1").arg(mcpdCount));
    myModuleID = mcpdCount++;
    myIP = QString("%1.%2").arg(ip).arg(121 + myModuleID);
    myPort = port;
    changeToolTip();
    maxNumberOfSubitems = 8;
}
MCPD_8::MCPD_8(MCPD_8 *mcpd)
    :Item(mcpd)
{
    myName = mcpd->myName;
    myIP = mcpd->myIP;
    myModuleID = mcpdCount++;
    myPort = mcpd->myPort;
    maxNumberOfSubitems = 8;
    changeToolTip();
}

void MCPD_8::changeIP(QString ip)
{
    myIP = ip;
}

void MCPD_8::changePort(int port)
{
    myPort = port;
}

void MCPD_8::changeModuleID(int id)
{
    myModuleID = id;
}

void MCPD_8::changeToolTip()
{
    QString toolTip(QString("Name:\t%1\nID:\t%2\nIP:\t%3\nPort:\t%4\nInfo:%5")
                    .arg(myName).arg(myModuleID).arg(myIP).arg(myPort).arg(myInfo));
    setToolTip(toolTip);
}

int MCPD_8::mcpdCount = 0;

void MCPD_8::setMcpdCount(int count)
{
    mcpdCount = count;
}


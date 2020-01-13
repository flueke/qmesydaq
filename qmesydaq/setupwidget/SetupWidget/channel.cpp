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

#include "channel.h"

#include <item.h>
#include<QDebug>

Channel::Channel(int type, QString path, QString toolTip, QGraphicsItem *parent)
    :Item(type, path, toolTip, parent)
{
    changeName(QString("Channel %1").arg(channelCount));
    myModuleID = channelCount++;
    changeToolTip();
    maxNumberOfSubitems = 0;
}

Channel::Channel(Channel* channel)
    :Item(channel)
{
    changeName(channel->myName);
    myModuleID = channelCount++;
    maxNumberOfSubitems = 0;
    myInfo = channel->myInfo;
    changeToolTip();
}

void Channel::changeModuleID(int id)
{
    myModuleID = id;
}

void Channel::changeToolTip()
{
    QString toolTip(QString("Name:\t%1\nID:\t%2\nInfo:%3")
                    .arg(myName).arg(myModuleID).arg(myInfo));
    setToolTip(toolTip);
}

int Channel::channelCount = 0;

void Channel::setChannelCount(int count)
{
    channelCount = count;
}



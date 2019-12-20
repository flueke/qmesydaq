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

#ifndef MCPD_8_H
#define MCPD_8_H

#include <item.h>
#include <QGraphicsItem>

class MPSD_8;

class MCPD_8 : public Item
{
public:
    enum {Type = UserType+5};
    int type() const
        { return Type;}
    MCPD_8(QString ip, int port, int type, QString path, QString toolTip, QGraphicsItem *parent=NULL);
    MCPD_8(MCPD_8* mcpd);
    QString getIP()
         {return myIP;}
    int getPort()
         {return myPort;}
    int getModuleID()
         {return myModuleID;}
    static int getModuleCount()
         {return mcpdCount;}
    void changeIP(QString ip);
    void changePort(int port);
    void changeModuleID(int id);
    void changeToolTip();
    static void setMcpdCount(int count);

private:
    QString myIP;
    static int mcpdCount;
    int myModuleID;
    int myPort;
};

#endif // MCPD_8_H

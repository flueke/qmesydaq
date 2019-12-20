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

#include "mcpd_8inputdialog.h"

#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QLabel>
#include <QGridLayout>

#include <QDebug>

MCPD_8InputDialog::MCPD_8InputDialog(QString name, QString toolTip, QString ip,
                                     int port, int id, QWidget *parent)
    :InputDialog(name, toolTip, parent)
{
    changeNewIP(ip);
    changeNewPort(QString("%1").arg(port));
    changeNewID(QString("%1").arg(id));

    ipEdit = new QLineEdit(newIP);
    QLabel *ipLabel = new QLabel("IP:");
    portEdit = new QLineEdit(QString::number(newPort));
    QLabel *portLabel = new QLabel("Port:");
    idEdit = new QLineEdit(QString::number(newID));
    QLabel *idLabel = new QLabel("ID:");

    QString oktet("(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
    QRegExp ipExp("^"+oktet+"\\."+oktet+"\\."+oktet+"\\."+oktet+"$");
    QRegExpValidator *regExpVal = new QRegExpValidator(ipExp);
    ipEdit->setValidator(regExpVal);

    QRegExp portExp("^[0-9]{1,5}$");
    portEdit->setValidator(new QRegExpValidator(portExp));
    idEdit->setValidator(new QRegExpValidator(portExp));

    mainLayout->addWidget(ipLabel,2,0);
    mainLayout->addWidget(ipEdit, 2,1);
    mainLayout->addWidget(portLabel, 3,0);
    mainLayout->addWidget(portEdit, 3,1);
    mainLayout->addWidget(idLabel, 4,0);
    mainLayout->addWidget(idEdit, 4,1);

    connect(ipEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeNewIP(QString)));
    connect(portEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeNewPort(QString)));
    connect(idEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeNewID(QString)));
}

void MCPD_8InputDialog::changeNewIP(QString ip)
{
    newIP = ip;
}

void MCPD_8InputDialog::changeNewPort(QString port)
{
    newPort = port.toInt();
}

void MCPD_8InputDialog::changeNewID(QString id)
{
    newID = id.toInt();
}

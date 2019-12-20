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

#include "channelinputdialog.h"

#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QLabel>
#include <QGridLayout>

#include <QDebug>

ChannelInputDialog::ChannelInputDialog(QString name, QString toolTip,
                                       QString id, QWidget *parent)
   :InputDialog(name, toolTip, parent)
{
    changeNewID(id);
    idEdit = new QLineEdit(QString::number(newID));
    QLabel *idLabel = new QLabel("ID: ");

    idEdit->setValidator(new QRegExpValidator(QRegExp("^[0-9]{1,5}$")));

    mainLayout->addWidget(idLabel, 4,0);
    mainLayout->addWidget(idEdit, 4,1);

    connect(idEdit, SIGNAL(textChanged(QString)),
            this, SLOT(changeNewID(QString)));
}

void ChannelInputDialog::changeNewID(QString id)
{
    newID = id.toInt();
}

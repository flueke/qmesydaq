/***************************************************************************
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "ipaddresswidget.h"
#include <QRegExp>
#include <QRegExpValidator>

/*!
    constructor

    \param address
    \param parent
 */
IPAddressWidget::IPAddressWidget(const QString &address, QWidget *parent)
	: QLineEdit(address, parent)
{
	QRegExp ex("(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
	setInputMask("000.000.000.000;0");
	setValidator(new QRegExpValidator(ex, this));
	setText(address);
}

/*!
    default constructor

    \param parent
 */
IPAddressWidget::IPAddressWidget(QWidget *parent)
	: QLineEdit(parent)
{
	QRegExp ex("(([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])\\.){3}([01]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
	setInputMask("000.000.000.000;0");
	setValidator(new QRegExpValidator(ex, this));
	setText("0.0.0.0");
}

/*************************************************************************
 * This code was developed on base of the discussion thread about a Qt   *
 * based IP input widget. The code was taken and adopted to the needed   *
 * requirements.                                                         *
 * http://stackoverflow.com/questions/9306335/an-ip-address-widget-for-qt-similar-to-mfcs-ip-address-control
 * Copyright (C) 2012 by jpo38, Tugo                                     *
 * Copyright (C) 2014 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *************************************************************************/

#ifndef IP_ADDRESS_WIDGET_H
#define IP_ADDRESS_WIDGET_H

#include <QFrame>
#include <QString>
#include "ui_ipaddresswidget.h"

class QLineEdit;
class QEvent;

class IPAddressWidget : public QFrame, public Ui_IPAddressWidget
{
	Q_OBJECT
	Q_PROPERTY(QString address READ getAddress WRITE setAddress)

public:
	//! standard constructor
	IPAddressWidget(QWidget *parent = 0);

	/**
	 * constructor
	 *
	 * \param address the IP address
	 */
	IPAddressWidget(const QString &name, QWidget *parent = 0);

	//! destructor
	~IPAddressWidget();

	//! setter method for the IP address
	void setAddress(const QString &);

	//! getter method for the IP address
	QString getAddress(void);

public slots:
	void slotTextChanged(QLineEdit *pEdit);

signals:
	void signalTextChanged(QLineEdit *pEdit);

protected:
	//! overwritten from QFrame
	bool eventFilter(QObject *obj, QEvent *event);

private:
	static const unsigned int QT_UTL_IP4_SIZE = 4;

	//! initialize the widget
	void init(void);

	//! move to the next edit entry
	void moveNextLineEdit(unsigned int i);

	//! move to the previous edit entry
	void movePrevLineEdit(int i);

private:
	//! Store the line input fields
	QLineEdit	*m_pLineEdit[QT_UTL_IP4_SIZE];
};

#endif

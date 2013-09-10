/***************************************************************************
 *   Copyright (C) 2002 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2008 by Lutz Rossa <rossa@hmi.de>                       *
 *   Copyright (C) 2009-2010 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2010 by Alexander Lenz <alexander.lenz@frm2.tum.de>     *
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

#ifndef QTINTERFACE_H
#define QTINTERFACE_H

#include <QObject>
#include "CommandEvent.h"

/*!
    \class QtInterface

    \short This class defines the Qt interface for the remote interface

    \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class QtInterface : public QObject
{
	Q_OBJECT

	//! the receiver object
	Q_PROPERTY(QObject *m_receiver READ getReceiver WRITE setReceiver)

public:
	QtInterface(QObject *receiver = 0, QObject *parent = 0);

	void setReceiver(QObject *receiver);
	QObject *getReceiver();

	void postEvent(QEvent *);
	void postCommand(CommandEvent::Command, QList<QVariant> = QList<QVariant>());
	void postCommandToInterface(CommandEvent::Command, QList<QVariant> = QList<QVariant>());

protected:
	void postRequestCommand(CommandEvent::Command, QList<QVariant> = QList<QVariant>());
	bool waitForEvent(void);

	//! receiver object 
	QObject *m_receiver;

	//! indicator for received events
	bool     m_eventReceived;
};

#endif // QTINTERFACE_H

/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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
#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <QHostAddress>

#include "mesydaqobject.h"
#include "structures.h"

class QUdpSocket;
class QSocketNotifier;

/**
 * Base class for network devices like MCPD-2, MCPD-8
 *
 * @author Gregor Montermann <g.montermann@mesytec.com>
*/
class NetworkDevice : public MesydaqObject
{
Q_OBJECT
public:
	NetworkDevice(QObject *parent = 0, QString = "192.168.168.121", quint16 = 54321);

	~NetworkDevice();

	int sendBuffer(MDP_PACKET &);

	QString ip() {return m_ipAddress;}

	quint16 port() {return m_port;}

public slots:
	void readSocketData();

signals:
	void bufferReceived(MDP_PACKET &);

private:
	int createSocket(void);

	void destroySocket(void);

private:
	QString 	m_ipAddress;

	quint16		m_port;

	QHostAddress	m_cpuAddress;

	QUdpSocket 	*m_sock;

	QSocketNotifier *m_notifyNet;

	MDP_PACKET	m_recBuf;
};

#endif

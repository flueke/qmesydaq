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
#include <QList>
#include <QMutex>

#include "mesydaqobject.h"
#include "structures.h"

class QUdpSocket;
class QSocketNotifier;

/**
 * \short Base class for network devices like MCPD-2, MCPD-8
 *
 * the objects of this class will be created and destroyed via a factory
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
*/
class NetworkDevice : public MesydaqObject
{
Q_OBJECT
public:
	static NetworkDevice *create(QObject *parent = 0, QString = "0.0.0.0", quint16 = 54321);

	static void destroy(NetworkDevice *);

private:
	NetworkDevice(QObject *parent = 0, QString = "0.0.0.0", quint16 = 54321);

	~NetworkDevice();

public:
	bool sendBuffer(const QString &target, const MDP_PACKET &packet);

	bool sendBuffer(const QString &target, const MDP_PACKET2 &packet);

	//! \return IP address of the target
	//! \return IP address of the target
	QString ip() {return m_source;}

	//! \return port number of the communication target
	quint16 port() {return m_port;}

public slots:
	//! handles the action if some data reach the socket for incoming data
	void readSocketData(void);

signals:
	//! This signal is emitted if a complete data or command packet has read.
	void bufferReceived(MDP_PACKET &);

private:
	int createSocket(void);

	void destroySocket(void);

private:
	static QMutex			m_mutex;

	static QList<NetworkDevice*>	m_networks;

	static QList<int>		m_inUse;

	quint16				m_port;

	QString				m_source;

	QUdpSocket 			*m_sock;

	QSocketNotifier 		*m_notifyNet;

	MDP_PACKET			m_recBuf;
};

#endif

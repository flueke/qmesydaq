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
#include <QSocketNotifier>
#include <QUdpSocket>

#include "networkdevice.h"

NetworkDevice::NetworkDevice(QObject *parent, QString ip, quint16 port)
	: MesydaqObject(parent)
	, m_ipAddress(ip)
	, m_port(port)
	, m_sock(NULL)
	, m_notifyNet(NULL)
{
	createSocket();
}

NetworkDevice::~NetworkDevice()
{
	destroySocket();
}

/*!
 * \fn NetworkDevice::destroySocket();
 *
 * closes UDP connection, used as destructor and for reconnection tasks
 */
void NetworkDevice::destroySocket()
{
	m_notifyNet->setEnabled(false);
	disconnect (m_notifyNet, SIGNAL (activated(int)), this, SLOT (readSocketData()));
	if (m_notifyNet)
		delete m_notifyNet;
	m_notifyNet = NULL;
	if (m_sock)
		delete m_sock;
	m_sock = NULL;
}


/*!
 * \fn NetworkDevice::createSocket(void)
 * 
 * opens a new UDP connection with set parameters for IP address and port, used as
 * constructor and for reconnection tasks
 */
int NetworkDevice::createSocket(void)
{
	protocol("init socket: address " + m_ipAddress, 1);
	m_cpuAddress.setAddress(m_ipAddress);
	
// create server address
	QHostAddress servaddr(QHostAddress::Any);
	if (m_sock)
		delete m_sock;
	m_sock = new QUdpSocket(this);

// bind address
	if (m_sock && m_sock->bind(servaddr, m_port))
	{
		if (m_notifyNet)
		{
			disconnect(m_notifyNet, SIGNAL (activated(int)), this, SLOT (readSocketData()));
			delete m_notifyNet;
		}
// establish socket notifier
		m_notifyNet = new QSocketNotifier(m_sock->socketDescriptor(), QSocketNotifier::Read);
		if (m_notifyNet)
		{
			m_notifyNet->setEnabled(false);
			connect (m_notifyNet, SIGNAL (activated(int)), this, SLOT (readSocketData()));
			m_notifyNet->setEnabled(true);
			return m_sock->socketDescriptor();
		}
	}
	return -1;
}

/*!
 *   \fn NetworkDevice::sendBuffer(MDP_PACKET &buf)
 */
int NetworkDevice::sendBuffer(MDP_PACKET &buf)
{
	protocol("send buffer", 3);
	if (m_sock->writeDatagram((const char *)&buf, 200, m_cpuAddress, m_port) != -1)
		return 1;
	return 0;
}

/*!
 *   \fn NetworkDevice::readSocketData();
 */
void NetworkDevice::readSocketData(void)
{
// read socket data into receive buffer and notify
	if (m_sock->hasPendingDatagrams())
	{
		protocol(tr("NetworkDevice::readSocketData() : %1").arg(ip()), 3);
		qint64 maxsize = m_sock->pendingDatagramSize();
		memset(&m_recBuf, 0, sizeof(m_recBuf));
		qint64 len = m_sock->readDatagram((char *)&m_recBuf, maxsize);
		if (len != -1)
		{
			protocol(tr("read datagram : %1 from %2 bytes").arg(len).arg(maxsize), 3);
			protocol(tr("read nr : %1 cmd : %2 status %3").arg(m_recBuf.bufferNumber).arg(m_recBuf.cmd).arg(m_recBuf.deviceStatus), 3);
			quint64 tim = m_recBuf.time[0] + m_recBuf.time[1] * 0x10000ULL + m_recBuf.time[2] * 0x100000000ULL;
			protocol(tr("read time : %1").arg(tim), 3);
			emit bufferReceived(m_recBuf);
		}
	}
}


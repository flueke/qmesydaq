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
#include "mdefines.h"

/**
 * constructor
 *
 * \param parent parent object
 * \param target IP address of the communication target
 * \param port port number of the communication target
 * \param source IP address of the communication source
 */
NetworkDevice::NetworkDevice(QObject *parent, QString target, quint16 port, QString source)
	: MesydaqObject(parent)
	, m_target(target)
	, m_port(port)
	, m_source(source)
	, m_sock(NULL)
	, m_notifyNet(NULL)
	, m_lastBufnum(0)
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
	if (m_notifyNet)
	{
		m_notifyNet->setEnabled(false);
		disconnect (m_notifyNet, SIGNAL (activated(int)), this, SLOT (readSocketData()));
	}
	delete m_notifyNet;
	m_notifyNet = NULL;
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
	protocol(tr("%1(%2) : init socket: address").arg(m_target).arg(m_port), NOTICE);
	m_cpuAddress.setAddress(m_target);
	
// create server address
	QHostAddress servaddr(m_source); // QHostAddress::Any);
	if (m_sock)
		delete m_sock;
	m_sock = new QUdpSocket(this);

// bind address
	if (m_sock && m_sock->bind(servaddr, m_port, QUdpSocket::DontShareAddress))
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
	protocol(tr("%1(%2) : send buffer").arg(ip()).arg(port()), DEBUG);
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
		protocol(tr("%1(%2) : NetworkDevice::readSocketData()").arg(ip()).arg(port()), DEBUG);
		qint64 maxsize = m_sock->pendingDatagramSize();
		memset(&m_recBuf, 0, sizeof(m_recBuf));
		qint64 len = m_sock->readDatagram((char *)&m_recBuf, maxsize);
		if (len != -1)
		{
			protocol(tr("%1(%2) : read datagram : %3 from %4 bytes").arg(ip()).arg(port()).arg(len).arg(maxsize), DEBUG);
			protocol(tr("%1(%2) : read nr : %3 cmd : %4 status %5").arg(ip()).arg(port()).arg(m_recBuf.bufferNumber).arg(m_recBuf.cmd).arg(m_recBuf.deviceStatus), DEBUG);
			quint64 tim = m_recBuf.time[0] + m_recBuf.time[1] * 0x10000ULL + m_recBuf.time[2] * 0x100000000ULL;
			protocol(tr("%1(%2) : read time : %3").arg(ip()).arg(port()).arg(tim), DEBUG);
			quint16 diff = m_recBuf.bufferNumber - m_lastBufnum;
			if(diff > 1)
				protocol(tr("%1(%2) : Lost %3 Buffers: current: %4, last: %5").arg(ip()).arg(port()).arg(diff).arg(m_recBuf.bufferNumber).arg(m_lastBufnum), ERROR);
			m_lastBufnum = m_recBuf.bufferNumber;
			emit bufferReceived(m_recBuf);
		}
	}
}


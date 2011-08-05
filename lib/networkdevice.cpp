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

QMutex			NetworkDevice::m_mutex(QMutex::Recursive);
QList<NetworkDevice*> 	NetworkDevice::m_networks;
QList<int>		NetworkDevice::m_inUse;

// NetworkDevice	*NetworkDevice::m_instance = NULL;

/*!
    \fn NetworkDevice *NetworkDevice::create(QObject *parent, QString target, quint16 port, QString source)

    factory method for creating a object of the class

    \param parent parent object
    \param source IP address of the communication source
    \param port port number of the communication source
    \return new object of the class
    \see destroy
 */
NetworkDevice *NetworkDevice::create(QObject *parent, QString source, quint16 port)
{
	NetworkDevice *tmp;
	m_mutex.lock();
	for (int i = 0; i < m_networks.size(); ++i)
	{
		tmp = m_networks.at(i);
		if (tmp->ip() == source || tmp->port() == port)
		{
			m_inUse[i]++;
			m_mutex.unlock();
			return tmp;
		}
	}
	tmp = new NetworkDevice(parent, source, port);
	m_networks.push_back(tmp);
	m_inUse.push_back(1);
	m_mutex.unlock();
	return tmp;
}

/*!
    \fn NetworkDevice::destroy(NetworkDevice *nd)

    destroys a object of this class

    \param nd object to destroy
    \see create
 */
void NetworkDevice::destroy(NetworkDevice *nd)
{
	NetworkDevice *tmp;
	m_mutex.lock();
	for (int i = 0; i < m_networks.size(); ++i)
	{
		tmp = m_networks.at(i);
		if (tmp->ip() == nd->ip() || tmp->port() == nd->port())
		{
			m_inUse[i]--;
			if (!m_inUse.at(i))
			{
				m_inUse.takeAt(i);
				tmp = m_networks.takeAt(i);
				delete tmp;
				break;
			}
		}
	}
	m_mutex.unlock();
}

/*!
 * constructor
 *
 * \param parent parent object
 * \param source IP address of the communication source
 * \param port port number of the communication source
 */
NetworkDevice::NetworkDevice(QObject *parent, QString source, quint16 port)
	: MesydaqObject(parent)
	, m_port(port)
	, m_source(source)
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
	protocol(tr("%1(%2) : init socket: address").arg(m_source).arg(m_port), NOTICE);
	
// create server address
	QHostAddress servaddr(m_source); // QHostAddress::Any);
	if (m_sock)
		delete m_sock;
	m_sock = new QUdpSocket(this);

// bind address
	if (m_sock && m_sock->bind(servaddr, m_port, QUdpSocket::/*Dont*/ShareAddress))
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
 *   \fn bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET &buf)
 *   
 *   sends a command packet to the network partner with IP address target
 *
 *   \param target IP address to send
 *   \param buf command packet to send
 */
bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET &buf)
{
	QHostAddress ha(target);
	protocol(tr("%1(%2) : send buffer %3 bytes").arg(ha.toString()).arg(m_port).arg(buf.bufferLength * 2), INFO);
	qint64 i = m_sock->writeDatagram((const char *)&buf, 100, ha, m_port);
	protocol(tr("%1 sent bytes").arg(i), DEBUG);
	return (i != -1); 
}

/*!
 *   \fn bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET2 &buf)
 *   
 *   sends a command packet to the network partner with IP address target
 *
 *   \param target IP address to send
 *   \param buf command packet to send
 */
bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET2 &buf)
{
	QHostAddress ha(target);
	protocol(tr("%1(%2) : send buffer %3 bytes").arg(ha.toString()).arg(m_port).arg(buf.headerlength), INFO);
	qint64 i = m_sock->writeDatagram((const char *)&buf, 200, ha, m_port);
	protocol(tr("%1 sent bytes").arg(i), DEBUG);
	return (i != -1); 
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
			protocol(tr("%1(%2) : ID = %5 read datagram : %3 from %4 bytes").arg(ip()).arg(port()).arg(len).arg(maxsize).arg(m_recBuf.deviceId), DEBUG);
			protocol(tr("%1(%2) : read nr : %3 cmd : %4 status %5").arg(ip()).arg(port()).arg(m_recBuf.bufferNumber).arg(m_recBuf.cmd).arg(m_recBuf.deviceStatus), DEBUG);
			quint64 tim = m_recBuf.time[0] + m_recBuf.time[1] * 0x10000ULL + m_recBuf.time[2] * 0x100000000ULL;
			protocol(tr("%1(%2) : read time : %3").arg(ip()).arg(port()).arg(tim), DEBUG);

			emit bufferReceived(m_recBuf);
		}
	}
}


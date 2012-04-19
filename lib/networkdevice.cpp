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
#include <QThread>
#include "networkdevice.h"
#include "mdefines.h"
#include "logging.h"

QMutex			NetworkDevice::m_mutex(QMutex::Recursive);
QList<NetworkDevice*> 	NetworkDevice::m_networks;
QList<int>		NetworkDevice::m_inUse;

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
                if (tmp->ip() == source && tmp->port() == port)
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
	for (int i=m_networks.size()-1; i>=0; --i)
	{
		tmp = m_networks.at(i);
                if (tmp->ip() == nd->ip() && tmp->port() == nd->port())
		{
			Q_ASSERT(m_inUse[i]>0);
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
	: QObject(parent)
	, m_pThread(NULL)
	, m_bFlag(false)
	, m_port(port)
	, m_source(source)
	, m_sock(NULL)
	, m_notifyNet(NULL)
{
  Q_ASSERT(parent==NULL);
  if (QMetaType::type("MDP_PACKET")==0)
    qRegisterMetaType<MDP_PACKET>();
  m_pThread=new QThread;
  moveToThread(m_pThread);
  connect(m_pThread,SIGNAL(started()),this,SLOT(createSocket()),Qt::DirectConnection);
  connect(m_pThread,SIGNAL(finished()),this,SLOT(destroySocket()),Qt::DirectConnection);
  m_pThread->start(QThread::HighestPriority);
  while (!m_bFlag)
    usleep(10000);
}

/*!
    destructor
 */
NetworkDevice::~NetworkDevice()
{
  m_pThread->quit();
  m_pThread->wait();
  delete m_pThread;
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
	m_bFlag=false;
}


/*!
 * \fn NetworkDevice::createSocket(void)
 * 
 * opens a new UDP connection with set parameters for IP address and port, used as
 * constructor and for reconnection tasks
 */
int NetworkDevice::createSocket(void)
{
	MSG_NOTICE << m_source.toLocal8Bit().constData() << '(' << m_port << ") : init socket: address";
	
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
			m_bFlag=true;
			return m_sock->socketDescriptor();
		}
	}
	m_bFlag=true;
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
	MSG_INFO << ha.toString().toLocal8Bit().constData() << '(' << m_port << ") : send buffer " << buf.bufferLength * 2 << " bytes";
	qint64 i = m_sock->writeDatagram((const char *)&buf, 100, ha, m_port);
	MSG_DEBUG << i << " sent bytes";
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
	MSG_INFO << ha.toString().toLocal8Bit().constData() << '(' << m_port << ") : send buffer " << buf.headerlength << " bytes";
	qint64 i = m_sock->writeDatagram((const char *)&buf, 200, ha, m_port);
	MSG_DEBUG << i << " sent bytes";
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
		MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : NetworkDevice::readSocketData()";
		qint64 maxsize = m_sock->pendingDatagramSize();
		memset(&m_recBuf, 0, sizeof(m_recBuf));
		qint64 len = m_sock->readDatagram((char *)&m_recBuf, maxsize);
		if (len != -1)
		{
			MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : ID = " << m_recBuf.deviceId << " read datagram : " << len << " from " << maxsize << " bytes";
			MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : read nr : " << m_recBuf.bufferNumber << " cmd : " << m_recBuf.cmd << " status " << m_recBuf.deviceStatus;
			quint64 tim = m_recBuf.time[0] + m_recBuf.time[1] * 0x10000ULL + m_recBuf.time[2] * 0x100000000ULL;
			MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : read time : " << tim;

			emit bufferReceived(m_recBuf);
		}
	}
}

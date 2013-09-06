/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#include "stdafx.h"

QMutex			NetworkDevice::m_mutex(QMutex::Recursive);
QList<NetworkDevice *> 	NetworkDevice::m_aNetworks;
QList<int>		NetworkDevice::m_inUse;

/*!
    \fn NetworkDevice *NetworkDevice::create(quint16 wPort = 54321, QString szHostIp = QString("0.0.0.0"))

    factory method for creating a object of the class

    \param wPort    UDP port number of the communication source
    \param szHostIp host interface to bind to
    \return new object of the class
    \see destroy
 */
NetworkDevice *NetworkDevice::create(quint16 wPort /*= 54321*/, QString szHostIp /*= QString("0.0.0.0")*/)
{
    QMutexLocker locker(&m_mutex);
    NetworkDevice *tmp;
    for (int i = 0; i < m_aNetworks.size(); ++i)
    {
        tmp = m_aNetworks.at(i);
        if (tmp->ip() == szHostIp && tmp->port() == wPort)
        {
            m_inUse[i]++;
            return tmp;
        }
    }
    tmp = new NetworkDevice(wPort, szHostIp);
    m_aNetworks.push_back(tmp);
    m_inUse.push_back(1);
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
    QMutexLocker locker(&m_mutex);
    for (int i = m_aNetworks.size() - 1; i >= 0; --i)
    {
        NetworkDevice *tmp = m_aNetworks.at(i);
        if (tmp->ip() == nd->ip() && tmp->port() == nd->port())
        {
            Q_ASSERT(m_inUse[i] > 0);
            m_inUse[i]--;
            if (!m_inUse.at(i))
            {
                m_inUse.takeAt(i);
                tmp = m_aNetworks.takeAt(i);
                delete tmp;
                break;
            }
        }
    }
}

/*!
 * \fn NetworkDevice::NetworkDevice(quint16 wPort = 54321, QString szHostIp = QString("0.0.0.0"))
 * constructor
 *
 * \param wPort      host UDP port to bind to
 * \param szHostIp   host IP address to bind to
 */
NetworkDevice::NetworkDevice(quint16 wPort /*= 54321*/, QString szHostIp /*= QString("0.0.0.0")*/)
    : m_pThread(NULL)
    , m_bFlag(false)
    , m_szHostIp(szHostIp)
    , m_wPort(wPort)
    , m_sock(NULL)
    , m_notifyNet(NULL)
{
    m_pThread = new QThread;
    moveToThread(m_pThread);
    m_pThread->start(QThread::HighestPriority);
    connect(m_pThread, SIGNAL(started()), this, SLOT(createSocket()), Qt::DirectConnection);
    connect(m_pThread, SIGNAL(finished()), this, SLOT(destroySocket()), Qt::DirectConnection);
    while (!m_bFlag)
        usleep(10000);
}

/*!
    destructor
 */
NetworkDevice::~NetworkDevice()
{
    if (m_pThread)
    {
        m_pThread->quit();
        m_pThread->wait();
        delete m_pThread;
    }
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
        disconnect(m_notifyNet, SIGNAL(activated(int)), this, SLOT(readSocketData()));
    }
    delete m_notifyNet;
    m_notifyNet = NULL;
    delete m_sock;
    m_sock = NULL;
    m_bFlag = false;
}


/*!
 * \fn NetworkDevice::createSocket(void)
 *
 * opens a new UDP connection with set parameters for IP address and port, used as
 * constructor and for reconnection tasks
 */
int NetworkDevice::createSocket(void)
{
    MSG_NOTICE << m_szHostIp.toLocal8Bit().constData() << '(' << m_wPort << ") : init socket: address";

    // create server address
    QHostAddress servaddr(m_szHostIp); // QHostAddress::Any);
    if (m_sock)
        delete m_sock;
    m_sock = new QUdpSocket(this);

    // bind address
    if (m_sock && m_sock->bind(servaddr, m_wPort, QUdpSocket::/*Dont*/ShareAddress))
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
            connect(m_notifyNet, SIGNAL (activated(int)), this, SLOT (readSocketData()));
            m_notifyNet->setEnabled(true);
            m_bFlag = true;
            return m_sock->socketDescriptor();
        }
    }
    m_bFlag = true;
    return -1;
}

/** \fn bool NetworkDevice::connect_handler(QHostAddress source, quint16 wPort, NetworkDevice::analyzeBufferFunction pFunction, void* pParam)
 *
 *  connect a new data packet handler for a MCPD
 *
 *  \param source     source address of MCPD
 *  \param wPort      source port of MCPD
 *  \param pFunction  pointer to data handler
 *  \param pParam     the data handler will be called with this pointer
 */
bool NetworkDevice::connect_handler(QHostAddress source, quint16 wPort, NetworkDevice::analyzeBufferFunction pFunction, void* pParam)
{
    if (source == QHostAddress::Any || source == QHostAddress::AnyIPv6)
        source.clear();
    if ((source.isNull() && wPort == 0) || pFunction == NULL)
        return false;

    QMutexLocker locker(&m_mutex);
    foreach(const struct handler &tmp, m_aHandler)
    {
        if ((tmp.SourceAddr == source || source.isNull())
	    && (tmp.wPort == wPort || wPort == 0)
	    && tmp.pFunction == pFunction)
            return false;
    }

    struct handler h = {source, wPort, pFunction, pParam};
#if 0
    h.SourceAddr = source;
    h.wPort = wPort;
    h.pFunction = pFunction;
    h.pParam = pParam;
#endif
    m_aHandler.append(h);
    return true;
}

/** \fn void NetworkDevice::disconnect_handler(QHostAddress source, quint16 wPort, NetworkDevice::analyzeBufferFunction pFunction)
 *
 *  disconnect packet handler for a MCPD
 *
 *  \param source     source address of MCPD
 *  \param wPort      source port of MCPD
 *  \param pFunction  pointer to data handler
 */
void NetworkDevice::disconnect_handler(QHostAddress source, quint16 wPort, NetworkDevice::analyzeBufferFunction pFunction)
{
    if (source == QHostAddress::Any || source == QHostAddress::AnyIPv6)
        source.clear();
    if (pFunction == NULL && source.isNull())
        return;

    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_aHandler.count(); ++i)
    {
        struct handler &tmp = m_aHandler[i];
        if ((tmp.SourceAddr == source || source.isNull())
	   && (tmp.wPort == wPort || wPort == 0)
	   && (tmp.pFunction == pFunction || pFunction == NULL))
            m_aHandler.removeAt(i--);
    }
}

/*!
 *   \fn bool NetworkDevice::sendBuffer(const QString &target, quint16 wPort, const QSharedDataPointer<SD_PACKET> &buf)
 *
 *   sends a command packet to the network partner with IP address target
 *
 *   \param szTargetIp IP address to send
 *   \param wPort      UDP port to send
 *   \param buf        command packet to send
 */
bool NetworkDevice::sendBuffer(const QString &szTargetIp, quint16 wPort, const QSharedDataPointer<SD_PACKET> &buf)
{
    QHostAddress ha(szTargetIp);
    MSG_INFO << ha.toString().toLocal8Bit().constData() << '(' << wPort << ") : send buffer " << buf->mdp.bufferLength * 2 << " bytes";
    qint64 i = m_sock->writeDatagram((const char *)(&buf->mdp), 100, ha, wPort);
    MSG_DEBUG << i << " sent bytes";
    return (i != -1);
}

/*!
 *   \fn bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET &buf)
 *
 *   sends a command packet to the network partner with IP address target
 *
 *   \param szTargetIp IP address to send
 *   \param wPort      UDP port to send
 *   \param buf        command packet to send
 */
bool NetworkDevice::sendBuffer(const QString &szTargetIp, quint16 wPort, const MDP_PACKET &buf)
{
    QHostAddress ha(szTargetIp);
    MSG_INFO << ha.toString().toLocal8Bit().constData() << '(' << wPort << ") : send buffer " << buf.bufferLength * 2 << " bytes";
    qint64 i = m_sock->writeDatagram((const char *)&buf, 100, ha, wPort);
    MSG_DEBUG << i << " sent bytes";
    return (i != -1);
}

/*!
 *   \fn bool NetworkDevice::sendBuffer(const QString &target, const MDP_PACKET2 &buf)
 *
 *   sends a command packet to the network partner with IP address target
 *
 *   \param szTargetIp IP address to send
 *   \param wPort      UDP port to send
 *   \param buf        command packet to send
 */
bool NetworkDevice::sendBuffer(const QString &szTargetIp, quint16 wPort, const MDP_PACKET2 &buf)
{
    QHostAddress ha(szTargetIp);
    MSG_INFO << ha.toString().toLocal8Bit().constData() << '(' << wPort << ") : send buffer " << buf.headerlength << " bytes";
    qint64 i = m_sock->writeDatagram((const char *)&buf, 200, ha, wPort);
    MSG_DEBUG << i << " sent bytes";
    return (i != -1);
}

/*!
 *   \fn NetworkDevice::readSocketData();
 */
void NetworkDevice::readSocketData(void)
{
    bool bLostPacket(false);
    // read socket data into receive buffer and notify
    if (m_sock->hasPendingDatagrams())
    {
//      MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : NetworkDevice::readSocketData()";
        qint64 maxsize = m_sock->pendingDatagramSize();
        if (maxsize > 0)
        {
            QSharedDataPointer<SD_PACKET> recBuf(new SD_PACKET);
            QHostAddress fromAddress;
            quint16	 fromPort;
            qint64 len = m_sock->readDatagram((char *)(&recBuf->mdp), maxsize, &fromAddress, &fromPort);
            bLostPacket=(len>0);
            if (len != -1)
            {
                // MSG_DEBUG << fromAddress.toString().toLocal8Bit().constData() << '(' << fromPort << ") : ID = " << recBuf.deviceId << " read datagram : " << len << " from " << maxsize << " bytes";
                // MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : read nr : " << recBuf.bufferNumber << " cmd : " << recBuf.cmd << " status " << recBuf.deviceStatus;
                // quint64 tim = recBuf.time[0] + recBuf.time[1] * 0x10000ULL + recBuf.time[2] * 0x100000000ULL;
                // MSG_DEBUG << ip().toLocal8Bit().constData() << '(' << port() << ") : read time : " << tim;

                foreach (const struct handler &tmp, m_aHandler)
                {
                    if ((tmp.SourceAddr.isNull() || tmp.SourceAddr == fromAddress)
			&& (tmp.wPort == 0 || tmp.wPort == fromPort))
                    {
                        bLostPacket = false;
                        (tmp.pFunction)(recBuf, tmp.pParam);
                    }
                }
            }
            if (bLostPacket)
                MSG_DEBUG << "lost " << len << " bytes from " << fromAddress << ", port " << fromPort << ": " << HexDump(&recBuf->mdp, len);
        }
    }
}

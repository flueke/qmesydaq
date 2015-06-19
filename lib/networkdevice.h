/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2015 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <QObject>
#include <QMutex>
#include <QHostAddress>

#include "libqmesydaq_global.h"
#include "structures.h"

class QUdpSocket;
class QSocketNotifier;

/**
 * \short Base class for network devices like MCPD-2, MCPD-8
 *
 * The objects of this class will be created and destroyed via a factory.
 *
 * There should be an instance for every UDP port (with interface) on the host computer.
 * Every instance has a listener thread waiting for incoming packets. You need to use
 * "connect_handler" to receive packets. You can use "sendBuffer" to send a packet to
 * a specific target.
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>, Lutz Rossa <rossa@helmholtz-berlin.de>
*/
class LIBQMESYDAQ_EXPORT NetworkDevice : public QObject
{
Q_OBJECT
public:
	typedef void (*analyzeBufferFunction)(QSharedDataPointer<SD_PACKET> pPacket, void* pParam);

	static NetworkDevice *create(quint16 wPort = 54321, QString szHostIp = QString("0.0.0.0"));

	static void destroy(NetworkDevice *);

private:
	NetworkDevice(quint16 wPort = 54321, QString szHostIp = QString("0.0.0.0"));
	~NetworkDevice();

public:
	bool connect_handler(QHostAddress source, quint16 wPort, analyzeBufferFunction pFunction, void *pParam);
	void disconnect_handler(QHostAddress source, quint16 wPort, analyzeBufferFunction pFunction);

	bool sendBuffer(const QString &szTargetIp, quint16 wPort, const QSharedDataPointer<SD_PACKET> &buf);
	bool sendBuffer(const QString &szTargetIp, quint16 wPort, const MDP_PACKET &packet);
	bool sendBuffer(const QString &szTargetIp, quint16 wPort, const MDP_PACKET2 &packet);

	//! \return IP address of the target
	QString ip() const;

	//! \return port number of the communication target
	quint16 port() const;

public slots:
	//! handles the action if some data reach the socket for incoming data
	void readSocketData(void);

signals:
	//! This signal is emitted if a complete data or command packet has read.
	void bufferReceived(const MDP_PACKET &);

private slots:
	int createSocket(void);

	void destroySocket(void);

private:
	struct handler
	{
		QHostAddress SourceAddr;
		quint16 wPort;
		analyzeBufferFunction pFunction;
		void *pParam;
	};

	static QMutex                m_mutex;

	static QList<NetworkDevice*> m_aNetworks;

	static QList<int>            m_inUse;

	QThread                      *m_pThread;

	bool                         m_bFlag;

	QString                      m_szHostIp;

	quint16                      m_wPort;

	QUdpSocket                   *m_sock;

	QSocketNotifier              *m_notifyNet;

	//! all packet handlers
	QList<struct handler>        m_aHandler;
};

#endif

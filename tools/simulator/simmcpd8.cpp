/***************************************************************************
 *   Copyright (C) 2013 by Lutz Rossa <rossa@helmholtz-berlin.de>          *
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
#include "main.h"
#include "simmcpd8.h"
#include "logging.h"

/////////////////////////////////////////////////////////////////////////////
// SimMCPD8::SimMCPD8(QObject *parent, quint8 id, const QHostAddress &address, quint16 port)
//
// constructor
SimMCPD8::SimMCPD8(quint8 id, const QHostAddress &address, quint16 port) :
	m_pSocket(NULL),
	m_DataTarget(QHostAddress::Broadcast),
	m_wDataPort(54321),
	m_byCpdId(id),
	m_wBufferNo(0)
{
	m_pSocket = new QUdpSocket(this);
	if (m_pSocket->bind(address, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint))
		connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::QueuedConnection);
	else
	{
		delete m_pSocket;
		m_pSocket = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// SimMCPD8::~SimMCPD8()
//
// destructor
SimMCPD8::~SimMCPD8()
{
    	if (m_pSocket != NULL)
		disconnect(m_pSocket);
}

/////////////////////////////////////////////////////////////////////////////
// SimMCPD8::QString ip() const
//
// return MCPD8 ip address
QString SimMCPD8::ip() const
{
	return m_pSocket->localAddress().toString();
}

/////////////////////////////////////////////////////////////////////////////
// quint8 SimMCPD8::id() const
//
// return MCPD8 id
quint8 SimMCPD8::id() const
{
	return m_byCpdId;
}

/////////////////////////////////////////////////////////////////////////////
// void SimMCPD8::id(quint8 id)
//
// set new MCPD8 id
void SimMCPD8::id(quint8 id)
{
	m_byCpdId = id;
}

/////////////////////////////////////////////////////////////////////////////
// quint16 SimMCPD8::NextBufferNo()
//
// return data buffer number and increment for next buffer
quint16 SimMCPD8::NextBufferNo()
{
	return m_wBufferNo++;
}

/////////////////////////////////////////////////////////////////////////////
// void SimMCPD8::SetTarget(const QHostAddress &ip, const quint16 wPort)
//
// set data packet target
void SimMCPD8::SetTarget(const QHostAddress &ip, const quint16 wPort)
{
	m_DataTarget = ip;
	m_wDataPort = wPort;
}

/////////////////////////////////////////////////////////////////////////////
// quint16 SimMCPD8::CalcCRC(const struct MDP_PACKET *pPacket)
//
// calculate CRC of command packets
quint16 SimMCPD8::CalcCRC(const struct MDP_PACKET *pPacket)
{
	const quint16 *p = reinterpret_cast<const quint16*>(pPacket);
	quint16 chksum = pPacket->headerChksum;
	for (quint32 i = 0; i < pPacket->bufferLength; ++i)
		chksum ^= p[i];
	return chksum;
}

/////////////////////////////////////////////////////////////////////////////
// void SimMCPD8::readyRead()
//
// read command packets
void SimMCPD8::readyRead()
{
	while (m_pSocket->hasPendingDatagrams())
	{
		QHostAddress sender;
		quint16 senderPort;
		QByteArray datagram;
		struct MDP_PACKET *pPacket;
		QString szText;

		datagram.resize(m_pSocket->pendingDatagramSize());
		m_pSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		pPacket = (struct MDP_PACKET*)datagram.data();
		if (datagram.size() >= (int)(sizeof(*pPacket) - sizeof(pPacket->data)) &&
			datagram.size() >= (int)pPacket->bufferLength)
		{
			if (pPacket->deviceId != m_byCpdId)
				szText.sprintf("ignoring different MCPD id (mcpd=%d, other=%d)", m_byCpdId, pPacket->deviceId);
			else if (CalcCRC(pPacket) != pPacket->headerChksum)
				szText.sprintf("invalid CRC 0x%04x 0x%04x", CalcCRC(pPacket), pPacket->headerChksum);
			else
			{
				emit CmdPacket(pPacket, this, sender, senderPort);
				return;
			}
		}
		else
			szText="unknown packet";

		QString s(HexDump(datagram.constData(), datagram.size()));
		logmsg(NULL, tr("%1: %2 data bytes from %3:%4 - %5").arg(szText).arg(datagram.size()).arg(sender.toString()).arg(senderPort).arg(s));
	}
}

/////////////////////////////////////////////////////////////////////////////
// void SimMCPD8::Send(struct MDP_PACKET *pPacket, const QHostAddress &address, const quint16 wPort)
//
// send data packet
void SimMCPD8::Send(struct MDP_PACKET *pPacket, const QHostAddress &address, const quint16 wPort)
{
	qint64 qi;
	pPacket->headerChksum = 0;
	pPacket->headerChksum = CalcCRC(pPacket);
	qi = pPacket->bufferLength;
	if (qi < 100)
		qi = 100;
	m_pSocket->writeDatagram((const char*)pPacket, qi, address, wPort);
}

/////////////////////////////////////////////////////////////////////////////
// void SimMCPD8::Send(struct DATA_PACKET *pPacket)
//
// send data packet
void SimMCPD8::Send(struct DATA_PACKET *pPacket)
{
	m_pSocket->writeDatagram((const char*)pPacket, 2 * pPacket->bufferLength, m_DataTarget, m_wDataPort);
}

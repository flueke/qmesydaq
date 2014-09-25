/***************************************************************************
 *   Copyright (C) 2013-2014 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
#ifndef __SIMMCPD8_H__56F81A51_FFDD_436A_A346_6103FF34B893__
#define __SIMMCPD8_H__56F81A51_FFDD_436A_A346_6103FF34B893__

#include <QUdpSocket>
#include <QVector>
#include "structures.h"
#include "mdefines.h"

class SimMCPD8 : public QObject
{
	Q_OBJECT
public:
	explicit SimMCPD8(quint8 id, const QHostAddress &address, quint16 port = 54321);
	virtual ~SimMCPD8();

	QString ip() const;
	quint8 id() const;
	void id(quint8 id);
	quint16 NextBufferNo();
	void SetTarget(const QHostAddress &ip, const quint16 wPort);
	void Send(struct MDP_PACKET *pPacket, const QHostAddress &address, const quint16 wPort);
	void Send(struct DATA_PACKET *pPacket);

private slots:
	void readyRead();

signals:
	void CmdPacket(struct MDP_PACKET *pPacket, SimMCPD8 *pMCPD8, QHostAddress &sender, quint16 &senderPort);

protected:
	static quint16 CalcCRC(const struct MDP_PACKET *pPacket);

protected:
	QUdpSocket	*m_pSocket;   // command/data udp socket
	QHostAddress    m_DataTarget; // data target udp address
	quint16         m_wDataPort;  // data target udp port
	quint8          m_byCpdId;    // MCPD id
	quint16         m_wBufferNo;  // data packet counter
};

#endif /*__SIMMCPD8_H__56F81A51_FFDD_436A_A346_6103FF34B893__*/

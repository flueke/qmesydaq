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
#ifndef __SIMAPP_H__
#define __SIMAPP_H__

#include <QCoreApplication>
#include <QHostAddress>
#include "structures.h"

class QTimerEvent;
class SimMCPD8;

class SimApp : public QCoreApplication
{
	Q_OBJECT
public:
	SimApp(int &argc, char **argv);

public slots:
	//! simulate new data packets
	void timerEvent(QTimerEvent *);

	//! MCPD8 command packet handler
	void NewCmdPacket(struct MDP_PACKET *pPacket, SimMCPD8 *pMCPD8, QHostAddress &sender, quint16 &senderPort);

private:
	//! generate a simulated spectrum
	void ComputeSpectrum(void);

	//! generate index array of points
	void GeneratePoints(void);

	//! called for start or stop commands
	void StartStop(SimMCPD8 *pMCPD8, bool bDAQ, const char *szReason);

	//! read more accurate clock
	quint64 GetClock(void);

private:
	//! default: MCPD has 64 channels (8 full MPSDs)
	quint16            m_wSpectrumWidth;

	//! spectrum height
	quint16            m_wSpectrumHeight;

	//! stop after this number of packets
	quint16            m_dwStopPacket;

	//! simulation timer
	quint16            m_wTimerInterval;

	//! simulate "round" detector of HZB instrument V4/SANS
	bool               m_bV4;

	QVector<SimMCPD8*> m_apMCPD8;

	//! simulator is running
	bool               m_bDAQ;

	//! header run#
	quint16            m_wRunId;

	//! start time
	quint64            m_qwMasterOffset;

	//! packet counter
	quint32            m_dwPackets;

	//! loop counter for full simulation cycles
	quint64            m_qwLoopCount;

	// output spectrum
	QVector<quint8>    m_abySpectrum;

	// point to simulate
	QVector<int>       m_aiPoints;

	// index into m_aiPoints for faster simulation (if >= 0)
	int                m_iFastPoint;

	// number of send events
	quint32            m_dwSendEvents;
};

#endif /* __SIMAPP_H__ */

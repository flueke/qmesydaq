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
#ifndef MCPD8_H
#define MCPD8_H

#include <QString>
#include <QDataStream>
#include <QMap>

#include "structures.h"
#include "mesydaqobject.h"

class NetworkDevice;
class MPSD_8;
class QTimer;

/**
 * representation of mcpd-8 central module
 *
 * @author Gregor Montermann <g.montermann@mesytec.com>
*/
class MCPD8 : public MesydaqObject
{
Q_OBJECT
public:
	MCPD8(quint8, QObject *parent = 0, QString = "192.168.168.121", quint16 = 54321);

	~MCPD8();

// commands of the MPCD-8
// commands: DAQ commands
	bool reset(void);

	bool start(void);

	bool stop(void);

	bool cont(void);

// commands: communication settings
	bool setId(quint8 mcpdid);

	bool readId(void);

	quint8 getId(void) { return m_id; }

	bool setProtocol(const QString addr, const QString datasink = "0.0.0.0", const quint16 dataport = 0, const QString cmdsink = "0.0.0.0", const quint16 cmdport = 0);

	void getProtocol(quint16 *addr);

	bool setTimingSetup(bool master, bool sync);

	bool isMaster(void) {return m_master;}

	bool setMasterClock(quint64);

	quint64 receivedData() {return m_dataRxd;} 

	quint64 receivedCmds() {return m_cmdRxd;} 

	quint64 sentCmds() {return m_cmdTxd;} 

// commands: General MCPD-8 settings
	bool setCounterCell(quint16 source, quint16 trigger, quint16 compare);

	void getCounterCell(quint8 cell, quint16 *celldata);

	bool setAuxTimer(quint16 tim, quint16 val);

	quint16 getAuxTimer(quint16 timer);

	bool setParamSource(quint16 param, quint16 source);

	quint16 getParamSource(quint16 param);

	bool setParameter(quint16 param, quint64 val);

	quint64 getParameter(quint16 param);

// commands: MPSD-8 settings
	bool setGain(quint16 addr, quint8 channel, quint8 gain);

	bool setGain(quint16 addr, quint8 channel, float gain);

	quint8	getGain(quint16 addr, quint8 chan);

	bool setThreshold(quint16 addr, quint8 thresh);

	quint8 getThreshold(quint16 addr);

	bool setPulser(quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff);

	bool setMode(quint16 addr, bool mode);

	bool getMode(quint16 addr);

	quint8 getMpsdId(quint8 addr);

	void initMpsd(quint8 id);

// commands: MCPD-8 ports
	bool setDac(quint16 dac, quint16 val);

	bool sendSerialString(QString str);

	QString readSerialString(void);

// commands:
	bool readPeriReg(quint16 mod, quint16 reg);

	bool writePeriReg(quint16 mod, quint16 reg, quint16 val);

	bool writeRegister(quint16 addr, quint16 reg, quint16 val);

	bool getVersion(void);

	float version(void);

public:
	void communicate(bool yesno) {m_commActive = yesno;}

	bool init(void);

	bool setStream(quint16 strm);

	bool getStream(void) {return m_stream;}

	bool serialize(QDataStream ds);

	bool isBusy(void) {return m_commActive;}

	bool isPulserOn();

	bool isPulserOn(quint8 addr);

	quint8	getPulsPos(quint8 addr, bool preset = false);

	quint8	getPulsAmp(quint8 addr, bool preset = false); 

	quint8	getPulsChan(quint8 addr, bool preset = false); 

	quint64 time(void) {return m_timemsec;}

	bool setRunId(quint16 runid);

	quint16 getRunId(void) {return m_runId;}

public slots:
	void analyzeBuffer(MDP_PACKET &pd);

	void commTimeout(void);

signals:
	void startedDaq(void);

	void stoppedDaq(void);

	void continuedDaq(void);

	void analyzeDataBuffer(DATA_PACKET &pd);

private:
	void initCmdBuffer(quint16);

	void finishCmdBuffer(quint16 buflen);

	int sendCommand(void);

	quint16 calcChksum(MDP_PACKET &buffer);

	int sendCommand(quint16 *buffer);

	void stdInit(void);

private:
	//! communication device
	NetworkDevice	*m_network;
	
	//! counter for the send command buffer packets
	quint16		m_txCmdBufNum;

	//! ID of the MCPD
	quint8 		m_id;

// communication params
	//! IP address of the module
	QString 	m_ownIpAddress;

	//! IP address for sending the commands
	QString 	m_cmdIpAddress;
	//! Port for commands
	quint16 	m_cmdPort;

	//! IP address for sending the data
	QString 	m_dataIpAddress;
	//! Port for the data
	quint16 	m_dataPort;
	
	//! is this MCPD master or not
	bool 		m_master;
	
	//! 8 counter cells, trig source in [0], compare reg in [1]
	quint8 		m_counterCell[8][2];

	//! four auxiliary timers, capture values
	quint16 	m_auxTimer[4];

	//! four parameters (transmitted in buffer header), 9 possible sources
	quint8 		m_paramSource[4];
	
	bool 		m_stream;
	quint64 	m_parameter[4];
	bool 		m_commActive;

	MDP_PACKET      m_cmdBuf;

	quint32 	m_lastBufnum;

	QTimer 		*m_commTimer;

	//! current run ID
	quint16 	m_runId;

	//! DAQ started?
	bool		m_daq;

	//! counter for the received data packets
	quint32		m_dataRxd;

	//! counter for the sent cmd packets
	quint32		m_cmdTxd;

	//! counter for the receivcd cmd packets
	quint32		m_cmdRxd;

public:
#warning TODO remove the public access of m_mpsd
	//! the accessed MPSD8 ????
	QMap<int, MPSD_8 *> m_mpsd;
	
private:
	//! the header time stamp
	quint64		m_headertime;

	//! the header time in ms
	quint64		m_timemsec;
};

#endif


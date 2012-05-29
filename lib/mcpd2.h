/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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
#ifndef MCPD2_H
#define MCPD2_H

#include <QObject>
#include <QString>
#include <QMap>

#include "libqmesydaq_global.h"
#include "structures.h"

class NetworkDevice;
class MPSD8;
class QTimer;

/**
 * \short representation of MCPD-8 central module
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
*/
class LIBQMESYDAQ_EXPORT MCPD2 : public QObject
{
Q_OBJECT
public:
	MCPD2(quint8, QObject *parent = 0, QString = "192.168.168.121", quint16 = 54321, QString = "0.0.0.0");

	~MCPD2();

// commands of the MPCD-8
// commands: DAQ commands
	bool reset(void);

	bool start(void);

	bool stop(void);

	bool cont(void);

// commands: communication settings
	bool setId(quint8 mcpdid);

	bool readId(void);

	//! \return the ID of this MCPD
	quint8 getId(void) { return m_id; }

	bool setProtocol(const QString& addr, const QString& datasink = QString("0.0.0.0"), const quint16 dataport = 0, const QString& cmdsink = QString("0.0.0.0"), const quint16 cmdport = 0);

	void getProtocol(quint16 *addr);

	bool setTimingSetup(bool master, bool term);

	//! \return whether this MCPD is configured as master or not
	bool isMaster(void) {return m_master;}

	//! \return whether this MCPD is terminated on the synchronization bus or not
	bool isTerminated(void) {return m_term;}

	bool setMasterClock(quint64);

	//! returns the number of received data packages
	quint64 receivedData() {return m_dataRxd;} 

	//! returns the number of received cmd answer packages
	quint64 receivedCmds() {return m_cmdRxd;} 

	//! returns the number of sent cmd packages 
	quint64 sentCmds() {return m_cmdTxd;} 

// commands: General MCPD-8 settings
	bool setCounterCell(quint16 source, quint16 trigger, quint16 compare);

	void getCounterCell(quint8 cell, quint16 *celldata);

	bool setAuxTimer(quint16 timer, quint16 val);

	quint16 getAuxTimer(quint16 timer);

	bool setParamSource(quint16 param, quint16 source);

	quint16 getParamSource(quint16 param);

	bool setParameter(quint16 param, quint64 val);

	quint64 getParameter(quint16 param);

// commands: MPSD-8 settings
	bool setGain(quint16 addr, quint8 channel, quint8 gain);

	bool setGain(quint16 addr, quint8 channel, float gain);

	quint8	getGainPoti(quint16 addr, quint8 chan);

	float	getGainVal(quint16 addr, quint8 chan);

	bool setThreshold(quint16 addr, quint8 thresh);

	//! \todo implementation 
	quint8 getThresholdPoti(quint16 addr);

	quint8 getThreshold(quint16 addr);

	bool setPulser(quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff);

	bool isPulserOn();

	bool isPulserOn(quint8 addr);

	quint8	getPulsPos(quint8 addr, bool preset = false);

	quint8	getPulsAmp(quint8 addr, bool preset = false); 

	quint8	getPulsChan(quint8 addr, bool preset = false); 

	bool setMode(quint16 addr, bool mode);

	bool getMode(quint16 addr);

	quint8 getModuleId(quint8 addr);

	void initModule(quint8 id);

// commands: MCPD-8 ports
	bool setDac(quint16 dac, quint16 val);

	bool sendSerialString(QString str);

	//! \todo implement me
	QString readSerialString(void);

// commands:
	quint16 readPeriReg(quint16 mod, quint16 reg);

	bool writePeriReg(quint16 mod, quint16 reg, quint16 val);

	bool writeRegister(quint16 reg, quint16 val);

	quint16 readRegister(quint16 reg);

	float version(void); 

	float version(quint16);

	//! \return number of modules found
	quint8 numModules(void) {return m_mpsd.size();}

	bool init(void);

	bool setStream(quint16 strm);

	quint16 capabilities();

	quint16 capabilities(quint16 mod);

	/**
	 * ????
	 *
	 * \return ???
	 * \see setStream
	 */
	bool getStream(void) {return m_stream;}

//	bool serialize(QDataStream ds);

	//! \return the current time of the MCPD in msec
	quint64 time(void) {return m_timemsec;}

	bool setRunId(quint16 runid);

	/**
	 * gets the current set run ID
	 *
	 * \return run ID
	 * \see setRunId
	 */
	quint16 getRunId(void) {return m_runId;}

	/**
	 * gets the address of the MCPD module
	 *
	 * \return IP address of the MCPD
	 */
	QString ip(void) {return m_ownIpAddress;}

	/**
	 * gets the command port of the MCPD
	 *
	 * \return port number of the command port
	 */
	quint16 port(void) {return m_cmdPort;}

public slots:
	//! analyse network packet
	void analyzeBuffer(MDP_PACKET pd);

private slots:
	//! callback for the communication timer to detect a timeout
	void commTimeout(void);

signals:
	//! this will be emitted if the MCPD-8 was started 
	void startedDaq(void);

	//! this will be emitted if the MCPD-8 was stopped 
	void stoppedDaq(void);

	//! this will be emitted if the MCPD-8 was continued 
	void continuedDaq(void);

	/**
	 * this will be emitted if the MCPD-8 has sent a new data packet
	 *
	 * \param pd data packet
	 */
	void analyzeDataBuffer(DATA_PACKET pd);

private:
	void communicate(bool yesno) {m_commActive = yesno;}

	void initCmdBuffer(quint8);

	void finishCmdBuffer(quint16 buflen);

	int sendCommand(bool = true);

	quint8 calcChksum(const MDP_PACKET2 &buffer);

	int sendCommand(quint16 *buffer);

	void stdInit(void);

	bool isBusy(void) {return m_commActive;}

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
	
	//! is this MCPD master on sync bus or not
	bool 		m_master;
	
	//! is this MCPD terminated on sync bus or not
	bool 		m_term;
	
	//! 8 counter cells, trig source in [0], compare reg in [1]
	quint8 		m_counterCell[8][2];

	//! four auxiliary timers, capture values
	quint16 	m_auxTimer[4];

	//! four parameters (transmitted in buffer header), 9 possible sources
	quint8 		m_paramSource[4];
	
	bool 		m_stream;
	quint64 	m_parameter[4];
	bool 		m_commActive;

	MDP_PACKET2	m_cmdBuf;

	//! last buffer number
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

	//! the accessed MPSD8 ????
	QMap<int, MPSD8 *> m_mpsd;
	
	//! the header time stamp
	quint64		m_headertime;

	//! the header time in ms
	quint64		m_timemsec;

	//! the firmware version 
	float		m_version;

	//! last register read value
	quint16		m_reg;

	//! last peripheral register value
	quint16		m_periReg;
};

#endif

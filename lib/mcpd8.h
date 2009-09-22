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
 * \short representation of MCPD-8 central module
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
*/
class MCPD8 : public MesydaqObject
{
Q_OBJECT
public:
	MCPD8(quint8, QObject *parent = 0, QString = "192.168.168.121", quint16 = 54321, QString = "0.0.0.0");

	~MCPD8();

// commands of the MPCD-8
// commands: DAQ commands
	//! resets the MCPD-8
	bool reset(void);

	//! starts the data acquisition
	bool start(void);

	//! stops the data acquisition
	bool stop(void);

	//! continues the data acquisition
	bool cont(void);

// commands: communication settings
	/**
	 * sets the id of the MCPD
	 *
	 * \param mcpdid the new ID of the MCPD
	 * \return true if operation was succesful or not
	 */
	bool setId(quint8 mcpdid);

	/**
	 *  reads the ID's of all connected MPSD-8/8+ and MSTD-16
	 *
	 * \return true if operation was succesful or not
	 */
	bool readId(void);

	//! \return the ID of this MCPD
	quint8 getId(void) { return m_id; }

	bool setProtocol(const QString addr, const QString datasink = "0.0.0.0", const quint16 dataport = 0, const QString cmdsink = "0.0.0.0", const quint16 cmdport = 0);

	void getProtocol(quint16 *addr);

	bool setTimingSetup(bool master, bool sync);

	//! \return whether this MCPD is configured as master or not
	bool isMaster(void) {return m_master;}

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

	/**
	 * sets the auxiliary timer to a new value
	 *
	 * \param timer number of the timer
	 * \param val new timer value
	 * \return true if operation was succesful or not
	 * \see getAuxTimer
	 */
	bool setAuxTimer(quint16 timer, quint16 val);

	/**
	 * get the value of auxiliary counter
	 *
	 * \param timer number of the timer
	 * \return counter value
	 * \see setAuxTimer
	 */
	quint16 getAuxTimer(quint16 timer);

	bool setParamSource(quint16 param, quint16 source);

	quint16 getParamSource(quint16 param);

	bool setParameter(quint16 param, quint64 val);

	quint64 getParameter(quint16 param);

// commands: MPSD-8 settings
	/**
	 * sets the gain to a poti value
	 * 
	 * \param addr number of the module
	 * \param channel channel number of the module
	 * \param gain poti value of the gain
	 * \return true if operation was succesful or not
	 * \see getGain
	 */ 
	bool setGain(quint16 addr, quint8 channel, quint8 gain);

	/**
	 * \overload setGain(quint16 addr, quint8 channel, float gain);
	 * 
	 * the gain value will be set as a user value
	 */
	bool setGain(quint16 addr, quint8 channel, float gain);

	/**
	 * gets the currently set gain value for a special module and channel
	 * 
	 * if the channel number is greater 7 than all channels of the module
	 * will be set
	 *
	 * \param addr number of the module
	 * \param chan number of the channel of the module
	 * \return poti value of the gain
	 * \see setGain
	 */
	quint8	getGain(quint16 addr, quint8 chan);

	/**
	 * set the threshold value as poti value
	 *
	 * \param addr number of the module
	 * \param thresh threshold value as poti value
	 * \return true if operation was succesful or not
	 * \see getThresh
	 */
	bool setThreshold(quint16 addr, quint8 thresh);

	/**
	 * get the threshold value as poti value
	 *
	 * \param addr module number
	 * \return the threshold as poti value
	 * \see setThreshold
	 */
	quint8 getThreshold(quint16 addr);

	/**
	 * set the pulser of the module to a position, channel, and amplitude
	 * and switch it on or off
	 * 
	 * \param addr number of the module
	 * \param channel number of the channel of the module
	 * \param position set the position to left, middle or right of the 'tube'
	 * \param amp the amplitude of a test pulse (event)
	 * \param onoff true the pulser will be switch on, otherwise off
	 * \return true if operation was succesful or not
	 * \see isPulserOn
	 */
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
	/**
	 * reads the content of a register in a module
	 * 
	 * \param mod number of the module
	 * \param reg number of the register
	 * \return content of the register
	 * \see writePeriReg
	 */
	quint16 readPeriReg(quint16 mod, quint16 reg);

	/**
	 * writes a value into a module register
	 *
	 * \param mod number of the module
	 * \param reg number of the register
	 * \param val new value
	 * \return true if operation was succesful or not
	 * \see readPeriReg
	 */
	bool writePeriReg(quint16 mod, quint16 reg, quint16 val);

	/**
	 * writes a value into a register of the MCPD
	 *
	 * \param reg number of the register
	 * \param val new value
	 * \return true if operation was succesful or not
	 * \see readRegister
	 */
	bool writeRegister(quint16 reg, quint16 val);

	/**
	 * reads the content of a register 
	 * 
	 * \param reg number of the register
	 * \return content of the register
	 * \see writeRegister
	 */
	quint16 readRegister(quint16 reg);

	//! \return firmware version of the MCPD whereas the integral places represents the major number and the decimal parts the minor number
	float version(void); 

	//! \return number of modules found
	quint8 numModules(void) {return m_mpsd.size();}

	bool init(void);

	bool setStream(quint16 strm);

	bool getStream(void) {return m_stream;}

	bool serialize(QDataStream ds);

	bool isBusy(void) {return m_commActive;}

	//! \return true if one of the connected modules has a switched on pulser
	bool isPulserOn();

	/**
	 checks if the pulser of module number addr
	 \param addr number of the module to query
	 \return is pulser on or not
	 */
	bool isPulserOn(quint8 addr);

	quint8	getPulsPos(quint8 addr, bool preset = false);

	quint8	getPulsAmp(quint8 addr, bool preset = false); 

	quint8	getPulsChan(quint8 addr, bool preset = false); 

	quint64 time(void) {return m_timemsec;}

	bool setRunId(quint16 runid);

	quint16 getRunId(void) {return m_runId;}

public slots:
	/**
	 * analyze the data package coming from the MCPD-8
	 *
	 * \param pd data package
	 */
	void analyzeBuffer(MDP_PACKET &pd);

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
	void analyzeDataBuffer(DATA_PACKET &pd);

private:
	void communicate(bool yesno) {m_commActive = yesno;}

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

	//! the accessed MPSD8 ????
	QMap<int, MPSD_8 *> m_mpsd;
	
private:
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


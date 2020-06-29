/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
 *   Copyright (C) 2013-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

#include "mcpd.h"

class MPSD8;
class M2D;
class QTimer;

/**
 * \short representation of MCPD-8 central module
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MCPD8 : public MCPD
{
	Q_OBJECT

public:
	MCPD8(quint8, QString = "192.168.168.121", quint16 = 54321, QString = QString::null, quint16 = 0, QString = QString::null, bool = false);

	virtual ~MCPD8();

	virtual bool isInitialized() const;

        //! \return whether the whole MCPD should integrated into the histogram
	bool histogram(void);

	void setHistogram(bool hist);

	void setHistogram(quint16 id, bool hist);

	void setHistogram(quint16 id, quint16 chan, bool hist);

	bool histogram(quint16 id);

	bool histogram(quint16 id, quint16 chan);

        //! \return whether the whole MCPD is in use or not
	bool active(void);

	bool active(quint16 id);

	bool active(quint16 id, quint16 chan);

	void setActive(bool act);

	void setActive(quint16 id, bool act);

	void setActive(quint16 id, quint16 chan, bool act);

	QList<quint16> getHistogramList(void);

	QList<quint16> getActiveList(void);

	QMap<quint16, quint16> getTubeMapping();

// commands of the MPCD-8
// commands: DAQ commands
	bool reset(void);

	bool start(void);

	bool stop(void);

	bool cont(void);

// commands: communication settings
	bool setId(quint8 mcpdid);

	bool readId(void);

	bool scanPeriph(void);

	bool setProtocol(const QString& addr, const QString& datasink = QString::null, const quint16 dataport = 0, const QString& cmdsink = QString::null, const quint16 cmdport = 0);

	void getProtocol(QString& ip, QString& cmdip, quint16& cmdport, QString& dataip, quint16& dataport) const;
	void getProtocol(quint16 *addr);

	bool setTimingSetup(bool master, bool term, bool extsync);

	//! \return whether this MCPD is configured as master or not
	bool isMaster(void) const;

	//! \return whether this MCPD is terminated on the synchronization bus or not
	bool isTerminated(void) const;

	//! \return whether this MCPD has enabled external sync
	bool isExtsynced(void) const;

	bool setMasterClock(quint64);

	//! returns the number of received data packages
	quint64 receivedData() const;

	//! returns the number of missed data packages
	quint64 missedData() const;

	//! returns the number of received cmd answer packages
	quint64 receivedCmds() const;

	//! returns the number of sent cmd packages
	quint64 sentCmds() const;

// commands: General MCPD-8 settings
	bool setCounterCell(quint16 source, quint16 trigger, quint16 compare);

	void getCounterCell(quint8 cell, quint16 *celldata);

	bool setAuxTimer(quint16 timer, quint16 val);

	quint16 getAuxTimer(quint16 timer);

	bool setParamSource(quint16 param, quint16 source);

	quint16 getParamSource(quint16 param);

	bool setParameter(quint16 param, quint64 val);

	quint64 getParameter(quint16 param);

	bool getParameter(void);

// commands: MPSD-8 settings
	bool setGain(quint16 addr, quint8 channel, quint8 gain);

	bool setGain(quint16 addr, quint8 channel, float gain);

	quint8	getGainPoti(quint16 addr, quint8 chan);

	float	getGainVal(quint16 addr, quint8 chan);

	bool setThreshold(quint16 addr, quint8 thresh);

	bool setMdllThresholds(quint8 threshX, quint8 threshY, quint8 threshA);

	bool setMdllSpectrum(quint8 shiftX, quint8 shiftY, quint8 scaleX, quint8 scaleY);

	bool setMdllDataset(quint8 set);

	bool setMdllTimingWindow(quint16 xlo, quint16 xhi, quint16 ylo, quint16 yhi);

	bool setMdllEnergyWindow(quint8 elo, quint8 ehi);

	quint8 getMdllThreshold(quint8 val);

	quint8 getMdllSpectrum(quint8 val);

	quint8 getMdllDataset(void);

	quint16 getMdllTimingWindow(quint8 val);

	quint8 getMdllEnergyWindow(quint8 val);

	quint8 getMdllPulser(quint8 val);


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

	quint8 getChannels(quint8 addr);

	QString getModuleType(quint8 addr);

	bool online(quint8 addr);

	void initModule(quint8 id);

	quint16 getTxMode(const bool cached = false);

	quint16 getTxMode(quint8 id);

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

	float fpgaVersion(void);

	float version(quint16 mod);

	//! \return number of modules found
	quint8 numModules(void);

	bool init(void);

	bool setStream(quint16 strm);

	bool setTxMode(quint16 mode);

	quint16 capabilities(const bool);

	quint16 capabilities(quint8 mod);

	/**
	 * ????
	 *
	 * \return ???
	 * \see setStream
	 */
	bool getStream(void) const;

//	bool serialize(QDataStream ds);

	//! \return the current time of the MCPD in msec
	quint64 time(void) const;

	bool setRunId(quint32 runid);

	/**
	 * gets the current set run ID
	 *
	 * \return run ID
	 * \see setRunId
	 */
	quint32 getRunId(void) const;

	quint16 bins();

        /**
         * gets the list of available MPSD's connected with the MCPD
         *
         * \return list of id numbers
         */
	QList<int> mpsdId(void);

        /**
         * gets the list of histogrammed MPSD's connected with the MCPD
         *
         * \return list of id numbers
         */
	QList<int> histogrammedId(void);

	QList<int> channelId(int mod);

	int width() const;

public slots:
	//! analyse network packet
	virtual bool analyzeBuffer(QSharedDataPointer<SD_PACKET> pPacket);

private slots:
	//! callback for the communication timer to detect a timeout
	// void commTimeout(void);

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
	void analyzeDataBuffer(QSharedDataPointer<SD_PACKET> pd);

	//! a new header time received
	void headerTimeChanged(quint64);

protected:
	void initCmdBuffer(quint16);

	void finishCmdBuffer(quint16 buflen);

	bool sendCommand(bool bCheckAnswer);
	
        /*!
            fills the buffer on index with the value
           
            \param index
            \param value
         */
	void setBuffer(quint8 index, quint16 value);

	bool parseDataBuffer(QSharedDataPointer<SD_PACKET> pd);

	bool parseCmdBuffer(QSharedDataPointer<SD_PACKET> pd);

private:
	quint16 calcChksum(const MDP_PACKET &buffer);

	quint16 calcChksum(const QSharedDataPointer<SD_PACKET> &buffer);

	void stdInit(void);

private:
	//! test mode: do not initialize, for testing existence only (read MCPD version)
	bool		m_bTestOnly;

	//! counter for the send command buffer packets
	quint16		m_txCmdBufNum;

// communication params
	//! IP address of the module
	QString 	m_ownIpAddress;
	
	//! is this MCPD master on sync bus or not
	bool		m_master;

	//! is this MCPD terminated on sync bus or not
	bool 		m_term;
	
	//! is this MCPD external sync enabled or not
	bool 		m_extsync;

	//! 8 counter cells, trig source in [0], compare reg in [1]
	quint8 		m_counterCell[8][2];

	//! four auxiliary timers, capture values
	quint16 	m_auxTimer[4];

	bool		m_stream;
	quint64		m_parameter[4];
	volatile enum {
		SEND,
		RECV,
		RECV_INVALID,
	}		m_iCommActive;


	//! four parameters (transmitted in buffer header), 9 possible sources
	quint8 		m_paramSource[4];
	
	MDP_PACKET      m_cmdBuf;

	//! last command buffer number
	quint32 	m_lastCmdBufnum;

	//! last data buffer number
	quint32		m_lastDataBufnum;

	QTimer 		*m_commTimer;

	//! current run ID
	quint32 	m_runId;

	//! counter for the received data packets
	quint32		m_dataRxd;

	//! counter for the missed data packets
	quint32		m_dataMissed;

	//! counter for the sent cmd packets
	quint32		m_cmdTxd;

	//! counter for the receivcd cmd packets
	quint32		m_cmdRxd;

	//! the accessed MPSD8 ????
	QMap<int, MPSD8 *> m_mpsd;
	
	//! possibly connected MDLL
	QMap<int, M2D *> m_mdll;

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

	//! capabilities
	quint16		m_capabilities;

	//! Tx mode
	quint16		m_txMode;

	//! READID data
	quint16		m_awReadId[8];

	//! FPGA version
	float		m_fpgaVersion;
};

#endif

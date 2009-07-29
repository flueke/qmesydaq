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


#ifndef _MESYDAQ2_H_
#define _MESYDAQ2_H_

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QString>
#include <QTimer>

#include "mesydaqobject.h"
#include "structures.h"
#include "counter.h"

/**
 * @short Mesydaq DAQ object (without any graphical frontend
 * @author Gregor Montermann <g.montermann@mesytec.com>
 * @author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 * @version 0.9
 */

class Histogram;
class MCPD8;
class MPSD8;

class Mesydaq2 : public MesydaqObject 
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
	Mesydaq2(QObject *parent);

    /**
     * Default Destructor
     */
	virtual ~Mesydaq2();

	bool startDaq(void);
	void stopDaq(void);
	void startedDaq(void);
	void stoppedDaq(void);
	void clearChanHist(ulong chan);
	void clearAllHist(void);
	void scanPeriph(quint16 id);
	void initMpsd(quint8 id);
	void initMcpd(quint8 id);
	void setTimingwidth(quint8 width);
	void writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val);
	quint16 readPeriReg(quint16 id, quint16 mod, quint16 reg);

// list mode oriented methods
	void setListfilename(QString name) {m_listfilename = name;}
	QString getListfilename() {return m_listfilename;}
	bool checkListfilename(void);
	void setListfilepath(QString path) {m_listPath = path;}
	QString getListfilepath() {return m_listPath;}
	void readListfile(QString readfilename);
	void writeListfileHeader(void);
	void writeClosingSignature(void);
	void writeBlockSeparator(void);
	void writeHeaderSeparator(void);

// histogram file oriented methods
	void setHistfilename(QString name);
	QString getHistfilename(void) {return m_histfilename;}
	void setHistfilepath(QString path) {m_histPath = path;}
	QString getHistfilepath(void) {return m_histPath;}
	void writeHistograms();

// configuration file oriented methods
	void setConfigfilename(QString name) {m_configfilename = name;}
	QString getConfigfilename(void) {return m_configfilename;}
	void setConfigfilepath(QString path) {m_configPath = path;}
	QString getConfigfilepath(void) {return m_configPath;}
	bool loadSetup(const QString &name);
	bool saveSetup(const QString &name);

	bool checkMcpd(quint8 device);
	void copyData(quint32 line, ulong *data);
	quint8 isDaq(void) {return m_daq;}

	bool isPulserOn();
	
	bool isPulserOn(quint16 mod);

	bool isPulserOn(quint16 mod, quint8 addr);

	quint8 getMpsdId(quint16 mod, quint8 addr);

	quint8 getGain(quint16 mod, quint16 addr, quint8 chan);

	quint8 getThreshold(quint16 mod, quint16 addr);

	quint64 getMTime(void) {return m_timeMsecs;}

	quint64 getHeadertime(void) {return m_headertime;}

	quint64 receivedData() {return m_dataRxd;}

	quint64 receivedCmds() {return m_cmdRxd;}

	quint64 sentCmds() {return m_cmdTxd;}

public slots:
	void writeRegister(quint16 id, quint16 addr, quint16 reg, quint16 val);

	void setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP = "0.0.0.0", const qint16 dataPort = 0, const QString cmdIP = "0.0.0.0", const qint16 cmdPort = 0);
	
	void setMode(const quint16 id, quint16 addr, bool mode);

	void setPulser(const quint16 id, quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff);

	void setCounterCell(quint16 mod, quint16 source, quint16 trigger, quint16 compare);

	void setParamSource(quint16 mod, quint16 param, quint16 source);

	void setAuxTimer(quint16 mod, quint16 tim, quint16 val);

	void setMasterClock(quint16 mod, quint64);

	void setTimingSetup(quint16 mod, bool master, bool sync);

	void setId(quint16 mod, quint8 mcpdid);

	void setGain(quint16 mod, quint16 addr, quint8 channel, quint8 gain);

	void setGain(quint16 mod, quint16 addr, quint8 channel, float gain);

	void setThreshold(quint16 mod, quint16 addr, quint8 thresh);

	void centralDispatch();
	void protocol(QString str, quint8 level = 1);
    	void acqListfile(bool yesno);
    	void setCountlimit(quint8 cNum, ulong lim);

	void start(void);
	void stop(void);
	void reset(void);
	void cont(void);

	void allPulserOff();

	void analyzeBuffer(DATA_PACKET &pd);

signals:
	void statusChanged(const QString &);

	void setCounter(quint32 cNum, quint64 val);

	void incEvents();

	void incEvents(quint16 chan, quint16 data0, quint16 data1, quint64 tim);

	void updateCounters();

	void incCounter(quint8 trigId, quint8 dataId, quint32 data, quint64 tim);

private:
	void initNetwork(void);
	void initValues(void);
	void initHardware(void);
	void initDevices(void);
	void initTimers(void);

	void analyzeBuffer(DATA_PACKET &pd, quint8 daq, Histogram &hist);

	void setLimit(quint8 cNum, ulong lim);

public:
#warning TODO
	MCPD8 		*m_mcpd[8];

private:
	static const quint16  	sep0 = 0x0000;
	static const quint16  	sep5 = 0x5555;    
	static const quint16  	sepA = 0xAAAA;
	static const quint16  	sepF = 0xFFFF;

private:
// not really used in Mesydaq2 
	ulong 		m_dataRxd;
	ulong 		m_cmdRxd;
	ulong 		m_cmdTxd;

	Histogram 	*m_hist;

	quint8  	m_daq;
	ulong 		statuscounter[0];
    
	bool 		m_acquireListfile;
	QString 	m_listfilename;
	QString 	m_histfilename;
	QString 	m_configfilename;

	QFile 		m_datfile;
	QDataStream 	m_datStream;
	QTextStream 	m_textStream;

	QString 	m_listPath;
	QString 	m_histPath;
	QString 	m_configPath;

	quint8  	m_timingwidth;
	quint32 	m_lastBufnum;
	quint8  	m_dispatch[10];

	MesydaqCounter 	m_counter[9];

	QTimer 		*theTimer;

	quint64		m_headertime;

	quint64 	m_timeMsecs;
};



#endif // _MESYDAQ2_H_

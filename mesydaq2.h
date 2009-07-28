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
#include <QLabel>
#include <QApplication>

#include "structures.h"

/**
 * @short Application Main Object
 * @author Gregor Montermann <g.montermann@mesytec.com>
 * @version 0.8
 */


class MainWidget;
class Histogram;
class MCPD8;
class MPSD8;
class Measurement;
class CorbaThread;
class ControlInterface;

class Mesydaq2 : public QObject // MainWindow, public Ui_Mesydaq2MainWindow
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
	void draw(void);
	void scanPeriph(quint16 id);
	void dispMpsd(void);
	void initMpsd(quint8 id);
	void initMcpd(quint8 id);
	void allPulserOff();
	void setTimingwidth(quint8 width);
	void writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val);
	quint16 readPeriReg(quint16 id, quint16 mod, quint16 reg);

// list mode oriented methods
	void setListfilename(QString name) {m_listfilename = name;}
	QString getListfilename() {return m_listfilename;}
	bool checkListfilename(void);
	void setListfilepath(QString path) {listPath = path;}
	QString getListfilepath() {return listPath;}
	void readListfile(QString readfilename);
	void writeListfileHeader(void);
	void writeClosingSignature(void);
	void writeBlockSeparator(void);
	void writeHeaderSeparator(void);

// histogram file oriented methods
	void setHistfilename(QString name);
	QString getHistfilename(void) {return m_histfilename;}
	void setHistfilepath(QString path) {histPath = path;}
	QString getHistfilepath(void) {return histPath;}
	void writeHistograms();

// configuration file oriented methods
	void setConfigfilename(QString name) {m_configfilename = name;}
	QString getConfigfilename(void) {return m_configfilename;}
	void setConfigfilepath(QString path) {configPath = path;}
	QString getConfigfilepath(void) {return configPath;}
	bool loadSetup(bool ask);
	bool saveSetup(void);

	bool checkMcpd(quint8 device);
	void copyData(quint32 line, ulong *data);
	quint8 isDaq(void) {return m_daq;}

	bool isPulserOn();
	
	bool isPulserOn(quint16 mod);

	bool isPulserOn(quint16 mod, quint8 addr);

	quint8 getMpsdId(quint16 mod, quint8 addr);

	quint8 getGain(quint16 mod, quint16 addr, quint8 chan);

	quint8 getThreshold(quint16 mod, quint16 addr);

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

	void dispTime();
	void centralDispatch();
	void commTimeout();
	void protocol(QString str, quint8 level = 1);
    	void acqListfile(bool yesno);
    	void setCountlimit(quint8 cNum, ulong lim);

	void start(void);
	void stop(void);
	void reset(void);
	void cont(void);

	void analyzeBuffer(DATA_PACKET &pd);

private:
	void initNetwork(void);
	void initValues(void);
	void initHardware(void);
	void initDevices(void);
	void initTimers(void);

	void analyzeBuffer(DATA_PACKET &pd, quint8 daq, Histogram &hist);
	void setLimit(quint8 cNum, ulong lim);

public:
// not really used in Mesydaq2 
	ulong 		m_dataRxd;
	ulong 		m_cmdRxd;
	ulong 		m_cmdTxd;
	ulong 		m_headertime;

	ulong 		m_timeMsecs;

#warning TODO
	MCPD8 		*m_mcpd[8];
	Measurement 	*meas;
	ControlInterface *cInt;

private:
	Histogram 	*m_hist;

	quint8  	m_daq;
	ulong 		statuscounter[0];
	quint16  	sep0;
	quint16  	sep5;    
	quint16  	sepA;
	quint16  	sepF;
	quint8  	cmdLen[50];
	QString 	setupfile;
	quint32 	dispatchLevel;
	quint8  	commId;
//	quint16  	commandBuffer[20];
    
	bool 		m_acquireListfile;
	QString 	m_listfilename;
	QString 	m_histfilename;
	QString 	m_configfilename;

	QFile 		datfile;
	QDataStream 	datStream;
	QTextStream 	textStream;
	QString 	listPath;
	QString 	histPath;
	QString 	configPath;
	bool 		ovwList;
	quint8  	timingwidth;
	quint32 	m_lastBufnum;
	quint16  	initstep;
	quint16  	initcmd;
	quint8  	initcpd;
	quint8  	initpsd;
	bool 		initialize;
	CorbaThread	*ct;
	quint8  	dispatch[10];

	ulong 		m_countLimit[9];

	QString 	pstring;
	QTimer 		*dispTimer;
	QTimer 		*commTimer;
	QTimer 		*theTimer;
};



#endif // _MESYDAQ2_H_

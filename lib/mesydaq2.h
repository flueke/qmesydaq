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


#ifndef _MESYDAQ2_H_
#define _MESYDAQ2_H_

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QTimer>
#include <QMap>

#include "mesydaqobject.h"
#include "structures.h"

/**
 * @short Mesydaq DAQ object (without any graphical frontend
 * @author Gregor Montermann <g.montermann@mesytec.com>
 * @author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 * @version 0.9
 */

// class Histogram;
class MCPD8;
class MPSD_8;

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

	void scanPeriph(quint16 id);
	void initMcpd(quint8 id);
	void setTimingwidth(quint8 width);
	void writePeriReg(quint16 id, quint16 mod, quint16 reg, quint16 val);
	quint16 readPeriReg(quint16 id, quint16 mod, quint16 reg);

// list mode oriented methods
	void setListfilename(QString name) {m_listfilename = name;}
	QString getListfilename() {return m_listfilename;}
	void setListfilepath(QString path) {m_listPath = path;}
	QString getListfilepath() {return m_listPath;}
	void writeListfileHeader(void);
	void writeClosingSignature(void);
	void writeBlockSeparator(void);
	void writeHeaderSeparator(void);

// histogram file oriented methods
	void setHistfilename(QString name);
	QString getHistfilename(void) {return m_histfilename;}
	void setHistfilepath(QString path) {m_histPath = path;}
	QString getHistfilepath(void) {return m_histPath;}

// configuration file oriented methods
	void setConfigfilename(QString name) {m_configfilename = name;}
	QString getConfigfilename(void) {return m_configfilename;}
	void setConfigfilepath(QString path) {m_configPath = path;}
	QString getConfigfilepath(void) {return m_configPath;}
	bool loadSetup(const QString &name);
	bool saveSetup(const QString &name);

	bool checkMcpd(quint8 device);
	quint8 isDaq(void) {return m_daq;}

	bool isPulserOn();
	
	bool isPulserOn(quint16 mod);

	bool isPulserOn(quint16 mod, quint8 addr);

	bool getMode(const quint16 mod, quint8 addr);

	quint8 getMpsdId(quint16 mod, quint8 addr);

	QString getMpsdType(quint16 mod, quint8 addr);

	quint8 getGain(quint16 mod, quint8 addr, quint8 chan);

	quint8 getThreshold(quint16 mod, quint8 addr);

	quint8 getPulsChan(quint16 mod, quint8 addr);

	quint8 getPulsAmp(quint16 mod, quint8 addr);

	quint8 getPulsPos(quint16 mod, quint8 addr);

	quint64 receivedData(); 

	quint64 receivedCmds(); 

	quint64 sentCmds(); 

	quint64 time();

	float getFirmware(quint16 mod);

public slots:
	void writeRegister(quint16 id, quint16 reg, quint16 val);

	void setProtocol(const quint16 id, const QString &mcpdIP, const QString dataIP = "0.0.0.0", const qint16 dataPort = 0, const QString cmdIP = "0.0.0.0", const qint16 cmdPort = 0);
	
	void setMode(const quint16 id, quint16 addr, bool mode);

	void setPulser(const quint16 id, quint16 addr, quint8 channel, quint8 position, quint8 amp, bool onoff);

	void setCounterCell(quint16 mod, quint16 source, quint16 trigger, quint16 compare);

	void setParamSource(quint16 mod, quint16 param, quint16 source);

	void setAuxTimer(quint16 mod, quint16 tim, quint16 val);

	void setMasterClock(quint16 mod, quint64);

	void setTimingSetup(quint16 mod, bool master, bool sync);

	void setId(quint16 mod, quint8 mcpdid);

	void setGain(quint16 mod, quint8 addr, quint8 channel, quint8 gain);

	void setGain(quint16 mod, quint8 addr, quint8 channel, float gain);

	void setThreshold(quint16 mod, quint8 addr, quint8 thresh);

	void setThreshold(quint16 mod, quint8 addr, quint16 thresh);

    	void acqListfile(bool yesno);

	void start(void);
	void startedDaq(void);

	void stop(void);
	void stoppedDaq(void);

	void reset(void);

	void cont(void);

	void allPulserOff();

	void analyzeBuffer(DATA_PACKET &pd);

signals:
	void statusChanged(const QString &);

	void analyzeDataBuffer(DATA_PACKET &pd);

protected:
	void timerEvent(QTimerEvent *event);

private:
	void initHardware(void);

	void initDevices(void);

	void initTimers(void);

public:
#warning TODO remove the public attribute
	QMap<int, MCPD8	*>	m_mcpd;

private:
	static const quint16  	sep0 = 0x0000;
	static const quint16  	sep5 = 0x5555;    
	static const quint16  	sepA = 0xAAAA;
	static const quint16  	sepF = 0xFFFF;

private:
	quint8  	m_daq;
    
	bool 		m_acquireListfile;
	QString 	m_listfilename;
	QString 	m_histfilename;
	QString 	m_configfilename;

	QFile 		m_datfile;
	QDataStream 	m_datStream;

	QString 	m_listPath;
	QString 	m_histPath;
	QString 	m_configPath;

	quint8  	m_timingwidth;
	quint32 	m_lastBufnum;

	int 		m_checkTimer;
};



#endif // _MESYDAQ2_H_
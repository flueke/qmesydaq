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

#include <list>

#include <QFile>
#include <QDataStream>
#include <QString>
#include <QTimer>
#include <QHash>
#include <QFileInfo>

#include "mesydaqobject.h"
#include "structures.h"
#include "inifile.h"


class MCPD8;
class MPSD_8;

/**
 * \short Mesydaq DAQ object (without any graphical frontend)
 * \author Gregor Montermann <g.montermann@mesytec.com>
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 * \version 0.9
 */
class Mesydaq2 : public MesydaqObject 
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
	Mesydaq2(QObject *parent = 0);

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
	/**
	 * sets the file name of a list mode data file
	 *
	 * \param name name of the next list mode data file
	 */
	void setListfilename(QString name) {m_listfilename = name;}
	
	//! \return name of the current list mode data file
	QString getListfilename() {return m_listfilename;}

	/**
	 * sets the path for the list mode data files
	 *
	 * \param path to the list mode data files
	 */
	void setListfilepath(QString path) {m_listPath = path;}

	//! \return path to store all list mode data files
	QString getListfilepath() {return m_listPath;}

	void writeListfileHeader(void);

	void writeClosingSignature(void);

	void writeBlockSeparator(void);

	void writeHeaderSeparator(void);

	//! \brief store header for list mode file
	void setListFileHeader(const QByteArray& header) { m_datHeader=header; }

	//! \return header for list mode file
	const QByteArray& getListFileHeader() const { return m_datHeader; }

// histogram file oriented methods
	/**
	 * sets the file name of a histogram data file
	 *
	 * \param name name of the next histogram data file
	 */
	void setHistfilename(QString name);

	//! \return name of the current histogram data file
	QString getHistfilename(void) {return m_histfilename;}

	/**
	 * sets the path for the histogram data files
	 *
	 * \param path to the histogram data files
	 */
	void setHistfilepath(QString path) {m_histPath = path;}

	//! \return path to store all histogram data files
	QString getHistfilepath(void) {return m_histPath;}

// configuration file oriented methods
	/**
	 * sets the config file name
	 *
	 * \param name config file name
	 */
	void setConfigfilename(const QString &name);

	//! \return last loaded config file name
	QString getConfigfilename(void);

	/**
	 * sets the path for the config files
	 *
	 * \param path to the config files
	 */
	void setConfigfilepath(QString path) {m_configPath = path;}

	//! \return path to store all config files
	QString getConfigfilepath(void) {return m_configPath;}

	bool loadSetup(const QString &name);
	bool saveSetup(const QString &name);

	bool checkMcpd(quint8 device);

	qint16 firstMcpd(void);

	//! \return if data acquisition started or not
	quint8 isDaq(void) {return m_daq;}

	bool isPulserOn();
	
	bool isPulserOn(quint16 mod);

	bool isPulserOn(quint16 mod, quint8 addr);

	bool getMode(const quint16 mod, quint8 addr);

	quint8 getMpsdId(quint16 mod, quint8 addr);

	QString getMpsdType(quint16 mod, quint8 addr);

	float getMpsdVersion(quint16 mod, quint8 addr);

	bool online(quint16 mod, quint8 addr);

	float getGain(quint16 mod, quint8 addr, quint8 chan);

	quint8 getThreshold(quint16 mod, quint8 addr);

	quint8 getPulsChan(quint16 mod, quint8 addr);

	quint8 getPulsAmp(quint16 mod, quint8 addr);

	quint8 getPulsPos(quint16 mod, quint8 addr);

	void getCounterCell(quint16 mod, quint8 cell, quint16 *celldata);

	quint16 getParamSource(quint16 mod, quint16 param);
	
	quint16 getAuxTimer(quint16 mod, quint16 timer);

	quint64 getParameter(quint16 mod, quint16 param);

	void setRunId(quint16 mod, quint16 runid);

	quint64 receivedData(); 

	quint64 receivedCmds(); 

	quint64 sentCmds(); 

	quint64 time();

	float getFirmware(quint16 mod);

	void addMCPD(quint16 id, QString = "192.168.168.121", quint16 = 54321, QString = "0.0.0.0");

	//! \return number of MCPD's
	quint16 numMCPD(void) {return m_mcpd.size();}

	QList<int> mcpdId(void) 
	{
		QList<int> st = m_mcpd.keys();
		qSort(st);
		return st;
	}

	bool isMaster(quint16 mod);

	bool isTerminated(quint16 mod);

	quint16 bins();

public slots:
	void writeRegister(quint16 id, quint16 reg, quint16 val);

	void setProtocol(const quint16 id, const QString &mcpdIP, const QString &dataIP = QString("0.0.0.0"), quint16 dataPort = 0, const QString &cmdIP = QString("0.0.0.0"), quint16 cmdPort = 0);
	
	void setMode(const quint16 id, quint8 addr, bool mode);

	void setPulser(const quint16 mod, quint8 addr, quint8 channel, quint8 position, quint8 amp, bool onoff);

	void setCounterCell(quint16 mod, quint16 source, quint16 trigger, quint16 compare);

	void setParamSource(quint16 mod, quint16 param, quint16 source);

	void setAuxTimer(quint16 mod, quint16 tim, quint16 val);

	void setMasterClock(quint16 mod, quint64);

	void setTimingSetup(quint16 mod, bool master, bool term);

	void setId(quint16 mod, quint8 mcpdid);

	void setGain(quint16 mod, quint8 addr, quint8 channel, quint8 gain);

	void setGain(quint16 mod, quint8 addr, quint8 channel, float gain);

	void setThreshold(quint16 mod, quint8 addr, quint8 thresh);

	void setThreshold(quint16 mod, quint8 addr, quint16 thresh);

    	void acqListfile(bool yesno);
	bool acqListfile() const { return m_acquireListfile; }

	void start(void);

	void startedDaq(void);

	void stop(void);

	void stoppedDaq(void);

	void reset(void);

	void cont(void);

	void allPulserOff();

	virtual void analyzeBuffer(DATA_PACKET &pd);

signals:
	/**
	 * will be emitted if the status has changed
	 *	
	 * \param str the status as string
	 */
	void statusChanged(const QString &str);

	/**
	 * will be emitted if new data packet reached from one of the MCPD's 
	 *
	 * \param pd data packet
	 */
	void analyzeDataBuffer(DATA_PACKET &pd);

protected:
	void timerEvent(QTimerEvent *event);

private:
	void clear();
	
	void initHardware(void);

	void initDevices(void);

	void initTimers(void);

	void storeLastFile(void);

	static void saveSetup_helper(CConfigFile& file, const QString& szSection, int iPriority, const QString& szItem, QString szValue);
	static QString loadSetup_helper(CConfigSection* pSection, const QString& szItem, const QString& szDefault);

	QHash<int, MCPD8 *>	m_mcpd;

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
	QFileInfo 	m_configfile;

	QFile 		m_datfile;
	QDataStream 	m_datStream;

	QString 	m_listPath;
	QString 	m_histPath;
	QString 	m_configPath;

	quint8  	m_timingwidth;
	int 		m_checkTimer;

	QByteArray 	m_datHeader;
};



#endif // _MESYDAQ2_H_

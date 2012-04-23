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
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QObject>
#include <QRect>
#include <QFileInfo>

#include "libqmesydaq_global.h"
#include "counter.h"
#include "structures.h"
#include "mdefines.h"
#include "mesydaq2.h"

class Histogram;
class Spectrum;
class MapCorrection;
class MappedHistogram;
class Mesydaq2;
class QTextStream;

/**
 * \short representation of a measurement

   \author Gregor Montermann <g.montermann@mesytec.com>
*/
class LIBQMESYDAQ_EXPORT Measurement : public QObject
{
	Q_OBJECT
	Q_ENUMS(Mode)

	//! stores the current mode of the measurement
	Q_PROPERTY(Mode m_mode READ mode)

	Q_PROPERTY(QString m_histfilename READ getHistfilename WRITE setHistfilename)
	Q_PROPERTY(QString m_histPath READ getHistfilepath WRITE setHistfilepath)
	Q_PROPERTY(QString m_listPath READ getListfilepath WRITE setListfilepath)
	Q_PROPERTY(QString m_configPath READ getConfigfilepath WRITE setConfigfilepath)

        //! stores the currently loaded configfile name
	Q_PROPERTY(QString  m_configfile READ getConfigfilename WRITE setConfigfilename)

public:
	//! Defines the current mode values of a measurement 
        //! - DataAcquistion - data will be taken from the hardware
        //! - ReplayListFile - data will be taken from a list mode file
        //! - HistogramLoad - data will be taken from a histogram file
	enum Mode {
		DataAcquisition = 0,
		ReplayListFile,
		HistogramLoad,
	};

public:
	Measurement(Mesydaq2 *mesy, QObject *parent = 0);

	~Measurement();

	void resizeHistogram(quint16 w, quint16 h, bool = true, bool = false);

	//! \return the width of the histogram
	quint16 width(void);

	//! \return the height of the histogram
	quint16 height(void);

	quint64	getMeastime(void);

	void	setCurrentTime(quint64 msecs);
	void	stop();
	void	start();
	void	cont();

	void	calcMeanRates();
	ulong	getRate(quint8 cNum);

	void	setCounter(quint32 cNum, quint64 val);
	quint64	getCounter(quint8 cNum);

	quint8	isOk(void);
	void	setOnline(bool truth);

	quint64	getPreset(quint8 cNum);
	void	setPreset(quint8 cNum, quint64 prval, bool mast);

	void	setListmode(bool truth);
	void	setRemote(bool truth);
	bool	remoteStart(void);

	bool	isMaster(quint8 cNum);
	void	clearCounter(quint8 cNum);

	//! \return stream the data into a separate file too
	bool	acqListfile() const;

	void	readListfile(QString readfilename);

	//! \returns the number of counts in the defined ROI
	quint64	getROICounts(void);

	void 	setROI(QRectF r);

	/** 
		gets the value of the defined monitor 1
		\todo monitor mapping configuration
		\return counter value for the monitor 1 
	 */
	quint64	mon1() {return m_counter[MON1ID]->value();}

	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon2() {return m_counter[MON2ID]->value();}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon3() {return m_counter[MON3ID]->value();}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon4() {return m_counter[MON4ID]->value();}
	
	/** 
		gets the value of the defined event counter
		\todo counter mapping configuration
		\return counter value for the event counter
	 */
	quint64	events() {return m_counter[EVID]->value();}

	/** 
		gets the value of the timer
		\todo counter mapping configuration
		\return counter value for the event counter
	 */
	quint64	timer() {return m_counter[TIMERID]->value();}

	/** 
		gets the number of all events in the selected ROI
		\todo counter mapping configuration
		\return events in ROI 
	 */
	quint64 ampEventsInROI();

	/** 
		gets the number of all events in the selected ROI
		\todo counter mapping configuration
		\return events in ROI 
	 */
	quint64 posEventsInROI();

	//! \return time of the last header read from interface
	quint64 getHeadertime(void) {return m_headertime;}

	void writeHistograms(const QString &name);

	void readHistograms(const QString &name);

	void clearAllHist(void);

	void clearChanHist(quint16 chan);

	Spectrum *posData(quint16 line);
	Spectrum *posData();

	//! \return the position histogram
	Histogram *posHist() {return m_posHist;}

	Spectrum *ampData(quint16 line);	
	Spectrum *ampData();	

	//! \return the amplitude histogram
	Histogram *ampHist() {return m_ampHist;}	

	Spectrum *timeData();

	//! \return the diffractogram
	Spectrum *diffractogram(); 

	//! \return the mapping and correction data for position histogram
	MapCorrection*& posHistMapCorrection() { return m_posHistMapCorrection; }

	//! \return a mapped and corrected position histogram
	MappedHistogram*& posHistCorrected() { return m_posHistCorrected; }

	void getPosMean(float &, float &);
	void getPosMean(quint16, float &, float &);
	void getAmpMean(float &, float &);
	void getAmpMean(quint16, float &, float &);
	void getTimeMean(float &, float &);

	//! \brief store header for list mode file
	void setListFileHeader(const QByteArray& header);

// run ID oriented methods
	//! \return the current run ID
	quint16 runId(void) { return m_runID; }

	/*!
            sets the runid for the measurement
            \param runid
	 */
	void setRunId(quint16 runid)
	{
		m_mesydaq->setRunId(runid);
	}

	//! returns the current operation mode
	Mode mode(void) {return m_mode;}

// histogram file oriented methods
	/**
	 * sets the path for the histogram data files
	 *
	 * \param path to the histogram data files
	 */
	void setHistfilepath(QString path) {m_histPath = path;}

	//! \return path to store all histogram data files
	QString getHistfilepath(void) {return m_histPath;}

	/**
	 * sets the file name of a histogram data file
         *
         * \param name name of the next histogram data file
         */
	void setHistfilename(QString name);

	//! \return name of the current histogram data file
	QString getHistfilename(void) {return m_histfilename;}

// list mode oriented methods
	/**
	 * sets the path for the list mode data files
	 *
	 * \param path to the list mode data files
	 */
	void setListfilepath(QString path) {m_listPath = path;}

	//! \return path to store all list mode data files
	QString getListfilepath() {return m_listPath;}

// configuration file oriented methods
	/**
	 * sets the path for the config files
	 *
	 * \param path to the config files
	 */
	void setConfigfilepath(QString path) {m_configPath = path;}

	//! \return path to store all config files
	QString getConfigfilepath(void) {return m_configPath;}

	/**
	 * sets the config file name
	 *
	 * \param name config file name
	 */
	void setConfigfilename(const QString &name);

	//! \return last loaded config file name
	QString getConfigfilename(void);

	bool loadSetup(const QString &name);

	bool saveSetup(const QString &name);


public slots:
	void analyzeBuffer(DATA_PACKET pd);

	void calcRates();

private slots:
	void requestStop(void);

signals:
	//! will be emitted if one of the master counter reaches its limit
	void stopSignal(bool = false);

	//! will be emitted in case of change in respect to the handling of list mode data files
	void acqListfile(bool);

	//! will be emitted in case of a desired draw of the events
	void draw();
	
protected:
	void timerEvent(QTimerEvent *event);

private:
	void fillHistogram(QTextStream &t, Histogram *hist);

	void destroyHistogram(void);

	void storeLastFile(void);

private:
	static const quint16  	sep0 = 0x0000;
	static const quint16  	sep5 = 0x5555;    
	static const quint16  	sepA = 0xAAAA;
	static const quint16  	sepF = 0xFFFF;

	//! Access to hardware
	Mesydaq2	*m_mesydaq;

	//! position histogram buffer
	Histogram	*m_posHist;

	//! amplitude histogram buffer
	Histogram	*m_ampHist;

	//! time spectrum
	Spectrum	*m_timeSpectrum;

	//! 'diffractogram'
	Spectrum	*m_diffractogram;

	//! spectrum for the MSTD-16, it's only a hack not a solution
	Spectrum	*m_tubeSpectrum;

	//! mapping and correction data for position histogram
	MapCorrection   *m_posHistMapCorrection;

	//! position histogram with mapped and corrected data
	MappedHistogram	*m_posHistCorrected;

	quint64 	m_lastTime;

	quint64 	m_starttime_msec;
	quint64 	m_meastime_msec;

	quint8 		m_status;
	bool 		m_rateflag;
	bool 		m_online;
	bool 		m_working; 		// it's set to true and nothing else ????
	bool 		m_listmode;
	bool 		m_remote;

	quint64		m_headertime;

	//! definitions of the counters
	QHash<int, MesydaqCounter *>	m_counter; // [TIMERID + 1];

	int		m_rateTimer;

	int 		m_onlineTimer;

	QRect		m_roi;

	quint32		m_packages;

	quint64		m_triggers;

	quint16		m_runID;

	quint16		m_width;

	quint16		m_height;

	Mode		m_mode;

	QString 	m_histfilename;

	QString 	m_histPath;

	QString 	m_listPath;

	QString 	m_configPath;

	QFileInfo 	m_configfile;

};

#endif

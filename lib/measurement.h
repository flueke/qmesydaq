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
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QTextStream>
#include <QRectF>

#include "mesydaqobject.h"
#include "counter.h"
#include "structures.h"
#include "mdefines.h"

class Histogram;
class Spectrum;
class MapCorrection;
class MappedHistogram;
class Mesydaq2;

/**
 * \short representation of a measurement

   \author Gregor Montermann <g.montermann@mesytec.com>
*/
class Measurement : public MesydaqObject
{
	Q_OBJECT
public:
	Measurement(Mesydaq2 *mesy, QObject *parent = 0);

	~Measurement();

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

	ulong	getPreset(quint8 cNum);
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
	quint64	mon1() {return m_counter[M1CT]->value();}

	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon2() {return m_counter[M2CT]->value();}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon3() {return m_counter[M3CT]->value();}
	
	/** 
		gets the value of the defined monitor 2
		\todo monitor mapping configuration
		\return counter value for the monitor 2 
	 */
	quint64	mon4() {return m_counter[M4CT]->value();}
	
	/** 
		gets the value of the defined event counter
		\todo counter mapping configuration
		\return counter value for the event counter
	 */
	quint64	events() {return m_counter[EVCT]->value();}

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

public slots:
	void analyzeBuffer(DATA_PACKET &pd);

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
	MesydaqCounter	*m_counter[8];

	MesydaqTimer	*m_timer;

	int		m_rateTimer;

	int 		m_onlineTimer;

	QRect		m_roi;

	quint32		m_packages;

	quint64		m_triggers;
};

#endif

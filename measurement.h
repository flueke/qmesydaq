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

#include "mesydaqobject.h"
#include "counter.h"
#include "structures.h"
#include "mdefines.h"

class Histogram;
class Mesydaq2;

/**
	@author Gregor Montermann <g.montermann@mesytec.com>
*/
class Measurement : public MesydaqObject
{
Q_OBJECT
public:
	Measurement(Mesydaq2 *mesy, QObject *parent = 0);

	~Measurement();

	ulong	getMeastime(void);

	void	setCurrentTime(quint64 msecs);
	void	stop(quint64 time);
	void	start(quint64 time);
	void	cont();

	void	calcRates();
	void	setCounter(quint32 cNum, quint64 val);
	ulong	getRate(quint8 cNum);
	quint64	getCounter(quint8 cNum);
	void	calcMeanRates();
	quint8	isOk(void);
	void	setOnline(bool truth);
	ulong	getPreset(quint8 cNum);
	void	setPreset(quint8 cNum, quint64 prval, bool mast);
	void	setRunnumber(quint32 number);
	void	setListmode(bool truth);
	void	setRemote(bool truth);
	bool	remoteStart(void);

	void	setCarStep(quint32 step) {m_carStep = step;}
	void	setCarHistSize(quint32 h, quint32 w)
	{
		m_carHistWidth = w;
		m_carHistHeight = h;
	}
	quint32	getCarHeight() {return m_carHistHeight;}
	quint32	getCarWidth() {return m_carHistWidth;}

	quint32	getRun();
	
	bool	isMaster(quint8 cNum);
	void	clearCounter(quint8 cNum);
	bool	hasStopped(quint8 cNum);
	void	copyCounters(void);
	bool	limitReached(quint8 cNum);

	void 	readListfile(QString readfilename);

	quint64	mon1() {return m_counter[M1CT].value();}
	quint64	mon2() {return m_counter[M2CT].value();}
	quint64	events() {return m_counter[EVCT].value();}

	void writeHistograms(const QString &name);

	void clearAllHist(void);

	void clearChanHist(quint16 chan);

	void copyData(quint32 line, ulong *data);

public slots:
	void analyzeBuffer(DATA_PACKET &pd);

private slots:
	void requestStop(void);

signals:
	void stop();

	void acqListfile(bool);

	void setCountlimit(quint8, ulong);

	void draw();
	
private:
	static const quint16  	sep0 = 0x0000;
	static const quint16  	sep5 = 0x5555;    
	static const quint16  	sepA = 0xAAAA;
	static const quint16  	sepF = 0xFFFF;

	//! Access to hardware
	Mesydaq2	*m_mesydaq;


	//! Histogram buffer
	Histogram	*m_hist;

	quint64 	m_starttime_msec;
	quint64 	m_meastime_msec;

	bool 		m_running;
	bool 		m_stopping;
	bool 		m_rateflag;
	bool 		m_online;
	bool 		m_working; 		// it's set to true and nothing else ????
	bool 		m_listmode;
	bool 		m_remote;

	//! definitions of the counters
	MesydaqCounter	m_counter[8];

	//! CARESS 
	quint32 	m_runNumber;
	quint32 	m_carHistHeight;
	quint32 	m_carHistWidth;
	quint32 	m_carStep;
};

#endif

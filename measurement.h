/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann   *
 *   g.montermann@mesytec.com   *
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

class mesydaq2;

/**
	@author Gregor Montermann <g.montermann@mesytec.com>
*/
class Measurement : public QObject
{
Q_OBJECT
public:
	Measurement(QObject *parent = 0);

	~Measurement();

	ulong	getMeastime(void);

	void	setCurrentTime(ulong msecs);
	void	stop(ulong time);
	void	start(ulong time);
	void	calcRates();
	void	setCounter(quint32 cNum, quint64 val);
	ulong	getRate(quint8 cNum);
	quint64	getCounter(quint8 cNum);
	void	calcMeanRates();
	quint8	isOk(void);
	void	setOnline(bool truth);
	ulong	getPreset(quint8 cNum);
	void	setPreset(quint8 cNum, ulong prval, bool mast);
	void	setRunnumber(quint32 number);
	void	setListmode(bool truth);
	void	setCarHistSize(quint32 h, quint32 w);
	void	setRemote(bool truth);
	bool	remoteStart(void);
	void	setStep(quint32 step);
	quint32	getCarHeight();
	quint32	getCarWidth();
	quint32	getRun();
	bool	isMaster(quint8 cNum);
	void	clearCounter(quint8 cNum);
	bool	hasStopped(quint8 cNum);
	void	copyCounters(void);
	bool	limitReached(quint8 cNum);

	void	incMon1() {++m_mon1;}
	void	incMon2() {++m_mon2;}
	void	incEvents() {++m_events;}

	ulong	mon1() {return m_mon1;}
	ulong	mon2() {return m_mon2;}
	ulong	events() {return m_events;}

signals:
	void protocol(QString, quint8 = 1);
	void stop();
	void acqListfile(bool);
	void setCountlimit(quint8, ulong);
	
private:
	ulong 	m_events;
	ulong 	m_mon1;    
	ulong 	m_mon2;

	ulong 	m_starttime_msec;
	ulong 	m_meastime_msec;
	ulong 	m_ratetime_msec;
	bool 	m_running;
	bool 	m_stopping;
	bool 	m_rateflag;
	bool 	m_online;
	bool 	m_working; // it's set to true and nothing else ????
	bool 	m_listmode;
	bool 	m_remote;

	quint64 m_counter[2][8];
	quint64 m_counterStart[8];
	quint64 m_counterOffset[8];
	quint64 m_preset[8];
	bool 	m_master[8];
	bool 	m_stopped[8];
	ulong 	m_rate[11][8];
	quint8 	m_ratecount[8];
	quint8 	m_ratepointer[8];

	quint32 m_runNumber;
	quint32 m_carHistHeight;
	quint32 m_carHistWidth;
	quint32 m_carStep;
};

#endif

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
#ifndef COUNTER_H
#define COUNTER_H

#include <QQueue>

#include "mesydaqobject.h"

#include "mdefines.h"

class MesydaqCounter : public MesydaqObject
{
Q_OBJECT

public:
	//! constructor
	MesydaqCounter(); 

	virtual void start(quint64 time);

	virtual void stop(quint64 time);

	/** 
	 * increments the counter if not limit reached and emits the signal
	 * stop() if the limit is reached
	 */
	void operator++(void); 

	operator quint64() {return value();}

	//! checks whether the the limit is reached or not
	bool isStopped(void);

	/**
	 * sets the counter value
	 * 
	 * \param val new counter value
	 */
	void set(quint64 val) 
	{
   		m_value = m_offset + val - m_start;
	}
	
	//! sets the counter value to zero
	void reset(void); 

	/**
	 * Sets the counter limit if this counter is configured as master otherwise the limit will be cleared. 
	 * If this limit will be reached the signal stop() will be emitted. 
	 *
	 * \param val counter limit, if the value is zero no limit will be set
	 */
	void setLimit(quint64 val) {m_limit = m_master ? val : 0;}

	//! \return the current counter limit if limit is 0 no limit is set
	quint64 limit(void) {return m_limit;}

	//! \return current counter value
	quint64 value(void) {return m_value;}

	//! \return whether this counter is master or not
	bool isMaster(void) {return m_master;}

	/**
	 * Sets this counter as master or slave. If the counter is not master
	 * the limit will be cleared.
	 *
	 * \param val if true then this counter is master else not
	 */
	void setMaster(const bool val = true);

	virtual void setTime(quint64 val);

	quint64 rate(void);

	void calcRate(void);

signals:
	//! the counter has to be stopped
	void stop();

protected:
	//! current counter value
	quint64 m_value;

	//! store the value during the last calculation of the rate
	quint64 m_lastValue;

	//! current counter limit
	quint64 m_limit;

	//! is this counter a master counter
	bool	m_master;

	quint64 m_start;

	quint64 m_offset;

	quint64 m_meastime_msec;
	
	quint64 m_ratetime_msec;

	bool	m_rateflag;

	QQueue<quint64>	m_rate;

	quint8	m_ratepointer;
};

class MesydaqTimer : public MesydaqCounter
{
Q_OBJECT
public:
	MesydaqTimer();

	virtual void start(quint64 val);

	virtual void setTime(quint64 val);
};

#endif // COUNTER_H

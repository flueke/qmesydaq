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
#include "counter.h"

MesydaqCounter :: MesydaqCounter() 
	: MesydaqObject()
	, m_value(0)
	, m_limit(0)
	, m_start(0)
	, m_offset(0)
	, m_meastime_msec(0)
	, m_ratetime_msec(0)
	, m_rateflag(false)
	, m_ratepointer(0)
{
}

void MesydaqCounter::start(quint64 )
{
	reset();
	m_rateflag = false;
}

void MesydaqCounter::stop(quint64 )
{
}

void MesydaqCounter::operator++(void)
{
	if (!m_limit || m_value < m_limit)
		++m_value;
	if (isStopped())
		emit stop();
}

bool MesydaqCounter::isStopped(void) 
{
	if (!m_limit)
		return false;
	return (m_value >= m_limit);
}

void MesydaqCounter::setMaster(const bool val) 
{
	m_master = val;
	if (!m_master)
		setLimit(0);
}

void MesydaqCounter::reset(void) 
{
	set(0);
	m_offset = 0;
	m_start = 0;
	m_rate.clear();
}

void MesydaqCounter::setTime(quint64 val)
{
	m_meastime_msec = val;
}

void MesydaqCounter::calcRate()
{
	if(m_meastime_msec == 0)
		return;
	if(m_ratetime_msec >= m_meastime_msec)
	{
		m_ratetime_msec = m_meastime_msec;
		return;
	}
	if (m_rateflag == true)
	{
		quint64 tval = (m_meastime_msec - m_ratetime_msec);
		if (m_rate.size() > 10)
			m_rate.dequeue();
		if (m_rate.size() > 1)
			m_rate.enqueue((m_value - m_lastValue) * 1000 / tval);
		else
			m_rate.enqueue(0);
		m_lastValue = m_value;
	}
	m_ratetime_msec = m_meastime_msec;
	m_rateflag = true;
}

quint64 MesydaqCounter::rate()
{
	m_rate.last();
}

MesydaqTimer::MesydaqTimer()
	: MesydaqCounter()
{
}

void MesydaqTimer::start(quint64 time)
{
	m_start = time / 1000;
	set(m_offset);
}

void MesydaqTimer::setTime(quint64 time)
{
	set(time / 1000);
}
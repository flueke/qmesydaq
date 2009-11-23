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
	, m_meanRate(0)
	, m_ratepointer(0)
	, m_lastValue(0)
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

void MesydaqCounter::set(quint64 val)
{
	protocol(tr("MesydaqCounter::set(%1)").arg(val));
	m_value = m_offset + val - m_start;
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
	m_value = 0;
	m_offset = 0;
	m_start = 0;
	m_lastValue = 0;
	m_rate.clear();
}

void MesydaqCounter::setTime(quint64 val)
{
	m_meastime_msec = val;
}

void MesydaqCounter::calcRate(void)
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
		if (m_meastime_msec > m_ratetime_msec)
		{
			quint64 tval = (m_meastime_msec - m_ratetime_msec);
			if (m_rate.size() > 10)
				m_rate.removeFirst();
			if (m_rate.size() > 1 && (m_value >= m_lastValue))
				m_rate.enqueue((m_value - m_lastValue) * 1000 / tval);
			else
			{
				if (m_value)
					protocol(tr("size : %3; m_value : %1, m_lastValue : %2").arg(m_value).arg(m_lastValue).arg(m_rate.size()));
				m_rate.enqueue(0);
			}
		}
		else
			protocol(tr("m_meastime_msec : %1,  m_ratetime_msec %2").arg(m_meastime_msec).arg(m_ratetime_msec));
		m_lastValue = m_value;
	}
	m_ratetime_msec = m_meastime_msec;
	m_rateflag = true;
}

void MesydaqCounter::calcMeanRate(void)
{
	quint64 val2(0);
	for(quint16 c = 1; c < m_rate.size(); ++c)
		val2 += m_rate[c];
	if(m_rate.size() > 2)
		m_meanRate = val2 / (m_rate.size() - 1);
	else
		m_meanRate = 0;			
}

quint64 MesydaqCounter::rate()
{
	return m_meanRate;
}

MesydaqTimer::MesydaqTimer()
	: MesydaqCounter()
{
}

void MesydaqTimer::start(quint64 val)
{
	m_start = val;
	m_value = 0;
	protocol(tr("timer start1"), INFO);
}

quint64 MesydaqTimer::value(void) 
{
	return m_value;
}

void MesydaqTimer::setTime(quint64 val)
{
	if (!isStopped())
		m_value = val - m_start;
	if (isStopped())
	{
		protocol(tr("timer stop"), INFO);
		emit stop();
	}
}

/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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
#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <cmath>

class TubeRange  
{
public:
	TubeRange()
		: m_min(0.0)
		, m_max(0.0)
	{
	}

	TubeRange(qreal min, qreal max)
		: m_min(min)
		, m_max(max)
	{
	}

	TubeRange(qreal min)
		: m_min(min)
		, m_max(min)
	{
	}

	TubeRange(const TubeRange &tr)
		: m_min(tr.m_min)
		, m_max(tr.m_max)
	{
	}

	void setMax(const qreal max)
	{
		m_max = max;
	}

	qreal start(void) const
	{
		return m_min;
	}

	qreal height(void) const
	{
		return m_max - m_min;
	}
private:
	qreal	m_min;

	qreal	m_max;
};

class TubeCorrection
{
public:
	TubeCorrection()
		: m_calibScale(1.0)
		, m_shift(0)
		, m_detStart(0)
		, m_detEnd(0)
	{
	}

	TubeCorrection(const TubeRange &detRange, const TubeRange &tubeRange)
	{
		m_calibScale = detRange.height() / tubeRange.height();
		m_detStart = detRange.start();
		m_detEnd = m_detStart + detRange.height();
		m_shift = tubeRange.start() - m_detStart;
	}

	TubeCorrection(const TubeCorrection &tc)
	{
		m_calibScale = tc.m_calibScale;
		m_shift = tc.m_shift;
		m_detStart = tc.m_detStart;
		m_detEnd = tc.m_detEnd;
	}

	quint32 calibrate(const qint32 pos)
	{
		if (m_calibScale == 1.0)
			return pos;
		qint32 target = pos - m_shift;
		if (target >= qint32(m_detStart) && target <= qint32(m_detEnd))
		{
			return  quint32(::round(m_calibScale * (pos - m_shift)));
		}
		else
			return -1;
	}

private:
	qreal	m_calibScale;

	qint32	m_shift;

	quint32	m_detStart;

	quint32 m_detEnd;
};

#endif

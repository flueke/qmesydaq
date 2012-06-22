/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2011 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#include <QRectF>
#include <cmath>

class TubeRange : protected QRectF
{
public:
	TubeRange()
		: QRectF()
	{
	}

	TubeRange(qreal min, qreal max)
		: QRectF(QPointF(0, max), QSizeF(0, max - min))
	{
	}

	TubeRange(qreal min)
		: QRectF()
	{
		setBottom(min);
	}

	TubeRange(const TubeRange &tr)
		: QRectF(QPointF(tr.topLeft()), QSizeF(tr.size()))
	{
	}

	void setMax(const qreal max)
	{
		setTop(max);
	}

	qreal start(void) const
	{
		return bottom();
	}

	qreal height(void) const
	{
		return QRectF::height();
	}
};

class TubeCorrection
{
public:
	TubeCorrection()
		: m_calibScale(1.0)
		, m_shift(0)
		, m_detStart(0)
	{
	}

	TubeCorrection(const TubeRange &detRange, const TubeRange &tubeRange)
	{
		m_calibScale = detRange.height() / tubeRange.height();
		m_detStart = detRange.start();
		m_shift = tubeRange.start() - m_detStart;
	}

	TubeCorrection(const TubeCorrection &tc)
	{
		m_calibScale = tc.m_calibScale;
		m_shift = tc.m_shift;
		m_detStart = tc.m_detStart;
	}

	quint32 calibrate(const quint32 pos)
	{
		if (m_calibScale == 1.0)
			return pos;
		if (pos > m_detStart)
		{
			return  quint32(::round(m_calibScale * (pos - m_shift)));
		}
		else
			return 0;
	}

private:
	qreal	m_calibScale;

	qint32	m_shift;

	quint32	m_detStart;
};

#endif

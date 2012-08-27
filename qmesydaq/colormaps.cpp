/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "colormaps.h"

#include "logging.h"

#include <cmath>

MesydaqColorMap::MesydaqColorMap()
	: QwtLinearColorMap()
	, m_log(false)
{
}

void MesydaqColorMap::setLinearScaling(void)
{
	m_log = false;
}

void MesydaqColorMap::setLogarithmicScaling(void)
{
	m_log = true;
}

QColor MesydaqColorMap::color(const QwtDoubleInterval &interval, double value) const 
{
	if (m_log)
	{
		QwtDoubleInterval iv(interval);
		if (iv.minValue() < 1.0)
			iv.setMinValue(1.0);

		double 	minLog = ::log10(iv.minValue()),
			maxLog = ::log10(iv.maxValue()),
			valLog = ::log10(value);

		return QwtLinearColorMap::color(QwtDoubleInterval(minLog, maxLog), valLog);
	}
	else
		return QwtLinearColorMap::color(interval, value);
}

QRgb MesydaqColorMap::rgb(const QwtDoubleInterval &interval, double value) const
{
	if (m_log)
	{
		QwtDoubleInterval iv(interval);
		if (iv.minValue() < 1.0)
			iv.setMinValue(1.0);

		double 	minLog = ::log10(iv.minValue()),
			maxLog = ::log10(iv.maxValue()),
			valLog = ::log10(value);

		return QwtLinearColorMap::rgb(QwtDoubleInterval(minLog, maxLog), valLog);
	}
	else
		return QwtLinearColorMap::rgb(interval, value);
}

QwtColorMap *MesydaqColorMap::copy() const
{
	MesydaqColorMap *map = new MesydaqColorMap();
	*map = *this;
	map->m_log = this->m_log;
	return map;
}

StdColorMap::StdColorMap()
	: MesydaqColorMap() 
{
	setColorInterval(Qt::darkBlue, Qt::darkRed);
	addColorStop(0.143, Qt::blue);
	addColorStop(0.286, Qt::darkCyan);
	addColorStop(0.429, Qt::cyan);
	addColorStop(0.572, Qt::green);
	addColorStop(0.715, Qt::yellow);
	addColorStop(0.858, Qt::red);
}

JetColorMap::JetColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 127), QColor(127, 0, 0));
	addColorStop(0.110, QColor(0, 0, 255));	
	addColorStop(0.125, QColor(0, 0, 255));	
	addColorStop(0.340, QColor(0, 221, 255));	
	addColorStop(0.350, QColor(0, 229, 246));	
	addColorStop(0.375, QColor(4, 255, 226));	
	addColorStop(0.640, QColor(237, 255, 8));	
	addColorStop(0.650, QColor(246, 245, 0));	
	addColorStop(0.660, QColor(255, 236, 0));	
	addColorStop(0.890, QColor(255, 18, 0));	
	addColorStop(0.910, QColor(226, 0, 0));	
}

HotColorMap::HotColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(10, 0, 0), QColor(255, 255, 255));
	addColorStop(0.365079, QColor(255, 0, 0));
	addColorStop(0.746032, QColor(255, 255, 0));
}

HsvColorMap::HsvColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(255, 0, 0), QColor(255, 0, 23));
	addColorStop(0.158730, QColor(255, 239, 0));
	addColorStop(0.174603, QColor(247, 255, 0));
	addColorStop(0.333333, QColor(7, 255, 0));
	addColorStop(0.349206, QColor(0, 255, 15));
	addColorStop(0.507937, QColor(0, 255, 255));
	addColorStop(0.666667, QColor(0, 15, 255));
	addColorStop(0.682540, QColor(7, 0, 255));
	addColorStop(0.841270, QColor(247, 0, 255));
	addColorStop(0.857143, QColor(255, 0, 239));
}

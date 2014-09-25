/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2014 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

SpringColorMap::SpringColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(255, 0, 255), QColor(255, 255, 0));
}

SummerColorMap::SummerColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 127, 104), QColor(255, 255, 104));
}

AutumnColorMap::AutumnColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(255, 0, 0), QColor(255, 255, 0));
}

WinterColorMap::WinterColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 255), QColor(0, 255, 127));
}

BoneColorMap::BoneColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 255), QColor(255, 255, 255));
	addColorStop(0.365079, QColor(124, 81, 113));
	addColorStop(0.746032, QColor(166, 198, 195));
}

CoolColorMap::CoolColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 255, 255), QColor(255, 0, 255));
}

CopperColorMap::CopperColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 0), QColor(255, 199, 126));
	addColorStop(0.809524, QColor(255, 161, 102));
}

GrayColorMap::GrayColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 0), QColor(255, 255, 255));
}

SpectralColorMap::SpectralColorMap()
	: MesydaqColorMap()
{
	setColorInterval(QColor(0, 0, 0), QColor(204, 204, 204));
	addColorStop(0.05, QColor(119, 0, 35));
	addColorStop(0.1, QColor(0, 0, 170));
	addColorStop(0.15, QColor(0, 0, 221));
	addColorStop(0.25, QColor(0, 119, 221));
	addColorStop(0.3, QColor(0, 153, 221));
	addColorStop(0.35, QColor(0, 170, 170));
	addColorStop(0.4, QColor(0, 170, 135));
	addColorStop(0.45, QColor(0, 153, 0));
	addColorStop(0.5, QColor(0, 186, 0));
	addColorStop(0.55, QColor(0, 221, 0));
	addColorStop(0.6, QColor(0, 255, 0));
	addColorStop(0.65, QColor(186, 255, 0));
	addColorStop(0.7, QColor(237, 237, 0));
	addColorStop(0.75, QColor(255, 204, 0));
	addColorStop(0.8, QColor(255, 153, 0));
	addColorStop(0.85, QColor(255, 0, 0));
	addColorStop(0.9, QColor(221, 0, 0));
	addColorStop(0.95, QColor(204, 0, 0));
}

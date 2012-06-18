/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Kr¿ger <jens.krueger@frm2.tum.de>     *
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

#include "data.h"

#include <cmath>

double myspectrum(double value)
{
	return 1.1 + sin(value);
}

SpectrumData::SpectrumData(double(*y)(double), size_t size)
	: d_size(size)
	, d_y(y)
{
}

QwtData *SpectrumData::copy() const
{
	return new SpectrumData(d_y, d_size);
}

size_t SpectrumData::size() const
{
	return d_size;
}

double SpectrumData::x(size_t i) const
{
	return 0.1 * i;
}
	
double SpectrumData::y(size_t i) const
{
	return d_y(x(i));
}

HistogramData::HistogramData()
	: QwtRasterData(QwtDoubleRect(-1.5, -1.5, 3.0, 3.0))
{
}

QwtRasterData *HistogramData::copy() const
{
       	return new HistogramData();
}

QwtDoubleInterval HistogramData::range() const
{
       	return QwtDoubleInterval(0.0, 10.0);
}

double HistogramData::value(double x, double y) const
{
	const double c = 0.842;

	const double v1 = x * x + (y-c) * (y+c);
	const double v2 = x * (y+c) + x * (y+c);

	return 1.0 / (v1 * v1 + v2 * v2);
}


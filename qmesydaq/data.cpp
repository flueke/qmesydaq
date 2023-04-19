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

#include "data.h"

#include <cmath>

double myspectrum(double value)
{
	return 1.1 + sin(value);
}

SpectrumData::SpectrumData(size_t size)
	: d_size(size)
{
	m_data = new double[size];
}

SpectrumData::SpectrumData()
	: d_size(0)
	, m_data(NULL)
{
}

SpectrumData::SpectrumData(const SpectrumData &data)
{
	this->d_size = data.d_size;
	m_data = new double[this->d_size];
	memcpy(this->m_data, data.m_data, d_size * sizeof(double));
}

SpectrumData::~SpectrumData()
{
	delete [] m_data;
}

#if QWT_VERSION < 0x060000
QwtData *SpectrumData::copy() const
{
	return new SpectrumData(*this);
}
#endif

size_t SpectrumData::size() const
{
	return d_size;
}

double SpectrumData::x(size_t i) const
{
	return i;
}

double SpectrumData::y(size_t i) const
{
	return m_data[i];
}

QwtDoubleRect SpectrumData::boundingRect() const
{
#if QWT_VERSION < 0x060000
	QwtDoubleRect rect = QwtData::boundingRect();
#else
    QwtDoubleRect rect;
#endif
	if (rect.isEmpty())
		 rect = QwtDoubleRect(0.0, 0.1, 1.0, 1.0);
	return rect;
}

#if QWT_VERSION >= 0x060000
QPointF SpectrumData::sample(size_t i) const
{
	return QPointF(x(i), y(i));
}
#endif

HistogramData::HistogramData()
#if QWT_VERSION < 0x060000
	: QwtRasterData(QRectF(-1.5, -1.5, 3.0, 3.0))
#endif
{
#if QWT_VERSION >= 0x060000
	setInterval(Qt::XAxis, QwtInterval(-1.5, 1.5));
	setInterval(Qt::YAxis, QwtInterval(-1.5, 1.5));
#endif
}

#if QWT_VERSION < 0x060000
QwtRasterData *HistogramData::copy() const
{
	return new HistogramData();
}
#endif

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

DiffractogramData::DiffractogramData(const SpectrumData &data)
{
	d_size = data.size();
}

DiffractogramData::DiffractogramData(const HistogramData &data, int dir)
	: SpectrumData()
{
#if QWT_VERSION < 0x060000
	QRectF r = data.boundingRect();
#else
	QRectF r(0, 0, data.interval(Qt::XAxis).width(), data.interval(Qt::YAxis).width());
#endif
	if (dir == Data::xDir)
	{
		d_size = size_t(r.width() / 0.1);
		m_data = new double[d_size];
		for (size_t i = 0; i < d_size; ++i)
			m_data[i] = data.value(i * 0.1 - r.width() / 2, 0);
	}
	else
	{
		d_size = size_t(r.height() / 0.1);
		m_data = new double[d_size];
		for (size_t i = 0; i < d_size; ++i)
			m_data[i] = data.value(0, i * 0.1 - r.height() / 2);
	}
}

#if QWT_VERSION < 0x060000
QwtData *DiffractogramData::copy() const
{
	return new DiffractogramData(*this);
}
#endif

size_t DiffractogramData::size() const
{
	return d_size;
}

double DiffractogramData::x(size_t i) const
{
	return i;
}

double DiffractogramData::y(size_t i) const
{
	return m_data[i];
}


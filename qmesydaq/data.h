/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#ifndef _DATA_H_
#define _DATA_H

#include <qwt_global.h>

#if QWT_VERSION < 0x060000
#	include <qwt_data.h>
#else
#	include <qwt_series_data.h>
#	define QwtDoubleRect QRectF
#	define QwtDoubleInterval QwtInterval
#endif
#include <qwt_raster_data.h>

double myspectrum(double value);

namespace Data
{
	enum Direction {
		xDir = 0,
		yDir,
	};
}

#if QWT_VERSION < 0x060000
class SpectrumData: public QwtData
#else
class SpectrumData: public QwtSeriesData<QPointF>
#endif
{
    // The x values depend on its index and the y values
    // can be calculated from the corresponding x value.
    // So we don´t need to store the values.
    // Such an implementation is slower because every point
    // has to be recalculated for every replot, but it demonstrates how
    // QwtData can be used.

public:
	SpectrumData(size_t size);

	SpectrumData(const SpectrumData &);

	SpectrumData();

	~SpectrumData();

#if QWT_VERSION < 0x060000
	virtual QwtData *copy() const;
#endif

	virtual size_t size() const;

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

#if QWT_VERSION >= 0x060000
	virtual QPointF sample(size_t i) const;
#endif

	//! \return the bounding rectangle of the data.
	QwtDoubleRect boundingRect() const;

protected:
	size_t 	d_size;

	double	*m_data;
};

class HistogramData: public QwtRasterData
{
public:
	HistogramData();

#if QWT_VERSION < 0x060000
	virtual QwtRasterData *copy() const;
#endif

	virtual QwtDoubleInterval range() const;

	virtual double value(double x, double y) const;

    void setInterval(Qt::Axis axis, const QwtInterval &interval)
    {
        if (0 <= axis && axis < 3)
            m_intervals[axis] = interval;
    }

    QwtInterval interval(Qt::Axis axis) const override
    {
        if (0 <= axis && axis < 3)
            return m_intervals[axis];
        return {};
    }

private:
    QwtInterval m_intervals[3];
};

class DiffractogramData : public SpectrumData
{
public:
	DiffractogramData(const SpectrumData &);

	DiffractogramData(const HistogramData &, int);

#if QWT_VERSION < 0x060000
	virtual QwtData *copy() const;
#endif

	virtual size_t size() const;

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

};

#endif

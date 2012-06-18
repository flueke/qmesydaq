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

#ifndef _DATA_H_
#define _DATA_H

#include <qwt_data.h>
#include <qwt_raster_data.h>

#include <cmath>

class SpectrumData: public QwtData
{
    // The x values depend on its index and the y values
    // can be calculated from the corresponding x value. 
    // So we don´t need to store the values.
    // Such an implementation is slower because every point 
    // has to be recalculated for every replot, but it demonstrates how
    // QwtData can be used.

public:
	SpectrumData(double(*y)(double), size_t size)
		: d_size(size)
		, d_y(y)
	{
	}

	virtual QwtData *copy() const
	{
        	return new SpectrumData(d_y, d_size);
	}

	virtual size_t size() const
	{
		return d_size;
	}

	virtual double x(size_t i) const
	{
		return 0.1 * i;
	}
	
	virtual double y(size_t i) const
	{
		return d_y(x(i));
	}
private:
	size_t d_size;
	double(*d_y)(double);
};

class HistogramData: public QwtRasterData
{
public:
	HistogramData()
		: QwtRasterData(QwtDoubleRect(-1.5, -1.5, 3.0, 3.0))
	{
	}

	virtual QwtRasterData *copy() const
	{
        	return new HistogramData();
	}

	virtual QwtDoubleInterval range() const
	{
        	return QwtDoubleInterval(0.0, 10.0);
	}

	virtual double value(double x, double y) const
	{
		const double c = 0.842;

		const double v1 = x * x + (y-c) * (y+c);
		const double v2 = x * (y+c) + x * (y+c);

		return 1.0 / (v1 * v1 + v2 * v2);
	}
};

#endif

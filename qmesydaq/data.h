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

#ifndef _DATA_H_
#define _DATA_H

#include <qwt_data.h>
#include <qwt_raster_data.h>

double myspectrum(double value);

namespace Data
{
	enum Direction {
		xDir = 0,
		yDir,
	};
}

class SpectrumData: public QwtData
{
    // The x values depend on its index and the y values
    // can be calculated from the corresponding x value.
    // So we don´t need to store the values.
    // Such an implementation is slower because every point
    // has to be recalculated for every replot, but it demonstrates how
    // QwtData can be used.

public:
	SpectrumData(size_t size);

	SpectrumData(const SpectrumData  &);

	SpectrumData();

	~SpectrumData();

	virtual QwtData *copy() const;

	virtual size_t size() const;

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

protected:
	size_t 	d_size;

	double	*m_data;
};

class HistogramData: public QwtRasterData
{
public:
	HistogramData();

	virtual QwtRasterData *copy() const;

	virtual QwtDoubleInterval range() const;

	virtual double value(double x, double y) const;
};

class DiffractogramData : public SpectrumData
{
public:
	DiffractogramData(const SpectrumData &);

	DiffractogramData(const HistogramData &, int );

	virtual QwtData *copy() const;

	virtual size_t size() const;

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

};

#endif

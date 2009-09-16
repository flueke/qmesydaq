/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
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

#ifndef MESYDAQ_DATA_H
#define MESYDAQ_DATA_H

#include <QVector>

#include <qwt_data.h>
#include <qwt_raster_data.h>

class Spectrum;
class Histogram;

class MesydaqSpectrumData : public QwtData
{
public:
	MesydaqSpectrumData();

	virtual QwtData *copy() const;

	virtual size_t size() const; 

	virtual double x(size_t i) const;

	virtual double y(size_t i) const;

	void setData(Spectrum *data);

	quint32 max(void);

private:
	Spectrum	*m_spectrum;
};

class MesydaqHistogramData : public QwtRasterData
{
public:
	MesydaqHistogramData();

	QwtRasterData *copy() const;

	QwtDoubleInterval range() const;

	double value(double x, double y) const;

	void setData(Histogram *data);
	
private:
	Histogram	*m_histogram;

};

#endif

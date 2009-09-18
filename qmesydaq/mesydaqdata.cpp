/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

#include "mesydaqdata.h"
#include "histogram.h"

MesydaqSpectrumData::MesydaqSpectrumData()
	: QwtData()
	, m_spectrum(NULL)
{
}

QwtData *MesydaqSpectrumData::copy() const
{
	MesydaqSpectrumData *tmp = new MesydaqSpectrumData();
	tmp->setData(m_spectrum);
	return tmp;
}

size_t MesydaqSpectrumData::size() const
{
	if (m_spectrum) 
		return m_spectrum->width();
	else
		return 0;
}

double MesydaqSpectrumData::x(size_t i) const
{
	return i;
}

double MesydaqSpectrumData::y(size_t i) const
{
	if (m_spectrum)
		return m_spectrum->value(i);
	else
		return 0.0;
}

void MesydaqSpectrumData::setData(Spectrum *data)
{
	m_spectrum = data;
}

quint32 MesydaqSpectrumData::max(void)
{
	if (m_spectrum)
		return m_spectrum->max();
	else
		return 0;
}

MesydaqHistogramData::MesydaqHistogramData() 
	: QwtRasterData()
	, m_histogram(NULL)
{
}

QwtRasterData *MesydaqHistogramData::copy() const
{
	MesydaqHistogramData *tmp = new MesydaqHistogramData();
	tmp->setData(m_histogram);
	return tmp;
}

QwtDoubleInterval MesydaqHistogramData::range() const
{
	if (m_histogram)
	{
		double m = double(m_histogram->max());
		return QwtDoubleInterval(0.0, m);
	}
	else
		return QwtDoubleInterval(0.0, 1.0);
}

double MesydaqHistogramData::value(double x, double y) const
{
	if (m_histogram)
		return m_histogram->value(quint16(y), quint16(x));
	else
		return 0.0;
}

void MesydaqHistogramData::setData(Histogram *data)
{
	m_histogram = data;
}
/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2019 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include "spectrum.h"
#include "logging.h"
#include "stdafx.h"
#include <cmath>
#include <algorithm>

/**
    \fn Spectrum::Spectrum(const quint16 bins)

    constructor

    \param bins number of points in the spectrum
 */
Spectrum::Spectrum(const quint16 bins)
	: QObject()
	, m_data(NULL)
	, m_maximumPos(0)
	, m_meanCount(0)
	, m_totalCounts(0)
	, m_meanPos(0)
	, m_autoResize(false)
	, m_width(0)
{
	setWidth(bins);
	clear();
}

//! copy constructor
Spectrum::Spectrum(const Spectrum &src)
	: QObject()
	, m_maximumPos(src.m_maximumPos)
	, m_meanCount(src.m_meanCount)
	, m_totalCounts(src.m_totalCounts)
	, m_meanPos(src.m_meanPos)
	, m_autoResize(src.m_autoResize)
	, m_width(src.m_width)
{
	memcpy(m_floatingMean, src.m_floatingMean, sizeof(m_floatingMean));
	m_data = (quint64*) malloc(m_width * sizeof(*m_data));
	if (m_data != NULL)
		memcpy(m_data, src.m_data, m_width * sizeof(*m_data));
}

//! copy operator
Spectrum& Spectrum::operator=(const Spectrum &src)
{
	m_maximumPos   = src.m_maximumPos;
	m_meanCount    = src.m_meanCount;
	m_totalCounts  = src.m_totalCounts;
	m_meanPos      = src.m_meanPos;
	memcpy(m_floatingMean, src.m_floatingMean, sizeof(m_floatingMean));
	m_autoResize   = src.m_autoResize;
	m_width        = src.m_width;
	m_data = (quint64*) malloc(m_width * sizeof(m_data[0]));
	if (m_data != NULL)
		memcpy(m_data, src.m_data, m_width * sizeof(m_data[0]));
	return *this;
}

//! destructor
Spectrum::~Spectrum()
{
	free(m_data);
}

/*!
    \fn Spectrum::incVal(const quint16 bin)

    add a event add position bin

    \param bin position inside the spectrum to increment
    \return true or false if successful or not
 */
bool Spectrum::incVal(const quint16 bin)
{
	if (!checkBin(bin))
		return false;

	quint64 *tmp(m_data + bin);
	++m_totalCounts;
	++(*tmp);
	calcMaximumPosition(bin);
	calcFloatingMean(bin);
	return true;
}

bool Spectrum::checkBin(const quint16 bin)
{
	if (bin < m_width)
		return true;
	if (m_autoResize)
		setWidth(bin + 1);
	else
		return false;
	return bin < m_width;
}

void Spectrum::calcMaximumPosition(const quint16 bin)
{
	if (bin < m_width)
		if (*(m_data + bin) > *(m_data + m_maximumPos))
			m_maximumPos = bin;
}

void Spectrum::calcFloatingMean(const quint16 bin)
{
	// Fill the ring buffer with a size of 256
	m_floatingMean[m_meanPos & 0xFF] = bin;
	++m_meanPos;
	// collect the number of used cells in the ring buffer
	if(m_meanCount < 255)
		++m_meanCount;
}

/*!
    \fn Spectrum::setValue(const quint16 bin, const quint64 val)

    sets the events at position bin

    \param bin position inside the spectrum to set
    \param val events
    \return true or false if successful or not
 */
bool Spectrum::setValue(const quint16 bin, const quint64 val)
{
	if (!checkBin(bin))
		return false;

	m_totalCounts -= m_data[bin];
	m_data[bin] = val;
	m_totalCounts += val;
	calcMaximumPosition(bin);
	calcFloatingMean(bin);
	return true;
}

/*!
    \fn Spectrum::addValue(quint16 bin, quint64 val)

    adds events at position bin

    \param bin position inside the spectrum to set
    \param val events
    \return true or false if successful or not
 */
bool Spectrum::addValue(const quint16 bin, const quint64 val)
{
	if (!checkBin(bin))
		return false;

	m_data[bin] += val;
	m_totalCounts += val;
	calcMaximumPosition(bin);
	calcFloatingMean(bin);
	return true;
}

/*!
    \fn Spectrum::clear(void)

    clear the spectrum
 */
void Spectrum::clear(void)
{
	memset(m_data, 0, sizeof(quint64) * m_width);
	memset(m_floatingMean, 0, sizeof(m_floatingMean));
	m_totalCounts = m_maximumPos = m_meanCount = m_meanPos = 0;
}

/*!
    \fn Spectrum::mean(float &s)

    calculates the mean value and standard deviation of the mean value

    \param s standard deviation of the floating mean value
    \return floating mean value
 */
float Spectrum::mean(float &s)
{
	float m = 0;
	if (m_meanCount > 0)
	{
		for (quint8 c = 0; c < m_meanCount; ++c)
			m += m_floatingMean[c];
		m /= m_meanCount;

		// calculate sigma
		for (quint8 c = 0; c < m_meanCount; c++)
		{
			float tmp = m_floatingMean[c] - m;
			s += tmp * tmp;
		}
		s = sqrt(s / m_meanCount);
		s *= 2.3; // ln(10)
	}
	else
		s = 0.0;
	return m;
}

/*!
    \fn void Spectrum::setWidth(const quint16 w);

    sets the width of a spectrum

    \param w width
 */
void Spectrum::setWidth(const quint16 w)
{
	if (w == m_width)
		return;
	resize(w);
}

void Spectrum::resize(const quint16 bins)
{
	if (bins == m_width)
		return;
	quint64* pNew = (quint64 *)realloc(m_data, bins * sizeof(quint64));
	if (pNew == NULL && bins > 0)
		return;
	m_data = pNew;
	if (bins > 0)
	{
		if (bins > m_width)
			memset(m_data + m_width, '\0', (bins - m_width) * sizeof(quint64));
	}
	m_width = bins;
}

quint64 Spectrum::value(const quint16 index) const
{
	if (index < m_width)
		return m_data[index];
	return 0;
}

quint64 Spectrum::max()
{
	return m_width ? m_data[m_maximumPos] : 0;
}

quint16 Spectrum::maxpos(void) const
{
	return m_maximumPos;
}

quint64 Spectrum::getTotalCounts(void) const
{
	return m_totalCounts;
}

quint64 Spectrum::getCounts(const QRectF &r) const
{
	quint64 sum(0);
	MSG_DEBUG << "getCounts(" << r << ")";
	int 	x = ceil(r.x()),   			// to ensure, that we always on the left edge
		w = floor(r.x() + r.width()) - x;	// This should be the right edge
	if (w < 1)		// we are inside one pixel
	{
		w = 1;
		if (r.width() > 1 || ceil(r.x()) == floor(r.x() + r.width()))
		{
			if (fabs(r.x() - x) > fabs(r.x() + r.width() - x))	// take the pixel with the bigger part
				x -= 1;
		}
		else
			x -= 1;
	}
	w += x;
	for (int i = x; i < w; ++i)
		sum += value(i);
	return sum;
}

quint16	Spectrum::width(void) const
{
	return m_width;
}

bool Spectrum::autoResize(void) const
{
	return m_autoResize;
}

void Spectrum::setAutoResize(const bool resize)
{
	m_autoResize = resize;
}

/*!
   constructor

   \param time number of time units for a time channel
   \param bins number of time channels
 */
TimeSpectrum::TimeSpectrum(const quint64 time, const quint16 bins)
	: Spectrum(bins)
	, m_time(time)
{
}

/*!
   \fn void TimeSpectrum::incVal(const quint64 time)

   Adds an event to the time channel. From the given time the time channel will be calculated
   and then the related event counter will be increased.

   \param time time in time units
 */
void TimeSpectrum::incVal(const quint64 time)
{
	quint16 bin = time / m_time;
	if (bin < width())
		Spectrum::incVal(bin);
}

/*!
    \fn bool TimeSpectrum::setValue(const quint64 time, const quint64 val)

    Sets the number of events to the time channel. From the given time the time channel will be calculated
    and then the related event counter will be set.

    \param time time in time units
    \param val new number of events

    \return setting was succesful or not
 */
bool TimeSpectrum::setValue(const quint64 time, const quint64 val)
{
	quint16 bin = time / m_time;
	if (bin < width())
		return Spectrum::setValue(bin, val);
	return false;
}

/*!
    \fn bool TimeSpectrum::addValue(const quint64 time, const quint64 val)

    Adds a number of events to the time channel. From the given time the time channel will be calculated
    and then the related event counter will be increased.

    \param time time in time units
    \param val number of events have to been added

    \return setting was succesful or not
 */
bool TimeSpectrum::addValue(const quint64 time, const quint64 val)
{
	quint16 bin = time / m_time;
	if (bin < width())
		return Spectrum::addValue(bin, val);
	return false;
}

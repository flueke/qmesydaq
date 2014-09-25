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

#include "histogram.h"
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
	memmove(m_floatingMean, src.m_floatingMean, sizeof(m_floatingMean));
	m_data = (quint64*) malloc(m_width * sizeof(*m_data));
	if (m_data != NULL)
		memmove(m_data, src.m_data, m_width * sizeof(*m_data));
}

//! copy operator
Spectrum& Spectrum::operator=(const Spectrum &src)
{
	m_maximumPos   = src.m_maximumPos;
	m_meanCount    = src.m_meanCount;
	m_totalCounts  = src.m_totalCounts;
	m_meanPos      = src.m_meanPos;
	memmove(m_floatingMean, src.m_floatingMean, sizeof(m_floatingMean));
	m_autoResize   = src.m_autoResize;
	m_width        = src.m_width;
	m_data = (quint64*) malloc(m_width * sizeof(m_data[0]));
	if (m_data != NULL)
		memmove(m_data, src.m_data, m_width * sizeof(m_data[0]));
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
#if defined(_MSC_VER)
#	pragma message("TODO mean value determination")
#else
#	warning TODO mean value determination
#endif
//! \todo mean value determination
	m_floatingMean[m_meanPos & 0xFF] = bin;
	++m_meanPos;
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
	if(m_meanCount > 0)
	{
		for(quint8 c = 0; c < m_meanCount; ++c)
			m += m_floatingMean[c];
		m /= m_meanCount;

		// calculate sigma
		for(quint8 c = 0; c < m_meanCount; c++)
		{
			float tmp = m_floatingMean[c] - m;
			s += tmp * tmp;
		}
		s = sqrt(s / m_meanCount);
		s *= 2.3; // ln(10)
	}
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

quint64 Spectrum::value(const quint16 index)
{
	if (index < m_width)
		return m_data[index];
	return 0;
}

/*!
    \fn Histogram::Histogram(const quint16 h, const quint16 w)

    constructor

    \param h number of channels (i.e. number of tubes)
    \param w number of bins (inside a tube)
 */
Histogram::Histogram(const quint16 w, const quint16 h)
	: QObject()
	, m_totalCounts(0)
	, m_data(NULL)
	, m_height(0)
	, m_width(0)
	, m_maximumPos(0)
	, m_autoResize(false)
	, m_minROI(0)
	, m_maxROI(0)
{
	resize(w, h);
	clear();
}

Histogram::Histogram(const Histogram &src)
	: QObject()
	, m_totalCounts(0)
	, m_data(NULL)
	, m_height(0)
	, m_width(0)
	, m_maximumPos(0)
	, m_autoResize(false)
	, m_minROI(0)
	, m_maxROI(0)
{
	*this = src;
}

Histogram& Histogram::operator=(const Histogram &src)
{
	m_totalCounts  = src.m_totalCounts;
	m_data1        = src.m_data1;
	for (int i = 0; i < m_width; ++i)
		delete m_data[i];
	m_data = (Spectrum**)realloc(m_data, src.m_width * sizeof(Spectrum*));
	if (m_data != NULL)
	{
		for (int i = 0; i < src.m_width; ++i)
		{
			Spectrum* pSrc = src.m_data[i];
			if (pSrc != NULL)
				m_data[i] = new Spectrum(*pSrc);
			else
				m_data[i] = NULL;
		}
	}
	m_dataKeys     = src.m_dataKeys;
	m_height       = src.m_height;
	m_width        = src.m_width;
	m_xSumSpectrum = src.m_xSumSpectrum;
	m_ySumSpectrum = src.m_ySumSpectrum;
	m_maximumPos   = src.m_maximumPos;
	m_autoResize   = src.m_autoResize;
	m_minROI       = src.m_minROI;
	m_maxROI       = src.m_maxROI;
	return *this;
}

//! destructor
Histogram::~Histogram()
{
	if (m_data != NULL)
	{
		for (int i = 0; i < m_width; ++i)
			delete m_data[i];
		free(m_data);
	}
}

/*!
	\fn Histogram::max(void) const

    \return the maximum of the whole histogram
*/
quint64 Histogram::max(void) const
{
        if (m_maximumPos < m_width)
		return m_height ? m_data[m_maximumPos]->max() : 0;
	return 0;
}

/*!
    \fn Histogram::value(const quint16 chan, const quint16 bin) const

	gives the counts of the cell x,y.

    The implementation is a little bit tricky. In the m_dataKeys list
    are stored all keys for the single tube spectra. The will mapped
    to a number of the tube. This saves some space in the memory.

    \param chan number of the bin
    \param bin number of the tube
    \return 0 rief the cell does not exist, otherwise the counts
 */
quint64 Histogram::value(const quint16 chan, const quint16 bin) const
{
	if (chan < m_width)
		return m_data[chan]->value(bin);
	return 0;
}

bool Histogram::checkChannel(const quint16 chan)
{
	if (chan < m_width)
		return true;
	if (m_autoResize)
		setWidth(chan + 1);
	else
		return false;
	return chan < m_width;
}

bool Histogram::checkBin(const quint16 bin)
{
	if (bin < m_height)
		return true;
	if (m_autoResize)
		setHeight(bin + 1);
	else
		return false;
	return bin < m_height;
}

void Histogram::calcMaximumPosition(const quint16 chan)
{
	if (chan < m_height)
		if (m_data[chan]->max() > m_data[m_maximumPos]->max())
			m_maximumPos = chan;
}

/**
    \fn Histogram::incVal(const quint16 chan, const quint16 bin)

    increment value by 1 in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \return true if it was ok otherwise false
 */
bool Histogram::incVal(const quint16 chan, const quint16 bin)
{
	if (!checkChannel(chan))
		return false;
	if (!checkBin(bin))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts++;
// sum spectrum of all channels
	m_ySumSpectrum.incVal(bin);
	m_xSumSpectrum.incVal(chan);
	m_data[chan]->incVal(bin);
	calcMaximumPosition(chan);
	return true;
}

/**
    \fn Histogram::setValue(const quint16 chan, const quint16 bin, const quint64 val)

    set the event value in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::setValue(const quint16 chan, const quint16 bin, const quint64 val)
{
	if (!checkChannel(chan))
		return false;
	if (!checkBin(bin))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts -= m_data[chan]->value(bin);
	m_totalCounts += val;
// sum spectrum of all channels
	m_xSumSpectrum.setValue(chan, val);
	m_ySumSpectrum.setValue(bin, val);
	m_data[chan]->setValue(bin, val);
	calcMaximumPosition(chan);
	return true;
}

/**
    \fn Histogram::addValue(const quint16 chan, const quint16 bin, const quint64 val)

    set the event value in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::addValue(const quint16 chan, const quint16 bin, const quint64 val)
{
	if (!checkChannel(chan))
		return false;
	if (!checkBin(bin))
		return false;
// total counts of histogram (like monitor ??)
	m_totalCounts += val;
// sum spectrum of all channels
	m_xSumSpectrum.addValue(chan, val);
	m_ySumSpectrum.addValue(bin, val);
	m_data[chan]->addValue(bin, val);
	calcMaximumPosition(chan);
	return true;
}

/*!
    \fn Histogram::clear(const quint16 channel)

    clears the spectrum channel

    \param channel number of the spectrum to be cleared
 */
void Histogram::clear(const quint16 channel)
{
	m_totalCounts -= m_data[channel]->getTotalCounts();

#if defined(_MSC_VER)
#	pragma message("TODO remove the counts from the sum spectrum and total counts and adjust the new maximum")
#else
#	warning TODO remove the counts from the sum spectrum and total counts and adjust the new maximum
#endif
//! \todo remove the counts from the sum spectrum and total counts and adjust the new maximum
	m_data[channel]->clear();
}


/*!
    \fn Histogram::clear(void);

    clears the complete histogram
 */
void Histogram::clear(void)
{
	if (m_data != NULL)
		for (int i = 0; i < m_width; ++i)
		{
			Spectrum *value = m_data[i];
			Q_ASSERT_X(value != NULL, "Histogram::clear", "one of the spectra is NULL");
			if (value != NULL)
				value->clear();
		}
	m_xSumSpectrum.clear();
	m_ySumSpectrum.clear();
	m_totalCounts = 0;
	m_maximumPos = 0;
}

/*!
    \fn Histogram::getTotalCounts(void) const

    \return the sum of all counts
 */
quint64 Histogram::getTotalCounts(void) const
{
	return m_totalCounts;
}

/*!
    \fn quint64 Histogram::getCounts(const QRect &region) const

    \param region region of interest
    \return the number of events in the region
 */
quint64 Histogram::getCounts(const QRect &region) const
{
	quint64 tmp(0);
	int h = region.y() + region.height();
	int w = region.x() + region.width();

	for (int y = region.y(); y < h; ++y)
		for (int x = region.x(); x < w; ++x)
		{
			int v = value(x, y);
			tmp += v;
		}
	return tmp;
}

/*!
    \fn Histogram::spectrum(const quint16 channel)

    \param channel number of the tube
    \return the spectrum of the tube channel
 */
Spectrum *Histogram::spectrum(const quint16 channel)
{
	if (channel < m_height && m_data && m_data[channel])
		return m_data[channel];
	else
		return NULL;
}

/*!
    \fn Histogram::max(const quint16 channel) const

    \param channel number of the tube
    \return the maximum of the spectrum of the tube channel
 */
quint64 Histogram::max(const quint16 channel) const
{
	if (channel < m_height && m_data && m_data[channel])
		return m_data[channel]->max();
	else
		return 0;
}

/*!
    \fn Histogram::maxpos(const quint16 channel) const

    \param channel number of the tube
    \return the number of the bin in the tube channel
 */
quint16 Histogram::maxpos(const quint16 channel) const
{
	if (channel < m_height && m_data && m_data[channel])
		return m_data[channel]->maxpos();
	else
		return 0;
}

/*!
    \fn Histogram::getMean(const quint16 chan, float &m, float &s)

    gives the mean value and the standard deviation of the last events in the tube chan

    \param chan the number of the tube
    \param m mean value
    \param s standard deviation
 */
void Histogram::getMean(const quint16 chan, float &m, float &s)
{
	if (chan < m_height && m_data && m_data[chan])
		m = m_data[chan]->mean(s);
	else
		m = s = 0.0;
}

/*!
    \fn Histogram::getMean(float &m, float &s)

    gives the mean value and the standard deviation of the last events

    \param m mean value
    \param s standard deviation
 */
void Histogram::getMean(float &m, float &s)
{
	m = m_xSumSpectrum.mean(s);
}

/*!
    \fn	quint16 Histogram::height(void) const

    \return number of tubes
*/
quint16	Histogram::height(void) const
{
	return m_height;
}

/*!
    \fn void Histogram::setWidth(const quint16 w)

    sets the width of each cell

    \param w new witdh of histogram (or number of tubes)
 */
void Histogram::setWidth(const quint16 w)
{
	if (w == m_width)
		return;
	if (w > m_width)
	{
		Spectrum **pNew = (Spectrum **)realloc(m_data, w * sizeof(Spectrum *));
		if (pNew == NULL)
			return;
		m_data = pNew;
		for (int i = m_width; i < w; ++i)
			m_data[i] = new Spectrum(m_height);
	}
	else
	{
		for (int i = m_width - 1; i >= w; --i)
			delete m_data[i];
		if (w > 0)
		{
			Spectrum **pNew = (Spectrum **)realloc(m_data, w * sizeof(Spectrum *));
			if (pNew == NULL)
				return;
			m_data = pNew;
		}
		else
		{
			free(m_data);
			m_data = NULL;
		}
	}
	m_xSumSpectrum.setWidth(w);
	m_maximumPos = w - 1;
	m_width = w;
}

/*!
    \fn void Histogram::setHeight(const quint16 h)

    \param h
 */
void Histogram::setHeight(const quint16 h)
{
	for(int i = 0; i < m_width; ++i)
	{
		Spectrum *s = m_data[i];
		Q_ASSERT_X(s != NULL, "Histogram::setWidth", "one of the spectra is NULL");
		s->setWidth(h);
	}
	m_ySumSpectrum.setWidth(h);
	m_height = h;
}

/*!
    \fn void Histogram::resize(const quint16 w, const quint16 h)

    sets the size of histogram

    \param w width of histogram
    \param h height of histogram
 */
void Histogram::resize(const quint16 w, const quint16 h)
{
	setWidth(w);
	setHeight(h);
}

void Histogram::setAutoResize(const bool resize)
{
	m_autoResize = resize;
	for(int i = 0; i < m_width; ++i)
	{
		Spectrum *s = m_data[i];
		Q_ASSERT_X(s != NULL, "Histogram::setAutoResize", "one of the spectra is NULL");
		s->setAutoResize(resize);
	}
}

/*!
    \fn QString Histogram::format(void)

    The histogram will be formatted

    \return formatted histogram as string
 */
QString Histogram::format(void)
{
	QString t("");

        for (int i = 0; i < width(); ++i)
                t += QString("\t%1").arg(i);
        t += "\r\n";
        for (int i = 0; i < height(); i++)
        {
                t += QString("%1").arg(i);
                for (int j = 0; j < width(); j++)
		{
                        t += QString("\t%1").arg(value(j, i));
		}
                t += "\r\n";
        }
        t += "\r\n";
        return t;
}

/**
   \fn void Histogram::calcMinMaxInROI(const QRectF &r)

   calculates the min and max in the ROI

   \param r required ROI
 */
void Histogram::calcMinMaxInROI(const QRectF &r)
{
	m_minROI = max();
	m_maxROI = 0;
// no idea why, but Qwt seems to define height as width and vice versa
	int right = ceil(r.left() + r.width());
	int top = ceil(r.top() + r.height());
	for (int j = floor(r.top()); j <= top; ++j)
		for (int i = floor(r.left()); i <= right; ++i)
		{
			quint64 tmp = value(i, j);
			m_minROI = std::min(m_minROI, tmp);
			m_maxROI = std::max(m_maxROI, tmp);
		}
	if (m_minROI == max())
	{
		m_maxROI = m_minROI = value(round(r.left()), round(r.top()));
	}
}

quint64 Histogram::minROI(void) const
{
	return m_minROI;
}

quint64 Histogram::maxROI(void) const
{
	return m_maxROI;
}

/**
    \fn void Histogram::addSlice(const quint16 n, const quint16 d, const Histogram &h)

   Adds a histogram as a slice to an existing histogram in the following manner:
   Beginning with the number n the histogram h will will be added column by column
   to the histogram. The first column of h will be the column n in the histogram and
   the second column of h will be the (n + d)th column of the histogram, and so on.

   \param n starting column in the histogram
   \param d distance between the columns
   \param h histogram
 */
void Histogram::addSlice(const quint16 n, const quint16 d, const Histogram &h)
{
	quint16 col(n);
	for (int i = 0; i < h.width(); ++i, col += d)
		for (int j = 0; j < h.height(); ++j)
			setValue(col, j, h.value(i, j));
}

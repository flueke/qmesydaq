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

#include "histogram.h"
#include "logging.h"
#ifdef _MSC_VER
#	include "stdafx.h"
#endif
#include <cmath>
#include <algorithm>

/**
    \fn Spectrum::Spectrum(const quint16 bins)

    constructor

    \param bins number of points in the spectrum
 */
Spectrum::Spectrum(const quint16 bins)
	: QObject()
	, m_maximumPos(0)
	, m_meanCount(0)
	, m_totalCounts(0)
	, m_meanPos(0)
	, m_autoResize(false)
	, m_width(0)
{
	setWidth(bins);
	m_floatingMean.resize(256);
	clear();
}

//! destructor
Spectrum::~Spectrum()
{
}

/*!
    \fn Spectrum::incVal(const quint16 bin)

    add a event add position bin

    \param bin position inside the spectrum to increment
    \return true or false if successful or not
 */
bool Spectrum::incVal(const quint16 bin)
{
	int size = m_data.size();
	if (bin >= size && autoResize())
		resize(size = bin + 1);
	if (bin < size)
	{
		m_data[bin]++;
		m_totalCounts++;
		calcMaximumPosition(bin);
		calcFloatingMean(bin);
		return true;
	}
	return false;
}

void Spectrum::calcMaximumPosition(const quint16 bin)
{
	if (m_data[bin] > m_data[m_maximumPos])
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
	m_floatingMean[m_meanPos] = bin;
	m_meanPos++;
	if(m_meanCount < 255)
		m_meanCount++;
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
	int size = m_data.size();
	if (bin >= size && autoResize())
		resize(bin + 1);
	if (bin < m_data.size())
	{
		m_totalCounts -= m_data[bin];
		m_data[bin] = val;
		m_totalCounts += val;
		calcMaximumPosition(bin);
		calcFloatingMean(bin);
		return true;
	}
	return false;
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
	int size = m_data.size();
	if (bin >= size && autoResize())
		resize(bin + 1);
	if (bin < size)
	{
		m_data[bin] += val;
		m_totalCounts += val;
		calcMaximumPosition(bin);
		calcFloatingMean(bin);
		return true;
	}
//	MSG_DEBUG << "bin(" << bin << ") > size(" << m_data.size() << ')';
	return false;
}

/*!
    \fn Spectrum::clear(void)

    clear the spectrum
 */
void Spectrum::clear(void)
{
	m_data.fill(0);
	m_floatingMean.fill(0);
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
	if (w == width())
		return;
	else if (w > width())
	{
		m_data.resize(w);
	}
	else
	{
		m_data.resize(w);
		m_data.squeeze();
	}
	m_width = w;
}

quint64 Spectrum::value(const quint16 index)
{
	if (index < m_data.size())
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
	, m_maximumPos(0)
{
	m_data.clear();
	resize(w, h);
	clear();
}

//! destructor
Histogram::~Histogram()
{
}

/*!
    \fn Histogram::max(void) const  

    \return the maximum of the whole histogram
*/
quint64 Histogram::max(void) const
{
	return m_data.size() ? m_data[m_maximumPos]->max() : 0;
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
	if (chan < m_dataKeys.size())
	{
		quint16 i = m_dataKeys[chan];
		return m_data[i]->value(bin);
	}
	return 0;
}

bool Histogram::checkChannel(const quint16 chan)
{
	if (chan < height())
		return true;
	if (autoResize())
		setHeight(chan + 1);
	return true;
}

bool Histogram::checkBin(const quint16 bin)
{
	if (bin < width())
		return true;
	if (autoResize())
		setWidth(bin + 1);
	return true;
}

void Histogram::calcMaximumPosition(const quint16 chan)
{
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
	m_data[chan]->incVal(bin);
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.incVal(bin);
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
	m_totalCounts += val;
	m_data[chan]->setValue(bin, val);
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.addValue(bin, val);

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
	
	if (chan < m_dataKeys.size())
	{
		quint16 i = m_dataKeys[chan];
		m_data[i]->addValue(bin, val);
	}
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.addValue(bin, val);

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
	m_data[channel]->clear();

#if defined(_MSC_VER)
#	pragma message("TODO remove the counts from the sum spectrum and total counts and adjust the new maximum")
#else
#	warning TODO remove the counts from the sum spectrum and total counts and adjust the new maximum
#endif
//! \todo remove the counts from the sum spectrum and total counts and adjust the new maximum
}


/*!
    \fn Histogram::clear(void);

    clears the complete histogram
 */
void Histogram::clear(void)
{
	foreach (Spectrum *value, m_data)
		value->clear();
//	m_data.clear();
	m_sumSpectrum.clear();
	m_totalCounts = 0;
	m_dataKeys = m_data.keys();
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
	int h = region.y() + region.width();
	int w = region.x() + region.height();

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
   	return m_data.contains(channel) ?  m_data[channel] : NULL;
}

/*!
    \fn Histogram::max(const quint16 channel) const

    \param channel number of the tube
    \return the maximum of the spectrum of the tube channel
 */
quint64 Histogram::max(const quint16 channel) const
{
   	return m_data.contains(channel) ? m_data[channel]->max() : 0;
}

/*!
    \fn Histogram::maxpos(const quint16 channel) const

    \param channel number of the tube
    \return the number of the bin in the tube channel
 */
quint16 Histogram::maxpos(const quint16 channel) const
{
   	return m_data.contains(channel) ? m_data[channel]->maxpos() : 0;
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
   	if (m_data.contains(chan))
	{
		m = m_data[chan]->mean(s);
	}
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
	m = m_sumSpectrum.mean(s);
}

/*!
    \fn	quint16 Histogram::height(void) const

    \return number of tubes
*/
quint16	Histogram::height(void) const 
{
	return m_dataKeys.size();
}

/*!
    \fn void Histogram::setHeight(const quint16 h)

    \param h
 */
void Histogram::setHeight(const quint16 h)
{
	if (h == height())
		return;
	else if (h > height())
	{
		int w = width();
		for (int i = height(); i < h; ++i)
			m_data.insert(i, new Spectrum(w));
	}
	else 
	{
		for (int i = height(); i >= h; --i)
			m_data.remove(i);
	}
	m_dataKeys = m_data.keys();
	qSort(m_dataKeys);
	m_maximumPos = m_data.size() ? m_dataKeys.last() : 0;
}

/*!
    \fn void Histogram::setWidth(const quint16 w)

    sets the width of each cell

    \param w new witdh of histogram (or number of tubes)
 */
void Histogram::setWidth(const quint16 w)
{
	foreach(Spectrum *s, m_data)
		s->setWidth(w);
	m_sumSpectrum.setWidth(w);
	m_width = w;
}

/*!
    \fn void Histogram::resize(const quint16 w, const quint16 h)

    sets the size of histogram

    \param w width of histogram
    \param h height of histogram
 */
void Histogram::resize(const quint16 w, const quint16 h) 
{
	setHeight(h);
	setWidth(w); 
}

void Histogram::setAutoResize(const bool resize) 
{
	m_autoResize = resize;
	foreach(Spectrum *s, m_data)
		s->setAutoResize(resize);
}

/*!
    \fn QString Histogram::format(void)

    The histogram will be formatted

    \return formatted histogram as string
 */
QString Histogram::format(void)
{
	QString t("");

        for (int i = 0; i < height(); ++i)
                t += QString("\t%1").arg(i);
        t += "\r\n";
        for (int i = 0; i < width(); i++)
        {
                t += QString("%1").arg(i);
                for (int j = 0; j < height(); j++)
		{
                        t += QString("\t%1").arg(value(j, i));
		}
                t += "\r\n";
        }
        t += "\r\n";
        return t;
}

void Histogram::calcMinMaxInROI(const QRectF &r)
{
	m_minROI = max();
	m_maxROI = 0;
// no idea why, but Qwt seems to define height as width and vice versa
	int right = r.left() + r.height(); 
	int top = r.top() + r.width();
	for (int i = floor(r.left()); i < ceil(right); ++i)
		for (int j = floor(r.top()); j < ceil(top); ++j)
		{
			quint64 tmp = value(i, j);
			m_minROI = std::min(m_minROI, tmp);
			m_maxROI = std::max(m_maxROI, tmp);
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


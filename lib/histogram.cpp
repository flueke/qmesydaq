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
#include <QTextStream>
#include <QFile>

#include <QDebug>

#include "histogram.h"

#include <cmath>

/**
    \fn Spectrum::Spectrum(quint16 bins)

    constructor

    \param bins number of points in the spectrum
 */
Spectrum::Spectrum(quint16 bins)
	: MesydaqObject()
	, m_data(NULL)
	, m_maximumPos(0)
	, m_meanCount(0)
	, m_totalCounts(0)
	, m_meanPos(0)
{
	m_data.resize(bins);
	m_floatingMean.resize(256);
	clear();
}

//! destructor
Spectrum::~Spectrum()
{
}

/*!
    \fn Spectrum::incVal(quint16 bin)

    add a event add position bin

    \param bin position inside the spectrum to increment
    \return true or false if successful or not
 */
bool Spectrum::incVal(quint16 bin)
{
	if (bin < m_data.size())
	{
		m_data[bin]++;
		m_totalCounts++;
		calcMaximumPosition(bin);
		calcFloatingMean(bin);
		return true;
	}
	return false;
}

void Spectrum::calcMaximumPosition(quint16 bin)
{
	if (m_data[bin] > m_data[m_maximumPos])
		m_maximumPos = bin;
}

void Spectrum::calcFloatingMean(quint16 bin)
{
#warning TODO mean value determination
//! \todo mean value determination
	m_floatingMean[m_meanPos] = bin;
	m_meanPos++;
	if(m_meanCount < 255)
		m_meanCount++;
}

/*!
    \fn Spectrum::setValue(quint16 bin, quint64 val)

    sets the events at position bin

    \param bin position inside the spectrum to set
    \param val events 
    \return true or false if successful or not
 */
bool Spectrum::setValue(quint16 bin, quint64 val)
{
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
bool Spectrum::addValue(quint16 bin, quint64 val)
{
	if (bin < m_data.size())
	{
		m_data[bin] += val;
		m_totalCounts += val;
		calcMaximumPosition(bin);
		calcFloatingMean(bin);
		return true;
	}
//	qDebug("bin(%d) > size(%d)", bin, m_data.size());
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
    \fn Histogram::Histogram(quint16 channels, quint16 bins)
    
    constructor

    \param channels number of channels (i.e. number of tubes)
    \param bins number of bins (inside a tube)
 */
Histogram::Histogram(quint16 , quint16 bins)
	: MesydaqObject()
	, m_totalCounts(0)
	, m_twidth(1)
	, m_maximumPos(0)
{
	m_data.clear();
	m_sumSpectrum.resize(bins);
	clear();
}

//! destructor
Histogram::~Histogram()
{
}

/*!
    \fn Histogram::max() 

    \return the maximum of the whole histogram
*/
quint64 Histogram::max() 
{
	return m_data.size() ? m_data[m_maximumPos]->max() : 0;
}

/*!
    \fn Histogram::value(quint16 chan, quint16 bin)

    gives the counts of the cell x,y. 

    The implementation is a little bit tricky. In the m_dataKeys list
    are stored all keys for the single tube spectra. The will mapped
    to a number of the tube. This saves some space in the memory.

    \param chan number of the bin
    \param bin number of the tube
    \return 0 rief the cell does not exist, otherwise the counts
 */
quint64 Histogram::value(quint16 chan, quint16 bin)
{
	if (chan < m_dataKeys.size())
	{
		quint16 i = m_dataKeys[chan];
		return m_data[i]->value(bin);
	}
	return 0;
}

void Histogram::checkChannel(quint16 chan)
{
	if (!m_data.contains(chan))
	{
		for (quint16 i = 8 * (chan / 8); i < 8 * (1 + chan / 8); ++i)
			if (!m_data.contains(i))
				m_data.insert(i, new Spectrum(m_sumSpectrum.width()));
		m_dataKeys = m_data.keys();
		qSort(m_dataKeys);
	}
	if (!m_data.contains(m_maximumPos))
		m_maximumPos = chan;
}

void Histogram::calcMaximumPosition(quint16 chan)
{
	if (m_data[chan]->max() > m_data[m_maximumPos]->max())
		m_maximumPos = chan;
}

/**
    \fn Histogram::incVal(quint16 chan, quint16 bin)

    increment value by 1 in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \return true if it was ok otherwise false
 */
bool Histogram::incVal(quint16 chan, quint16 bin)
{
	checkChannel(chan);
// total counts of histogram (like monitor ??)
	m_totalCounts++;
	m_data[chan]->incVal(bin);
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.incVal(bin);

	return true;
}

/**
    \fn Histogram::setValue(quint16 chan, quint16 bin, quint64 val)

    set the event value in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::setValue(quint16 chan, quint16 bin, quint64 val)
{
	checkChannel(chan);
// total counts of histogram (like monitor ??)
	m_totalCounts += val;
	m_data[chan]->setValue(bin, val);
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.addValue(bin, val);

	return true;
}

/**
    \fn Histogram::addValue(quint16 chan, quint16 bin, quint64 val)

    set the event value in cell[chan, bin]. If the cell does
    not exist it will be created.

    \param chan number of the spectrum
    \param bin number of the bin in the spectrum
    \param val events
    \return true if it was ok otherwise false
 */
bool Histogram::addValue(quint16 chan, quint16 bin, quint64 val)
{
	checkChannel(chan);
// total counts of histogram (like monitor ??)
	m_totalCounts += val;
	m_data[chan]->addValue(bin, val);
	calcMaximumPosition(chan);
// sum spectrum of all channels
	m_sumSpectrum.addValue(bin, val);

	return true;
}

/*!
    \fn Histogram::clear(quint16 channel)

    clears the spectrum channel

    \param channel number of the spectrum to be cleared
 */
void Histogram::clear(quint16 channel)
{
	m_totalCounts -= m_data[channel]->getTotalCounts();
	m_data[channel]->clear();

#warning TODO remove the counts from the sum spectrum and total counts and adjust the new maximum
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
	m_data.clear();
	m_sumSpectrum.clear();
	m_totalCounts = 0;
	m_twidth = 1;
	m_dataKeys = m_data.keys();
}

/*!
    \fn Histogram::getTotalCounts(void)

    \return the sum of all counts
 */
quint64 Histogram::getTotalCounts(void)
{
	return m_totalCounts;
}

/*!
    \fn Histogram::spectrum(quint16 channel)

    \param channel number of the tube
    \return the spectrum of the tube channel
 */
Spectrum *Histogram::spectrum(quint16 channel)
{
   	return m_data.contains(channel) ?  m_data[channel] : NULL;
}

/*!
    \fn Histogram::max(quint16 channel)

    \param channel number of the tube
    \return the maximum of the spectrum of the tube channel
 */
quint64 Histogram::max(quint16 channel)
{
   	return m_data.contains(channel) ? m_data[channel]->max() : 0;
}

/*!
    \fn Histogram::maxpos(quint16 channel)

    \param channel number of the tube
    \return the number of the bin in the tube channel
 */
quint16 Histogram::maxpos(quint16 channel)
{
   	return m_data.contains(channel) ? m_data[channel]->maxpos() : 0;
}

/*!
    \fn Histogram::getMean(quint16 chan, float &m, float &s)

    gives the mean value and the standard deviation of the last events in the tube chan
	
    \param chan the number of the tube
    \param m mean value
    \param s standard deviation
 */
void Histogram::getMean(quint16 chan, float &m, float &s)
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
    \fn	Histogram::height() 

    \return number of tubes
*/
quint16	Histogram::height() 
{
	return m_data.size();
}

/*!
    \fn Histogram::setWidth(quint8 width)

    sets the width of each cell

    \param width 
 */
void Histogram::setWidth(quint8 width)
{
	m_twidth = width; 
}

/*!
    \fn Histogram::writeHistogram(QFile *f, const QString title)

    writes the histogram to the opened file with a comment.

    \param f file pointer to the opened file
    \param title title for the histogram
    \return true in case of success else false
 */
bool Histogram::writeHistogram(QFile *f, const QString title)
{
	QTextStream t( f );        // use a text stream
	QString s;
	// Title
	t << title << '\r' << '\n'; 
	int width = m_data.size();
	for(int i = 0; i < width; i++)		// why 64 ??? 
		t << '\t' << i; 
	t << '\r' << '\n';
	t.flush();
	int size = m_sumSpectrum.width();
	for(int i = 0; i < size; ++i)
	{
		t << i;
		for(int j = 0; j < width; j++)	
			t << '\t' << value(j, i);
		t << '\r' << '\n';
		t.flush();
	}
	t << '\r' << '\n';
	t.flush();

// "position data: 1 row title (8 x 8 detectors), position data in columns";
// "amplitude/energy data: 1 row title (8 x 8 detectors), amplitude data in columns";
	return true;
}


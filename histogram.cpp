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
#include <QTextStream>
#include <QFile>

#include "histogram.h"

#include <cmath>

Spectrum::Spectrum(quint16 bins)
	: m_data(NULL)
	, m_bins(bins)
	, m_maximumPos(0)
	, m_meanCount(0)
	, m_meanPos(0)
{
	m_data = new quint64[bins];
	clear();
}

Spectrum::~Spectrum()
{
	delete m_data;
}

/**
 \param bin number of the bin in the spectrum
 \return true or false if successful or not
 */
bool Spectrum::incVal(quint16 bin)
{
	m_data[bin]++;
	if (m_data[bin] > m_data[m_maximumPos])
		m_maximumPos = bin;

#warning TODO mean value determination
#if 0
	m_floatingMean[m_meanPos] = bin;
	m_meanPos++;
#endif
	if(m_meanCount < 255)
		m_meanCount++;
	return true;
}

void Spectrum::clear(void)
{
	for (quint16 i = 0; i < m_bins; ++i)
		m_data[i] = 0;
	m_maximumPos = m_meanCount = m_meanPos = 0;
}

/**
 \param s sigma of the floating mean value
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

Histogram::Histogram(quint16 channels, quint16 bins, QObject *parent)
	: MesydaqObject(parent)
	, m_lastTime(0)
	, m_totalCounts(0)
	, m_twidth(1)
	, m_data(NULL)
	, m_channels(channels)
	, m_sumSpectrum(NULL)
	, m_timeSpectrum(NULL)
	, m_maximumPos(0)
{
	m_data = new Spectrum*[m_channels];
	for (quint16 i = 0; i < m_channels; ++i)
		m_data[i] = new Spectrum(bins);
	
	m_sumSpectrum = new Spectrum(bins);
	m_timeSpectrum = new Spectrum(bins);
	clear();
}

Histogram::~Histogram()
{
	for (quint16 i = 0; i < m_channels; ++i)
		delete m_data[i];
	delete m_data;
	delete m_sumSpectrum;
	delete m_timeSpectrum;
}

bool Histogram::incVal(quint16 chan, quint16 bin, quint64 time)
{
// total counts of histogram (like monitor ??)
	m_totalCounts++;
	if (chan < m_channels)
		m_data[chan]->incVal(bin);
	else
		qDebug("ERROR !!!!! chan = %d", chan); 
// sum spectrum of all channels
	m_sumSpectrum->incVal(bin);

// already a value?
	if(m_lastTime)
	{
		quint64 deltat = time - m_lastTime;
#warning TODO 960 ??????
		if(deltat > 959)		
			deltat = 959; 
#warning TODO	m_timeSpectrum->incVal((quint16)deltat);
	}
	m_lastTime = time;
		
	return true;
}

/*!
    \fn Histogram::clearHist(unsigned int channel)
 */
void Histogram::clear(quint16 channel)
{
	m_totalCounts -= m_data[channel]->getTotalCounts();
	m_data[channel]->clear();

#warning TODO
// remove the counts from the sum spectrum and total counts
// adjust the new maximum
}


/*!
    \fn Histogram::clearAllHist(void);
 */
void Histogram::clear(void)
{
 	for(quint16 i = 0;i < m_channels; i++)
		m_data[i]->clear();
	m_sumSpectrum->clear();
	m_timeSpectrum->clear();
	m_lastTime = 0;
	m_totalCounts = 0;
	m_twidth = 1;
}

/*!
    \fn Histogram::getTotalCounts(void)
 */
quint64 Histogram::getTotalCounts(void)
{
	return m_totalCounts;
}

/*!
    \fn Histogram::copyLine(channel)
 */
void Histogram::copyLine(quint16 channel, ulong *pLineBuffer)
{
   	if(channel <= m_channels)
		for(quint16 i = 0; i < 960; i++)
    			pLineBuffer[i] = m_data[channel]->val(i);
	else
		for(quint16 i = 0; i < 960; i++)
                        pLineBuffer[i] = 0;
}


/*!
    \fn Histogram::max(unsigned int channel)
 */
quint64 Histogram::max(quint16 channel)
{
	if(channel <= m_channels)
		return m_data[channel]->max();
	else
		return 0;
}


/*!
    \fn Histogram::maxpos(unsigned char channel)
 */
quint16 Histogram::maxpos(quint16 channel)
{
	return m_data[channel]->maxpos();
//	return m_maximumPos;
}

/*!
 */
void Histogram::getMean(quint16 chan, float &m, float &s)
{
	m = m_data[chan]->mean(s);
}

/*!
    \fn Histogram::getMean(float* vals)
 */
void Histogram::getMean(quint16 chan, float* vals)
{
	float s = 0;
	
// calculate mean for given channel:
	float m = m_data[chan]->mean(s);
	vals[0] = m;
	vals[1] = s;
//	qDebug("chan: %d, mean: %f +/- %f", chan, m, s);	
	return;
}


/*!
    \fn Histogram::setWidth(unsigned char width)
 */
void Histogram::setWidth(quint8 width)
{
	m_twidth = width; 
}


/*!
    \fn Histogram::writeHistogram(QFile f)
 */
bool Histogram::writeHistogram(QFile* f)
{
	quint32 i, j, k;
  
	QTextStream t( f );        // use a text stream
	QString s;
	// Title
	t << "position data: 1 row title (8 x 8 detectors), position data in columns";
	t << '\r' << '\n';
	t << '\t';
	for(i = 0; i < 64; i++)		// why 64 ??? 
	{
		t << i << '\t';
	}
	t << '\r' << '\n';
	for(i = 0; i < 960; i++)	// ???? why 960
	{
		t << i << '\t';
		for(k = 0; k < 8 ; k++)
		{
			for(j = 0; j < 8 ; j++)
			{
#warning TODO
				t << m_data[k]->val(i) << '\t';
			}
		}
		t << '\r' << '\n';
	}
	t << '\r' << '\n';

	t << "amplitude/energy data: 1 row title (8 x 8 detectors), amplitude data in columns";
	t << '\r' << '\n';
	t << '\t';
	for(i = 0; i < 64; i++)
	{
		t << i << '\t';
	}
	t << '\r' << '\n';
	for(i = 0; i < 960; i++)
	{
		t << i << '\t';
		for(k = 0; k < 8 ; k++)
		{
			for(j = 0; j < 8 ; j++)
			{
#warning TODO
				t << m_data[k]->val(i) << '\t';
			}
		}
		t << '\r' << '\n';
	}
	t << '\r' << '\n';
	return true;
}


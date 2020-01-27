/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2020 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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
#include "mappedhistogram.h"
#include "mapcorrect.h"
#include "qmlogging.h"

/*!
    constructor: store mapping and possible create histogram (generate a new mapped copy of the source)

    \param pMapCorrection pointer to to new mapping data (this class stores the reference only)
    \param pHistogram     pointer to existing source histogram or NULL
 */
MappedHistogram::MappedHistogram(MapCorrection *pMapCorrection, Histogram *pHistogram)
	: Histogram(0, 0)
	, m_pMapCorrection(NULL)
	, m_dblTotalCounts(0.0)
	, m_iMaxPos(-1)
	, m_spectrum(NULL)
{
	setMapCorrection(pMapCorrection, pHistogram);
	m_spectrum = new Spectrum(0);
}

/*!
    copy constructor

    \param src
 */
MappedHistogram::MappedHistogram(const MappedHistogram &src)
	: Histogram(src)
	, m_pMapCorrection(src.m_pMapCorrection)
	, m_adblData(src.m_adblData)
	, m_dblTotalCounts(src.m_dblTotalCounts)
	, m_iMaxPos(src.m_iMaxPos)
{
}

MappedHistogram::~MappedHistogram()
{
#if 0
	if (m_spectrum)
		delete m_spectrum;
	m_spectrum = NULL;
#endif
}

/*!
    copy operator

    \param src
    \return copy of the other object
 */
MappedHistogram& MappedHistogram::operator=(const MappedHistogram &src)
{
	Histogram::operator =(src);
	m_pMapCorrection = src.m_pMapCorrection;
	m_adblData = src.m_adblData;
	m_dblTotalCounts = src.m_dblTotalCounts;
	m_iMaxPos = src.m_iMaxPos;
	m_spectrum = new Spectrum(*src.m_spectrum);
	return *this;
}

// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]

/*!
    set new histogram and possible new mapping (generate a new mapped copy of the source)

    \param pMapCorrection pointer to to new mapping data (this class stores the reference only)
    \param pSrc           pointer to existing source histogram or NULL
 */
void MappedHistogram::setMapCorrection(MapCorrection *pMapCorrection, Histogram *pSrc /*= NULL*/)
{
	if (pMapCorrection != NULL && pMapCorrection != m_pMapCorrection)
	{
		const QRect &mapRect = pMapCorrection->getMapRect();
		m_pMapCorrection = pMapCorrection;
		resize(mapRect.width(), mapRect.height());
	}
	else if (m_pMapCorrection == NULL && pSrc != NULL)
		resize(pSrc->width(), pSrc->height());
	m_adblData.resize(m_width * m_height);
	if (pSrc == NULL)
		return;
	clear();
	register int iHeight = pSrc->height();
	register int iWidth = pSrc->width();
	for (int ys = 0; ys < iHeight; ++ys)
		for (int xs = 0; xs < iWidth; ++xs)
			addValue(xs, ys, pSrc->value(xs, ys));
}

quint64 MappedHistogram::getCorrectedTotalCounts(void)
{
	quint64 r = (quint64)(m_dblTotalCounts + 0.5);
	if (!r && m_totalCounts > 0) // single event only
		++r;
	return r;
}

/*!
    return the mapped value as 64 bit integer at a specific position

    \param x
    \param y

    \return the counts at a specific position
 */
quint64 MappedHistogram::value(quint16 x, quint16 y) const
{
	double r = this->floatValue(x, y);
	if (r > 0.0 && r < 1.0) 	// single event only
		r = 1.0;
	return (quint64)(r + 0.5);
}

/*!
    return the mapped value as double value at a specific position

    \param x
    \param y

    \return the counts at a specific position
 */
double MappedHistogram::floatValue(quint16 x, quint16 y) const
{
	register int iPos = y * m_width + x;
	if (x < m_width && y < m_height && iPos >= 0 && iPos < m_adblData.count())
		return m_adblData[iPos];
	return 0.0;
}

// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
bool MappedHistogram::incVal(quint16 channel, quint16 bin)
{
	bool bOK(false);
	if (m_pMapCorrection != NULL)
	{
		int iDstX(-1);
		int iDstY(-1);
		float fCorrection(0.0);
		if (m_pMapCorrection->getMap(channel, bin, iDstX, iDstY, fCorrection))
		{
			if (fCorrection > 0.0)
			{
				int iPos(m_width * iDstY + iDstX);
				if (iDstX >= 0 && iDstX < m_width &&  iDstY >= 0 && iDstY < m_height && iPos >= 0 && iPos < m_adblData.count())
				{
					++m_totalCounts;
					m_dblTotalCounts += 1.0;

					m_adblData[iPos] += fCorrection;
					if (m_iMaxPos < 0)
						m_iMaxPos = iPos;
					else if (m_adblData[iPos] > m_adblData[m_iMaxPos])
						m_iMaxPos = iPos;
					switch (m_pMapCorrection->orientation())
					{
#if defined(_MSC_VER)
#	pragma message("TODO the change of the iPos value for the unhandled cases")
#else
#	warning TODO the change of the iPos value for the unhandled cases
#endif

						case MapCorrection::OrientationLeft:
						case MapCorrection::OrientationLeftRev:
						case MapCorrection::OrientationRight:
						case MapCorrection::OrientationRightRev:
						case MapCorrection::OrientationUpRev:
						case MapCorrection::OrientationDown:
							break;
						default:
							++iDstY;
							break;
						case MapCorrection::OrientationDownRev:
							--iDstY;
							break;
					}

					iPos = m_width * iDstY + iDstX;
					if (iPos >= 0 && iDstY < m_height && iPos < m_adblData.count())
					{
						m_adblData[iPos] += 1 - fCorrection;
						if (m_adblData[iPos] > m_adblData[m_iMaxPos])
							m_iMaxPos = iPos;
					}
				}
				bOK = true;
			}
		}
	}
	return bOK;
}

bool MappedHistogram::addValue(const quint16 chan, const quint16 bin, const quint64 val)
{
	bool bOK(false);
	if (m_pMapCorrection != NULL)
	{
		int iDstX(-1);
		int iDstY(-1);
		float fCorrection(0.0);
		if (m_pMapCorrection->getMap(chan, bin, iDstX, iDstY, fCorrection))
		{
			if (fCorrection > 0.0)
			{
				int iPos(m_width * iDstY + iDstX);
				if (iDstX >= 0 && iDstX < m_width && iDstY >= 0 && iDstY < m_height && iPos >= 0 && iPos < m_adblData.count())
				{
					m_totalCounts += val;
					m_dblTotalCounts += val;
					m_adblData[iPos] += val * fCorrection;
					if (m_iMaxPos < 0)
						m_iMaxPos = iPos;
					else if (m_adblData[iPos] > m_adblData[m_iMaxPos])
						m_iMaxPos = iPos;

					switch (m_pMapCorrection->orientation())
					{
#if defined(_MSC_VER)
#	pragma message("TODO the change of the iPos value for the unhandled cases")
#else
#	warning TODO the change of the iPos value for the unhandled cases
#endif
						case MapCorrection::OrientationLeft:
						case MapCorrection::OrientationLeftRev:
						case MapCorrection::OrientationRight:
						case MapCorrection::OrientationRightRev:
						case MapCorrection::OrientationUpRev:
						case MapCorrection::OrientationDown:
							break;
						default:
							++iDstY;
							break;
						case MapCorrection::OrientationDownRev:
							--iDstY;
							break;
					}

					iPos = m_width * iDstY + iDstX;
					if (iPos >= 0 && iDstY < m_height && iPos < m_adblData.count())
					{
						m_adblData[iPos] += val * (1 - fCorrection);
						if (m_adblData[iPos] > m_adblData[m_iMaxPos])
							m_iMaxPos = iPos;
						bOK = true;
					}
				}
			}
		}
	}
	return bOK;
}

void MappedHistogram::clear(void)
{
	Histogram::clear();
	for (int i = 0; i<m_adblData.count(); ++i)
		m_adblData[i] = 0.0;
	m_dblTotalCounts = 0.0;
	m_iMaxPos=-1;
}

void MappedHistogram::setHistogram(Histogram *pSrc)
{
	setMapCorrection(NULL, pSrc);
}

quint64 MappedHistogram::max() const
{
	return (m_iMaxPos >= 0 && m_iMaxPos < m_adblData.count()) ? quint64(m_adblData[m_iMaxPos]) : 0;
}

quint16 MappedHistogram::maxpos() const
{
	return m_iMaxPos;
}

MapCorrection *MappedHistogram::getMapCorrection() const
{
	return m_pMapCorrection;
}

/*!
    \fn MappedHistogram::spectrum(const quint16 channel)

    \param channel number of the tube
    \return the spectrum of the tube channel
 */
Spectrum *MappedHistogram::spectrum(const quint16 channel)
{
	if (m_spectrum)
	{
		m_spectrum->clear();
		if (m_spectrum->width() != m_height)
			m_spectrum->resize(m_height);
		for (int i = 0; i < m_height; ++i)
			m_spectrum->setValue(i, floatValue(channel, i));
	}
	return m_spectrum;
}

#if 0
// OrientationUp:       channel --> X [left=0 ... right], bin --> Y [bottom=0 ... top]
// OrientationUpRev:    channel --> X [left=0 ... right], bin --> Y [top=0 ... bottom]
// OrientationDown:     channel --> X [right=0 ... left], bin --> Y [top=0 ... bottom]
// OrientationDownRev:  channel --> X [right=0 ... left], bin --> Y [bottom=0 ... top]
// OrientationLeft:     channel --> Y [bottom=0 ... top], bin --> X [left=0 ... right]
// OrientationLeftRev:  channel --> Y [bottom=0 ... top], bin --> X [right=0 ... left]
// OrientationRight:    channel --> Y [top=0 ... bottom], bin --> X [right=0 ... left]
// OrientationRightRev: channel --> Y [top=0 ... bottom], bin --> X [left=0 ... right]
void MappedHistogram::maporientation(int iSrcX, int iSrcY, int& iDstX, int& iDstY)
{
}

QPoint MappedHistogram::maporientation(const QPoint& src)
{
	QPoint dst;
	if (!m_pMapCorrection)
		return src;
	switch (m_pMapCorrection->m_iOrientation) // X
	{
		default: //MapCorrection::OrientationUp:
			return src;
		case MapCorrection::OrientationUpRev:
			dst.setX(src.x());
			break;
		case MapCorrection::OrientationDown:
		case MapCorrection::OrientationDownRev:
			dst.setX(m_iWidth - src.x()-1);
			break;
		case MapCorrection::OrientationLeft:
		case MapCorrection::OrientationRightRev:
			dst.setX(src.y());
			break;
		case MapCorrection::OrientationLeftRev:
		case MapCorrection::OrientationRight:
			dst.setX(m_iHeight - src.y() - 1);
			break;
	}
	switch (m_pMapCorrection->m_iOrientation) // Y
	{
		default:
			return src; // never reached
		case MapCorrection::OrientationUpRev:
		case MapCorrection::OrientationDown:
		case MapCorrection::OrientationDownRev:
		case MapCorrection::OrientationLeft:
		case MapCorrection::OrientationLeftRev:
		case MapCorrection::OrientationRight:
		case MapCorrection::OrientationRightRev:
	}
}
#endif

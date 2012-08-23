/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Krüger <jens.krueger@frm2.tum.de>          *
 *   Copyright (C) 2011-2012 by Lutz Rossa <rossa@helmholtz-berlin.de>     *
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

////////////////////////////////////////////////////////////////////////////
// $$HeadURL$$
//
// last change:
// $$Author$$
// $$Date$$
// revision $$Rev$$
////////////////////////////////////////////////////////////////////////////

#include "mappedhistogram.h"

#include "mapcorrect.h"

#include "logging.h"

/*!
    constructor: store mapping and possible create histogram (generate a new mapped copy of the source)

    \param pMapCorrection pointer to to new mapping data (this class stores the reference only)
    \param pHistogram     pointer to existing source histogram or NULL
 */
MappedHistogram::MappedHistogram(MapCorrection *pMapCorrection, Histogram *pHistogram)
	: Histogram(0, 0)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_pMapCorrection(NULL)
	, m_ullTotalCounts(0ULL)
	, m_dblTotalCounts(0.0)
	, m_iMaxPos(-1)
{
	setMapCorrection(pMapCorrection, pHistogram);
}

/*!
    copy constructor

    \param src
 */
MappedHistogram::MappedHistogram(const MappedHistogram &src)
	: Histogram(src.m_iHeight, src.m_iWidth)
	, m_iWidth(src.m_iWidth)
	, m_iHeight(src.m_iHeight)
	, m_pMapCorrection(src.m_pMapCorrection)
	, m_adblData(src.m_adblData)
	, m_ullTotalCounts(src.m_ullTotalCounts)
	, m_dblTotalCounts(src.m_dblTotalCounts)
	, m_iMaxPos(src.m_iMaxPos)
{
}

/*!
    copy operator

    \param src
    \return copy of the other object
 */
MappedHistogram& MappedHistogram::operator=(const MappedHistogram &src)
{
	m_iWidth = src.m_iWidth;
	m_iHeight = src.m_iHeight;
	m_pMapCorrection = src.m_pMapCorrection;
	m_adblData = src.m_adblData;
	m_ullTotalCounts = src.m_ullTotalCounts;
	m_dblTotalCounts = src.m_dblTotalCounts;
	m_iMaxPos = src.m_iMaxPos;
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
        	m_width = m_iWidth = mapRect.width();
        	m_height = m_iHeight = mapRect.height();
	}
	else if (m_pMapCorrection == NULL && pSrc != NULL) 
	{
        	m_width = m_iWidth = pSrc->width();
        	m_height = m_iHeight = pSrc->height();
	}
	m_adblData.resize(m_iWidth * m_iHeight);
	if (pSrc == NULL)
        	return;
	clear();
	for (int ys = 0; ys < pSrc->height(); ++ys)
        	for (int xs = 0; xs < pSrc->width(); ++xs)
			addValue(xs, ys, pSrc->value(xs, ys));
}

quint64 MappedHistogram::getCorrectedTotalCounts(void)
{
	quint64 r = (quint64)(m_dblTotalCounts + 0.5);
	if (!r && m_ullTotalCounts > 0) // single event only
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
	register int iPos = y * m_iWidth + x;
	if (x < m_iWidth && y < m_iHeight && iPos >= 0 && iPos < m_adblData.count())
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
				int iPos(m_iWidth * iDstY + iDstX);
				if (iDstX >= 0 && iDstX < m_iWidth &&  iDstY >= 0 && iDstY < m_iHeight && iPos >= 0 && iPos < m_adblData.count())
				{
					++m_ullTotalCounts;
					m_dblTotalCounts += 1.0;

					m_adblData[iPos] += fCorrection;
					if (m_iMaxPos < 0) 
						m_iMaxPos = iPos; 
					else if (m_adblData[iPos] > m_adblData[m_iMaxPos])
						m_iMaxPos = iPos;
					switch (m_pMapCorrection->orientation()) 
					{
#warning TODO the change of the iPos value for the unhandled cases
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

					iPos = m_iWidth * iDstY + iDstX;
					if (iPos >= 0 && iDstY < m_iHeight && iPos < m_adblData.count())
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
				int iPos(m_iWidth * iDstY + iDstX);
				if (iDstX >= 0 && iDstX < m_iWidth && iDstY >= 0 && iDstY < m_iHeight && iPos >= 0 && iPos < m_adblData.count())
				{
					m_ullTotalCounts += val;
					m_dblTotalCounts += val;
					m_adblData[iPos] += val * fCorrection;
					if (m_iMaxPos < 0) 
						m_iMaxPos = iPos; 
					else if (m_adblData[iPos] > m_adblData[m_iMaxPos])
						m_iMaxPos = iPos;

					switch (m_pMapCorrection->orientation()) 
					{
#warning TODO the change of the iPos value for the unhandled cases
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

					iPos = m_iWidth * iDstY + iDstX;
					if (iPos >= 0 && iDstY < m_iHeight && iPos < m_adblData.count())
					{
						m_adblData[iPos] += val * (1 - fCorrection);
						if (m_adblData[iPos] > m_adblData[m_iMaxPos])
							m_iMaxPos = iPos;
					}				
				}
			}
		}
	}
	return bOK;
}

void MappedHistogram::clear(void)
{
	for (int i = 0; i<m_adblData.count(); ++i)
		m_adblData[i] = 0.0;
	m_ullTotalCounts = 0;
	m_dblTotalCounts = 0.0;
	m_iMaxPos=-1;
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

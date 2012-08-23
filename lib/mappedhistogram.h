/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009 by Jens Kr�ger <jens.krueger@frm2.tum.de>          *
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

#ifndef _MAPPEDHISTOGRAM_H_
#define _MAPPEDHISTOGRAM_H_

#include "libqmesydaq_global.h"
#include "histogram.h"

class LIBQMESYDAQ_EXPORT MappedHistogram;
class LIBQMESYDAQ_EXPORT MapCorrection;

/**
 * \short represents a histogram of mapped and corrected data
 *
 * \author Lutz Rossa <rossa@helmholtz-berlin.de>
 */
class LIBQMESYDAQ_EXPORT MappedHistogram : public Histogram
{
	Q_OBJECT
public:
	MappedHistogram(MapCorrection *pMapCorrection, Histogram *pHistogram = NULL);
	MappedHistogram(const MappedHistogram& src);
	MappedHistogram& operator=(const MappedHistogram& src);

	//! destructor
	virtual ~MappedHistogram() {}

	/*!
	    set new histogram and possible new mapping (generate a new mapped copy of the source)

	    \param pMapCorrection pointer to to new mapping data (this class stores the reference only)
	    \param pSrc           pointer to existing source histogram or NULL
        */
	void setMapCorrection(MapCorrection *pMapCorrection, Histogram *pSrc = NULL);

	/*!
	    sets the histogram (generate a mapped copy of the source)

	    \param pSrc pointer to existing source histogram
	*/
	void setHistogram(Histogram *pSrc) 
	{ 
		setMapCorrection(NULL, pSrc); 
	}

	//! \return sum of all events as uncorrected value
	virtual quint64 getTotalCounts(void) const { return m_ullTotalCounts; }

	//! \return sum of all events after correction
	quint64 getCorrectedTotalCounts(void);

	//! \return the maximum value of this histogram
	virtual quint64 max() const
	{ 
		return m_iMaxPos >= 0 && m_iMaxPos < m_adblData.count() ? ((quint64)m_adblData[m_iMaxPos]) : 0; 
	}

	//! \return the number of the first tube containing the maximum value
	virtual quint16 maxpos() const { return m_iMaxPos; }

	virtual quint64 value(quint16 x, quint16 y) const;

	//! \return the mapped value as double
	double floatValue(quint16 x, quint16 y) const;

	//! \return the height of this histogram
	virtual quint16 height() const { return m_iHeight; }

	//! \return the width of this histogram
	virtual quint16 width() const { return m_iWidth; }

	//! \param chan hardware input channel
	//! \param bin  position at specified channel
	//! \brief store an event at position
	virtual bool incVal(quint16 chan, quint16 bin);

	virtual bool addValue(const quint16 chan, const quint16 bin, const quint64 val);

	//! \brief clear this histogram
	virtual void clear(void);

private:
	//! width of mapped histogram
	quint16 m_iWidth;

	//! height of mapped histogram
	quint16 m_iHeight;

	//! pointer to mapping information
	MapCorrection* m_pMapCorrection;

	//! mapped histogram data
	QVector<double> m_adblData;

	//! uncorrected event counter
	quint64 m_ullTotalCounts;

	//! corrected event counter
	double m_dblTotalCounts;

	//! array position of maximum value
	int m_iMaxPos;

#if 0
	//! remap point using stored orientation
	void maporientation(int iSrcX, int iSrcY, int& iDstX, int& iDstY);

	//! remap point using stored orientation
	QPoint maporientation(const QPoint& src);
#endif
};

#endif 

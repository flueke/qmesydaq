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
#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "mdefines.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QRect>

#include "libqmesydaq_global.h"

class QFile;

/**
 * \short represents a single spectrum
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT Spectrum : public QObject
{
	Q_OBJECT

	//! defines whether the spectrum should be automatically resized or not
	//! if the data points not included in the spectrum
	Q_PROPERTY(bool m_autoResize READ autoResize WRITE setAutoResize)

public:
	Spectrum(const quint16 bins = LINBINS);

	Spectrum(const Spectrum &src);

	Spectrum& operator=(const Spectrum &src);

	~Spectrum();

	bool incVal(const quint16 bin);

	bool setValue(const quint16 bin, const quint64 val);

	bool addValue(const quint16 bin, const quint64 val);

	void clear(void);

	//! \return the maximum value of the spectrum
	quint64 max();

	//! \return the first position of the maximum value of the spectrum
	quint16 maxpos(void) const;

	//! \return sum of counts in the spectrum
	quint64 getTotalCounts(void) const;

	//! \return sum of counst in the spectrum range
	quint64 getCounts(const QRectF &) const;

	float mean(float &s);

//	quint64 operator[](quint16 index) {return m_data[index];}

	/**
	 * gives the counts at the position index
	 *
	 * \param index position inside the spectrum for the required counts
	 * \return the number of neutrons
	 */
	quint64 value(const quint16 index) const;

	/*!
	   \return the width of the spectrum
	*/
	quint16	width(void) const;

	void setWidth(const quint16 w);

	/**
	 * sets the size of the spectrum to the desired size, if the size
	 * will be increased the new values will be set to zero, if the size
	 * will be decreased the values with indices above the new size will
	 * be discarded.
	 *
	 * \param  bins new size of the spectrum
	 */
	void resize(const quint16 bins);

	/*!
	 * The spectrum will be formatted. It gives back only a formatted array
	 * of the data.
	 *
	 * \return formatted spectrum as string
	 */
	QString format(void);

	//! \return auto resizing of the spectrum
	bool autoResize(void) const;

	/**
	 * sets the autoresizing capability of the spectrum
	 * \param resize
	 */
	void setAutoResize(const bool resize);

private:
	void calcFloatingMean(const quint16 bin);

	void calcMaximumPosition(const quint16 bin);

	/**
	 * checks whether the bin is inside the spectrum or not.
	 * If the autoresize is set and the bin isn't inside the size
	 * of the spectrum will be resized to the bin number.
	 *
	 * \param bin requested bin
	 */
	bool checkBin(const quint16 bin);

private:
	/* QVector< */quint64 /*> */	*m_data;

	quint64			m_maximumPos;

	quint64			m_meanCount;

	quint64			m_totalCounts;

	//! implicit ring buffer due to the change of 256 -> 0
	quint8			m_meanPos;

	//! last events
	quint16			m_floatingMean[256];

	bool			m_autoResize;

	quint16			m_width;
};

/**
 * \short represents a single time spectrum
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT TimeSpectrum : public Spectrum
{
	Q_OBJECT
public:
	TimeSpectrum(const quint64 timewidth, const quint16 bins = TIMEBINS);

	void incVal(const quint64 time);

	bool setValue(const quint64 time, const quint64 val);

	bool addValue(const quint64 time, const quint64 val);

private:
	//! width of a time slice in time units (100ns)
	quint64		m_time;
};
#endif

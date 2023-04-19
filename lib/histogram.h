/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2020 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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
#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "mdefines.h"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QRect>

#include "libqmesydaq_export.h"

#include "spectrum.h"

class QFile;

/**
 * \short represents a histogram
 *
 * This histogram object will automatically increase its size, if
 * the incVal method gives values which are not exist.
 *
 * \author Gregor Montermann <g.montermann@mesytec.com>
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class LIBQMESYDAQ_EXPORT Histogram : public QObject
{
	Q_OBJECT

	//! defines whether the histogram should be automatically resized or not
	//! if the data points not included in the histogram
	Q_PROPERTY(bool m_autoResize READ autoResize WRITE setAutoResize)
public:
	Histogram(const quint16 channels = CHANNELS, const quint16 bins = LINBINS);

	Histogram(const Histogram &src);

	Histogram& operator=(const Histogram &src);

	virtual ~Histogram();

	virtual bool incVal(const quint16 chan, const quint16 bin);

	virtual bool setValue(const quint16 chan, const quint16 bin, const quint64 val);

	virtual bool addValue(const quint16 chan, const quint16 bin, const quint64 val);

	virtual void clear(void);

	virtual quint64 getTotalCounts(void) const;

	quint64 getCounts(const QRect &r) const;

	quint64 getCounts(const QRectF &r) const;

	virtual Spectrum *spectrum(const quint16 channel);

	//! \return the x sum spectrum
	virtual Spectrum *xSumSpectrum(void);

	//! \return the y sum spectrum
	virtual Spectrum *ySumSpectrum(void);

	quint64 max(const quint16 channel) const;

	virtual quint64 max(void) const;

	//! \return the number of the first tube containing the maximum value
	virtual quint16 maxpos(void) const;

	quint16 maxpos(const quint16 channel) const;

	void getMean(float &mean, float &sigma);

	void getMean(const quint16 chan, float &mean, float &sigma);

	/*!
	 * The histogram will be formatted. It gives back only a formatted array
	 * of the data.
	 *
	 * \return formatted histogram as string
	 */
	QString format(void);

	/*!
	 * The histogram will be formatted, with a headline, column numbers, and
	 * line numbers
	 *
	 * \param headline text before the data
	 * \return formatted histogram as string
	 */
	QString format(const QString &headline);

	virtual quint64 value(const quint16 x, const quint16 y) const;

	virtual quint16	height(void) const;

	void setHeight(const quint16 h);

	//! /return the width of the histogram
	virtual quint16 width(void) const;

	void setWidth(const quint16);

	void resize(const quint16 w, const quint16 h);

	//! \return auto resizing of the spectrum
	bool autoResize(void) const;

	/**
	 * sets the autoresizing capability of the spectrum
	 * \param resize
	 */
	void setAutoResize(const bool resize);

	//! returns the minimum value in currently set ROI
	quint64	minROI(void) const;

	//! returns the minimum value in currently set ROI
	quint64 maxROI(void) const;

	void calcMinMaxInROI(const QRectF &);

	void addSlice(const quint16, const quint16, const Histogram &);

private:
	QString formatLine(const int line);

	/**
	 * Calculates the maximum position of a tube spectrum
	 *
	 * \param chan number of the tube
	 */
	void calcMaximumPosition(const quint16 chan);

	/**
	 * checks whether the ordinat x is inside the histogram or not.
	 * If the autoresize is set and the ordinate isn't inside the size
	 * of the histogram will be resized to the ordinate number.
	 *
	 * \param x requested ordinate
	 */
	bool checkX(const quint16 x);

	/**
	 * checks whether the abscissa is inside the histogram or not.
	 * If the autoresize is set and the abscissa isn't inside the size
	 * of the histogram will be resized to the abscissa number.
	 *
	 * \param y requested abscissa
	 */
	bool checkY(const quint16 y);

protected:
	//! total number of counts inside the histogram
	quint64			m_totalCounts;

	//! tube spectra list
	QHash<QPoint, int>	m_data1;

	//! all spectra
	QVector<Spectrum *>	m_data;

	//! the list of all keys for the spectra
	QList<quint16>		m_dataKeys;

	//! length of the tube
	quint16			m_height;

	//! number of tubes
	quint16			m_width;

	//! X sum spectrum
	Spectrum		*m_xSumSpectrum;

	//! Y sum spectrum
	Spectrum		*m_ySumSpectrum;

	//! number of the tube containing the histogram maximum
	quint16			m_maximumPos;

	//! automatic resize of the histogram allowed or not
	bool			m_autoResize;

	//! minimum value in ROI
	quint64			m_minROI;

	//! maximum value in ROI
	quint64			m_maxROI;
};

#endif

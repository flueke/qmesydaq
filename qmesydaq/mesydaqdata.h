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

#ifndef MESYDAQ_DATA_H
#define MESYDAQ_DATA_H

#include <qwt_data.h>
#include <qwt_raster_data.h>
#include <qwt_plot_spectrogram.h>
#include "spectrum.h"
#include "histogram.h"

/**
 * \class MesydaqSpectrumData
 *
 * \short wrapper class to display spectrum data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de
 */
class MesydaqSpectrumData : public QwtData
{
public:
	//! constructor
	MesydaqSpectrumData();

	//! \return a deep copy of the spectrum data
	virtual QwtData *copy() const;

	//! \return the length of the spectrum
	virtual size_t size() const; 

	/*! 
	    \param i cell number
	    \return the cell number of the spectrum
	 */
	virtual double x(size_t i) const;

	/*!
	    \param i cell number 
	    \return the counts of the cell i in spectrum
	 */
	virtual double y(size_t i) const;

	/**
	 * takes the data from the spectrum
	 * the data pointed by data will not copied.
	 *
	 * \param data pointer to the data spectrum
	 */
	void setData(Spectrum *data);

	//! \return the maximum value for the spectrum
	quint32 max(void);

	//! \return the bounding rectangle of the data.
	QwtDoubleRect boundingRect() const;

private:
	//! The spectrum data
	Spectrum	m_spectrum;
};

/**
 * \short wrapper class to display histogram data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de
 */
class MesydaqHistogramData : public QwtRasterData
{
public:
	//! constructor
	MesydaqHistogramData();

	//! \return a deep copy of the histogram data
	QwtRasterData *copy() const;

	//! \return the interval between min count and maximum count
	QwtDoubleInterval range(void) const;

	virtual void setRange(const QwtDoubleInterval &range);

	/**
	 * gets the value of the cell x, y if exist, otherwise 0,
	 * the x and y values will be treated as integer values.
	 *
	 * \param x bin inside a spectrum (truncated to integer)
	 * \param y number of the tube (truncated to integer)
	 * \return counts
	 */
	double value(double x, double y) const;

	/**
	 * takes the data from the histogram
	 * the data pointed by data will not copied.
	 *
	 * \param data pointer to the data histogram
	 */
	void setData(Histogram *data);

	virtual void initRaster(const QRectF &, const QSize &);

	QSize rasterHint(const QRectF &) const;
	
private:
	//! The histogram data
	Histogram		m_histogram;

	QwtDoubleInterval	m_range;
};

/**
 * \short wrapper class to display spectrogram data
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de
 */
class MesydaqPlotSpectrogram : public QwtPlotSpectrogram
{
public:
	/*!
             constructor
          
	     \param title
         */
	MesydaqPlotSpectrogram(const QString &title = QString::null)
		: QwtPlotSpectrogram(title)
	{
	}

protected:
	QImage renderImage(const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &area) const;
};

#endif

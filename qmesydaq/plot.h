/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2012 by Jens Kr�ger <jens.krueger@frm2.tum.de>     *
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

#ifndef _PLOT_H_
#define _PLOT_H_

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class QwtPlotCurve;
class QwtPlotSpectrogram;
class QwtLinearColorMap;
class QwtScaleWidget;

class Zoomer;

class SpectrumData;
class HistogramData;

/**
 * \short The curve to display a spectrum 
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class SpectrumCurve : public QwtPlotCurve
{
public:
	//! default constructor
	SpectrumCurve();
	
	/*!
	 * Constructor
	 *	 
	 * \param p pen for the curve 
	 * \param s curve name
	 */
	SpectrumCurve(const QPen &p, const QString &s = "");
};

/**
 * \short The class to display the curves, histograms, ... 
 *
 * \author Jens Kr&uuml;ger <jens.krueger@frm2.tum.de>
 */
class Plot : public QwtPlot
{
	Q_OBJECT

public:
	//! what should be displayed ?
	enum Mode
	{
		None	= 0,	//!< self explaining :-)
		Spectrum,	//!< spectra
		Histogram,	//!< histogram
		Diffractogram, 	//!< diffractogram
	};

	//! how should the y scale be displayed
	enum Scale
	{
		Linear = 1,	//!< linear scaled
		Logarithmic,	//!< logarithmic scaled
	};

	//! what should be displayed
	enum What
	{
		Position = 1,	//!< position data
		Amplitude,	//!< amplitude data
		CorrPosition, 	//!< corrected position data
	};

	/*!
	 * Constructor
	 *
	 * \param parent parent widget
	 */
	Plot(QWidget *parent = NULL);

	//! returns the the current display mode
	Mode displayMode(void) const
	{
		return m_mode;
	}

	/*!
	 * Sets the display of the plot
	 * 
	 * \param m display mode
	 */
	void setDisplayMode(const Mode &m);

	/*!
	 * sets the data for the spectrum data
	 *
	 * \param data spectrum data
	 */
	void setSpectrumData(SpectrumData *data);

	/*!
	 * Sets the data for the histogram data
	 *
	 * \param data histogram data
	 */
	void setHistogramData(HistogramData *);

public slots:
	/*!
	 * Sets the lin/log scaling
	 *
	 * \param log if true set the logarithmic plot 
	 */
	void 	setLinLog(const bool log);

	/*!
	 * replot the widget
	 */
	void	replot(void);

private:
	//! the zoomer object
	Zoomer 			*m_zoomer;

	//! the curves 
	QwtPlotCurve 		*m_curve[8];

	//! the histogram
	QwtPlotSpectrogram	*m_histogram;

	//! data for the spectrum
	SpectrumData		*m_spectrumData;
	
	//! data for the histogram
	HistogramData		*m_histogramData;

	//! color map for the linear scaling
	QwtLinearColorMap	*m_linColorMap;

	//! color map for the logarithmic scaling
	QwtLinearColorMap	*m_logColorMap;

	//! the right axis 
	QwtScaleWidget		*m_rightAxis;

	//! display mode
	enum Mode		m_mode;

	//! lin/log scaling
	bool			m_linlog;
};

#endif

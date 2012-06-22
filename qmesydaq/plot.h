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

class Zoomer;
class QwtPlotPanner;
class QwtPlotCurve;
class QwtPlotSpectrogram;
class QwtLinearColorMap;
class QwtScaleWidget;
class SpectrumData;
class HistogramData;

class SpectrumCurve : public QwtPlotCurve
{
public:
	SpectrumCurve();
	
	SpectrumCurve(const QPen&, const QString & = "");
};

class Plot : public QwtPlot
{
	Q_OBJECT

public:
	enum Mode
	{
		None	= 0,
		Spectrum,
		Histogram,
		Diffractogram, 
	};

	enum Scale
	{
		Linear = 1,
		Logarithmic,
	};

	enum What
	{
		Position = 1,
		Amplitude,
	};

	Plot(QWidget * = NULL);

	Mode displayMode(void) const
	{
		return m_mode;
	}

	void setDisplayMode(const Mode &m);

	void setSpectrumData(SpectrumData *);

	void setHistogramData(HistogramData *);

public slots:
	void 	setLinLog(const bool);

	void	replot(void);

private:
	Zoomer 			*m_zoomer;

	QwtPlotPanner		*m_panner;

	QwtPlotCurve 		*m_curve[8];

	QwtPlotSpectrogram	*m_histogram;

	SpectrumData		*m_spectrumData;

	HistogramData		*m_histogramData;

	QwtLinearColorMap	*m_linColorMap;

	QwtLinearColorMap	*m_logColorMap;

	QwtScaleWidget		*m_rightAxis;

	enum Mode		m_mode;

	bool			m_linlog;
};

#endif

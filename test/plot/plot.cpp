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

#include "plot.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>

#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include "data.h"
#include "zoomer.h"

#include <QDebug>

Plot::Plot(QWidget *parent)
	: QwtPlot(parent)
	, m_zoomer(NULL)
	, m_panner(NULL)
	, m_curve(NULL)
	, m_histogram(NULL)
	, m_spectrumData(NULL)
	, m_histogramData(NULL)
	, m_linColorMap(NULL)
	, m_logColorMap(NULL)
	, m_mode(None)
{
	m_linColorMap = new QwtLinearColorMap(Qt::darkBlue, Qt::darkRed);
	m_linColorMap->addColorStop(0.143, Qt::blue);
	m_linColorMap->addColorStop(0.286, Qt::darkCyan);
	m_linColorMap->addColorStop(0.429, Qt::cyan);
	m_linColorMap->addColorStop(0.572, Qt::green);
	m_linColorMap->addColorStop(0.715, Qt::yellow);
	m_linColorMap->addColorStop(0.858, Qt::red);
	
	m_logColorMap = new QwtLinearColorMap(Qt::darkBlue, Qt::darkRed);
	m_logColorMap->addColorStop(0.1585, Qt::blue);
	m_logColorMap->addColorStop(0.2511, Qt::green);
	m_logColorMap->addColorStop(0.3981, Qt::yellow);
	m_logColorMap->addColorStop(0.631, Qt::red);
    
	m_rightAxis = axisWidget(QwtPlot::yRight);
	m_rightAxis->setTitle("counts");
	m_rightAxis->setColorBarEnabled(true);

	setAxisAutoScale(QwtPlot::yLeft);
	setAxisAutoScale(QwtPlot::xBottom);

#if 0
	m_panner = new QwtPlotPanner(canvas());
	m_panner->setAxisEnabled(QwtPlot::yRight, false);
	m_panner->setMouseButton(Qt::MidButton);
#endif

	plotLayout()->setAlignCanvasToScales(true);

// Insert new curves
	m_curve = new QwtPlotCurve("spectrum");
	m_curve->setPen(QPen(Qt::red));

	m_curve->setRenderHint(QwtPlotItem::RenderAntialiased);

	m_histogram = new QwtPlotSpectrogram("histogram");
	m_histogram->setColorMap(*m_linColorMap);

// Create spectrum data
	m_spectrumData = new SpectrumData(::sin, 100);
// Create histogram data
	m_histogramData = new HistogramData();

	setDisplayMode(Histogram);
}

void Plot::setLinLog(const bool)
{
}

void Plot::setDisplayMode(const Mode &m)
{
	if (m_mode ==  m)
		return;
	m_mode = m;
	m_curve->detach();
	m_histogram->detach();
	QwtDoubleRect 	r;
	switch (m_mode)
	{
		case Spectrum:
			enableAxis(QwtPlot::yRight, false);
			setAxisTitle(xBottom, "channel");
			setAxisTitle(yLeft, "counts");
			m_curve->attach(this);
			r = m_spectrumData->boundingRect();
			m_curve->setData(*m_spectrumData);
			break;
		case Histogram:
			enableAxis(QwtPlot::yRight);
			setAxisTitle(xBottom, "tube");
			setAxisTitle(yLeft, "channel");
			m_histogram->attach(this);
			r = m_histogramData->boundingRect();
			m_histogram->setData(*m_histogramData);
			break;
		default:
			break;
	}
	setAxisAutoScale(QwtPlot::xBottom);
	setAxisAutoScale(QwtPlot::yLeft);
	if (m_zoomer)
		delete m_zoomer;
	m_zoomer = new Zoomer(canvas());
}

void Plot::replot(void) 
{
	switch(m_mode)
	{
		case Histogram:
			{
				QwtDoubleInterval r = m_histogram->data().range();
				m_rightAxis->setColorMap(r, m_histogram->colorMap());
				setAxisScale(QwtPlot::yRight, r.minValue(), r.maxValue());
			}
			break;
		default :
			break;
	}
	QwtPlot::replot();
}

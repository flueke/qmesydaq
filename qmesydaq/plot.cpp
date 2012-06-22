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

#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include <qwt_scale_engine.h>

#include "colormaps.h"
#include "data.h"
#include "zoomer.h"

#include <QDebug>

SpectrumCurve:: SpectrumCurve()
	: QwtPlotCurve("")
{
        setStyle(QwtPlotCurve::Steps);
        setRenderHint(QwtPlotItem::RenderAntialiased);
}

SpectrumCurve::SpectrumCurve(const QPen &p, const QString &s)
	: QwtPlotCurve(s)
{
        setStyle(QwtPlotCurve::Steps);
        setRenderHint(QwtPlotItem::RenderAntialiased);
	setPen(p);
}

Plot::Plot(QWidget *parent)
	: QwtPlot(parent)
	, m_zoomer(NULL)
	, m_panner(NULL)
	, m_histogram(NULL)
	, m_spectrumData(NULL)
	, m_histogramData(NULL)
	, m_linColorMap(NULL)
	, m_logColorMap(NULL)
	, m_mode(None)
	, m_linlog(false)
{
	m_linColorMap = new StdLinColorMap();
	
	m_logColorMap = new StdLogColorMap();

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

	QPen p[] = {	QPen(Qt::red)
			, QPen(Qt::black)
			, QPen(Qt::green)
			, QPen(Qt::blue)
			, QPen(Qt::yellow)
			, QPen(Qt::magenta)
			, QPen(Qt::cyan)
			, QPen(Qt::white)
			};

// Insert new curves
	for (int i = 0; i < 8; ++i)
		m_curve[i] = new SpectrumCurve(p[i], QString("spectrum_%1").arg(i));

	m_histogram = new QwtPlotSpectrogram("histogram");
	m_histogram->setColorMap(*m_linColorMap);
}

void Plot::setSpectrumData(SpectrumData *data)
{
	if (data)
	{
		m_spectrumData = data;
		m_curve[0]->setData(*m_spectrumData);
	}
}

void Plot::setHistogramData(HistogramData *data)
{
	if (data)
	{
		m_histogramData = data;
		m_histogram->setData(*m_histogramData);
	}
}

void Plot::setLinLog(const bool log)
{
	m_linlog = log;
	switch (m_mode)
	{
		case Histogram :
			m_histogram->setColorMap(m_linlog ? *m_logColorMap : *m_linColorMap);
			break;
		case Spectrum :
			if (m_linlog)
            			setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
			else 
            			setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
			break;
		default :
			break;
	}
	replot();
}

void Plot::setDisplayMode(const Mode &m)
{
	if (m_mode ==  m)
		return;
	m_mode = m;
	m_curve[0]->detach();
	m_histogram->detach();
	switch (m_mode)
	{
		case Spectrum:
			enableAxis(QwtPlot::yRight, false);
			if (m_linlog)
            			setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
			else 
            			setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
			setAxisTitle(xBottom, "channel");
			setAxisTitle(yLeft, "counts");
			m_curve[0]->attach(this);
			break;
		case Histogram:
			enableAxis(QwtPlot::yRight);
            		setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
			m_histogram->setColorMap(m_linlog ? *m_logColorMap : *m_linColorMap);
			setAxisTitle(xBottom, "tube");
			setAxisTitle(yLeft, "channel");
			m_histogram->attach(this);
			break;
		default:
			break;
	}
	setAxisAutoScale(QwtPlot::xBottom);
	setAxisAutoScale(QwtPlot::yLeft);
	if (m_zoomer)
		delete m_zoomer;
	m_zoomer = new Zoomer(canvas());
	replot();
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


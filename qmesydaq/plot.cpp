/***************************************************************************
 *   Copyright (C) 2008 by Gregor Montermann <g.montermann@mesytec.com>    *
 *   Copyright (C) 2009-2013 by Jens Krüger <jens.krueger@frm2.tum.de>     *
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

#include <qwt_plot_spectrogram.h>

#include <qwt_plot_zoomer.h>

#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include <qwt_scale_engine.h>

#include "colormaps.h"
#include "data.h"
#include "mesydaqdata.h"
#include "zoomer.h"

#include "logging.h"

#include <QResizeEvent>

SpectrumCurve:: SpectrumCurve()
	: QwtPlotCurve("")
{
        setStyle(QwtPlotCurve::Steps);
        setCurveAttribute(QwtPlotCurve::Inverted, true);
        setRenderHint(QwtPlotItem::RenderAntialiased);
}

SpectrumCurve::SpectrumCurve(const QPen &p, const QString &s)
	: QwtPlotCurve(s)
{
        setStyle(QwtPlotCurve::Steps);
        setCurveAttribute(QwtPlotCurve::Inverted, true);
        setRenderHint(QwtPlotItem::RenderAntialiased);
	setPen(p);
}

Plot::Plot(QWidget *parent)
	: QwtPlot(parent)
	, m_zoomer(NULL)
	, m_histogram(NULL)
	, m_colorMap(NULL)
	, m_mode(None)
	, m_linlog(Linear)
	, m_thresholds(QwtDoubleInterval(0., 0.))
{
	setColorMap(new JetColorMap());

	m_rightAxis = axisWidget(QwtPlot::yRight);
	m_rightAxis->setTitle("counts");
	m_rightAxis->setColorBarEnabled(true);

	setAxisAutoScale(QwtPlot::yLeft);
	setAxisAutoScale(QwtPlot::xBottom);

	plotLayout()->setAlignCanvasToScales(true);

// Standard colors for the curves
	QPen p[] = {	QPen(Qt::red),
			QPen(Qt::black),
			QPen(Qt::green),
			QPen(Qt::blue),
			QPen(Qt::yellow),
			QPen(Qt::magenta),
			QPen(Qt::cyan),
			QPen(Qt::white),
			};

// Insert new curves
	for (int i = 0; i < 8; ++i)
		m_curve[i] = new SpectrumCurve(p[i], QString("spectrum_%1").arg(i));

	m_histogram = new QwtPlotSpectrogram("histogram");

	m_xSumCurve = new SpectrumCurve(QPen(Qt::black), QString("x sum"));
	m_ySumCurve = new SpectrumCurve(QPen(Qt::black), QString("y sum"));

        setDisplayMode(Histogram);
	setLinLog(Linear);
}

void Plot::setSpectrumData(SpectrumData *data, int curve)
{
	if (data)
	{
		m_curve[curve]->setData(*data);
		if (m_zoomer && !m_zoomer->zoomRectIndex())
		{
			QRectF r = m_curve[curve]->boundingRect();
			setAxisScale(QwtPlot::xBottom, 0, r.width());
		}
		replot();
	}
	else
		m_curve[curve]->detach();
}

void Plot::setSpectrumData(MesydaqSpectrumData *data, int curve)
{
	if (data)
	{
		m_curve[curve]->setData(*data);
		if (m_zoomer && !m_zoomer->zoomRectIndex())
		{
			QRectF r = m_curve[curve]->boundingRect();
			setAxisScale(QwtPlot::xBottom, 0, r.width());
		}
		replot();
	}
	else
		m_curve[curve]->detach();
}

void Plot::setHistogramData(HistogramData *data)
{
	if (data)
	{
		m_histogram->setData(*data);
		if (m_zoomer && !m_zoomer->zoomRectIndex())
		{
			QRectF r = m_histogram->boundingRect();
			setAxisScale(QwtPlot::xBottom, 0, r.width());
			setAxisScale(QwtPlot::yLeft, 0, r.height());
		}
		replot();
	}
}

void Plot::setHistogramData(MesydaqHistogramData *data)
{
	if (data)
	{
		data->setRange(m_thresholds);
		m_histogram->setData(*data);
		if (m_zoomer && !m_zoomer->zoomRectIndex())
		{
			QRectF r = m_histogram->boundingRect();
			setAxisScale(QwtPlot::xBottom, 0, r.width());
			setAxisScale(QwtPlot::yLeft, 0, r.height());
			QwtDoubleInterval iv = data->range();
			// MSG_ERROR << tr("%1, %2").arg(iv.minValue()).arg(iv.maxValue());
			setAxisScale(QwtPlot::yRight, iv.minValue(), iv.maxValue());
		}
		replot();
	}
}

void Plot::setLinLog(const enum Scale log)
{
	m_linlog = log;
	switch (m_mode)
	{
		case Histogram :
			switch (m_linlog)
			{
				case Logarithmic :
            				setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
					m_colorMap->setLogarithmicScaling();
					m_histogram->setColorMap(*m_colorMap);
					break;
				case Linear :
				default:
            				setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);
					m_colorMap->setLinearScaling();
					m_histogram->setColorMap(*m_colorMap);
					break;
			}
			break;
		case Diffractogram:
		case SingleSpectrum:
		case Spectrum :
			switch (m_linlog)
			{
				case Logarithmic:
            				setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
					break;
				case Linear :
				default:
            				setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
					break;
			}
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
	for (int i = 0; i < 8; ++i)
		m_curve[i]->detach();
	m_histogram->detach();

	QColor 	c = QColor(Qt::black);
	switch (m_mode)
	{
		case Diffractogram:
			enableAxis(QwtPlot::yRight, false);
			switch(m_linlog)
			{
				case Logarithmic :
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
					break;
				case Linear :
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
					break;
			}
			setAxisTitle(QwtPlot::xBottom, "tube");
			setAxisTitle(QwtPlot::yLeft, "counts");
			m_curve[0]->attach(this);
			break;
		case SingleSpectrum:
			enableAxis(QwtPlot::yRight, false);
			switch (m_linlog)
			{
				case Logarithmic :
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
					break;
				case Linear :
				default:
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
					break;
			}
			setAxisTitle(QwtPlot::xBottom, "tube");
			setAxisTitle(QwtPlot::yLeft, "counts");
			m_curve[0]->attach(this);
			break;
		case ModuleSpectrum:
			for (int i = 1; i < 8; ++i)
				m_curve[i]->attach(this);
		case Spectrum:
			enableAxis(QwtPlot::yRight, false);
			switch (m_linlog)
			{
				case Logarithmic :
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
					break;
				case Linear :
				default:
					setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
					break;
			}
			setAxisTitle(QwtPlot::xBottom, "channel");
			setAxisTitle(QwtPlot::yLeft, "counts");
			m_curve[0]->attach(this);
			break;
		case Histogram:
			enableAxis(QwtPlot::yRight);
            		setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
			switch (m_linlog)
			{
				case Logarithmic :
            				setAxisScaleEngine(QwtPlot::yRight, new QwtLog10ScaleEngine);
					m_colorMap->setLogarithmicScaling();
					m_histogram->setColorMap(*m_colorMap);
					break;
				case Linear :
				default:
            				setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);
					m_colorMap->setLinearScaling();
					m_histogram->setColorMap(*m_colorMap);
					break;
			}
			setAxisTitle(QwtPlot::xBottom, "tube");
			setAxisTitle(QwtPlot::yLeft, "channel");
			m_histogram->attach(this);
			c = QColor(Qt::white);
			break;
		default:
			break;
	}
	setAxisAutoScale(QwtPlot::xBottom);
	setAxisAutoScale(QwtPlot::yLeft);
	replot();
	setZoomer(c);
}

void Plot::replot(void) 
{
	switch(m_mode)
	{
		case Histogram:
			{
				QwtDoubleInterval r = m_histogram->data().range();
				if (m_linlog && r.minValue() < 1)
					r.setMinValue(1.0);
				m_rightAxis->setColorMap(r, m_histogram->colorMap());
				setAxisScale(QwtPlot::yRight, r.minValue(), r.maxValue());
			}
			break;
		default :
			break;
	}
	QwtPlot::replot();
}

void Plot::setZoomer(const QColor &c)
{
	if (m_zoomer)
	{
#if QWT_VERSION >= 0x060000
		disconnect(m_zoomer, SIGNAL(selected(const QRectF &)), this, SLOT(zoomAreaSelected(const QRectF &)));
		disconnect(m_zoomer, SIGNAL(zoomed(const QRectF &)), this, SLOT(zoomed(const QRectF &)));
#else
		disconnect(m_zoomer, SIGNAL(selected(const QwtDoubleRect &)), this, SLOT(zoomAreaSelected(const QwtDoubleRect &)));
		disconnect(m_zoomer, SIGNAL(zoomed(const QwtDoubleRect &)), this, SLOT(zoomed(const QwtDoubleRect &)));
#endif
		delete m_zoomer;
	}
	replot();
	m_zoomer = new Zoomer(canvas());
        m_zoomer->setColor(c);

#if QWT_VERSION >= 0x060000
	connect(m_zoomer, SIGNAL(selected(const QRectF &)), this, SLOT(zoomAreaSelected(const QRectF &)));
	connect(m_zoomer, SIGNAL(zoomed(const QRectF &)), this, SLOT(zoomed(const QRectF &)));
#else
	connect(m_zoomer, SIGNAL(selected(const QwtDoubleRect &)), this, SLOT(zoomAreaSelected(const QwtDoubleRect &)));
	connect(m_zoomer, SIGNAL(zoomed(const QwtDoubleRect &)), this, SLOT(zoomed(const QwtDoubleRect &)));
#endif
}

void Plot::zoomed(const QRectF &rect)
{
	if (m_zoomer && !m_zoomer->zoomRectIndex())
	{
		QRectF r;
		switch(m_mode)
		{
			default : 
				setAxisAutoScale(QwtPlot::yLeft);
				r = m_curve[0]->boundingRect();
				setAxisScale(QwtPlot::xBottom, 0, r.width());
				break;
			case Histogram :
				r = m_histogram->boundingRect();
				setAxisScale(QwtPlot::xBottom, 0, r.width());
				setAxisScale(QwtPlot::yLeft, 0, r.height());
				break;
		}
	}
	replot(); 
#if QWT_VERSION >= 0x060000
	emit zoom(rect);
#else
	emit zoom(QwtDoubleRect(rect));
#endif
}

/*!
    \fn void Plot::zoomAreaSelected(const QRectF &)

    callback for the zoomer area 
*/
void Plot::zoomAreaSelected(const QRectF &)
{
	if (m_zoomer && !m_zoomer->zoomRectIndex())
		m_zoomer->setZoomBase();
}

void Plot::resizeEvent(QResizeEvent *e)
{
	QSize s = e->size();
	QwtPlot::resizeEvent(e);
	if (e->isAccepted())
	{
		QSize cs = canvas()->size();
//		MSG_ERROR << __PRETTY_FUNCTION__ << " " << cs;
		s = QSize((2 * s.width() - cs.width()), (2 * s.height() - cs.height()));
//		MSG_ERROR << __PRETTY_FUNCTION__ << " new size" << s;
//		QwtPlot::resize(s);
	}
}

int Plot::heightForWidth(int /* w */) const
{
//	MSG_ERROR << __PRETTY_FUNCTION__ << " " << w << " " << canvas()->size();
	return -1;
}

void Plot::print(QPaintDevice &printer, const QwtPlotPrintFilter &filter)
{
	QPen pen = m_curve[0]->pen();
	pen.setWidth(1);
	m_curve[0]->setPen(pen);
	QwtPlot::print(printer, filter);
	pen.setWidth(0);
	m_curve[0]->setPen(pen);
}

void Plot::setColorMap(MesydaqColorMap *map)
{
	m_colorMap = map;
}

void Plot::setThresholds(const QwtDoubleInterval &iv)
{
	MSG_DEBUG << iv.minValue() << " " << iv.maxValue();
	m_thresholds = iv;
}

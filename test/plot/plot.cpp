#include "plot.h"

#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>

#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>

#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_layout.h>

#include "data.h"

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

	m_zoomer = new QwtPlotZoomer(canvas());
	m_zoomer->setSelectionFlags(QwtPicker::DragSelection | QwtPicker::CornerToCorner);
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
	m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

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
	switch (m_mode)
	{
		case Spectrum:
			enableAxis(QwtPlot::yRight, false);
			setAxisTitle(xBottom, "channel");
			setAxisTitle(yLeft, "counts");
			m_curve->attach(this);
			m_curve->setData(*m_spectrumData);
			break;
		case Histogram:
			enableAxis(QwtPlot::yRight);
			setAxisTitle(xBottom, "tube");
			setAxisTitle(yLeft, "channel");
			m_histogram->attach(this);
			m_histogram->setData(*m_histogramData);
			break;
		default:
			break;
	}
	m_zoomer->setZoomBase();
}

void Plot::replot(void) 
{
	qDebug() << __FUNCTION__;
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

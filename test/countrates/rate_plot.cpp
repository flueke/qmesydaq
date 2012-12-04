#include <cstdlib>

#include <QPainter>
#include <QPaintEvent>

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include "rate_plot.h"

//
//  Initialize main window
//
RatePlot::RatePlot(QWidget *parent)
	: QwtPlot(parent)
	, m_x(NULL)
	, m_y(NULL)
	, m_width(0)
	, m_interval(0)
	, m_timerId(-1)
	, m_lastVal(0.0)
{
// Disable polygon clipping
	QwtPainter::setDeviceClipping(false);

// We don't need the cache here
	canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, false);
	canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, false);

#if QT_VERSION >= 0x040000
#ifdef Q_WS_X11
    /*
       Qt::WA_PaintOnScreen is only supported for X11, but leads
       to substantial bugs with Qt 4.2.x/Windows
     */
	canvas()->setAttribute(Qt::WA_PaintOnScreen, true);
#endif
#endif

	alignScales();

	QwtPlotCurve *cLeft = new QwtPlotCurve("Data Moving Left");
	cLeft->attach(this);

// Set curve style
	cLeft->setPen(QPen(Qt::blue));

//  Initialize data
	setWidth(PLOT_SIZE);
// Attach (don't copy) data. Both curves use the same x array.
	cLeft->setRawData(m_x, m_y, PLOT_SIZE);

//  Insert min rate line at y = 0
	m_minRate = new QwtPlotMarker();
	m_minRate->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
	m_minRate->setLineStyle(QwtPlotMarker::HLine);
	m_minRate->setLinePen(QPen(Qt::red));
	m_minRate->setYValue(0.0);
	m_minRate->attach(this);

//  Insert max rate line at y = 0
	m_maxRate = new QwtPlotMarker();
	m_maxRate->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
	m_maxRate->setLineStyle(QwtPlotMarker::HLine);
	m_maxRate->setLinePen(QPen(Qt::red));
	m_maxRate->setYValue(0.0);
	m_maxRate->attach(this);

// Axis
	setAxisTitle(QwtPlot::xBottom, "Time (s)");
	setAxisScale(QwtPlot::xBottom, -100, 0);

	setAxisTitle(QwtPlot::yLeft, "cnts/s");
//	setAxisScale(QwtPlot::yLeft, -1.5, 1.5);

	setTimerInterval(500.0);
}

//
//  Set a plain canvas frame and align the scales to it
//
void RatePlot::alignScales()
{
// The code below shows how to align the scales to
// the canvas frame, but is also a good example demonstrating
// why the spreaded API needs polishing.

//	canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
	canvas()->setLineWidth(1);

	for ( int i = 0; i < QwtPlot::axisCnt; i++ )
	{
		QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
		if ( scaleWidget )
			scaleWidget->setMargin(0);
		QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
		if ( scaleDraw )
			scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
	}
}

void RatePlot::setTimerInterval(double ms)
{
	m_interval = qRound(ms);

	if (m_timerId >= 0)
	{
		killTimer(m_timerId);
		m_timerId = -1;
	}
	if (m_interval >= 0)
		m_timerId = startTimer(m_interval);
}

void RatePlot::timerEvent(QTimerEvent *)
{
// y moves from left to right:
// Shift y array right and assign new value to y[0].
	for (int j = m_width - 1; j > 0; --j)
		m_y[j] = m_y[j - 1];
	m_y[0] = m_lastVal;
	replot();
}

void RatePlot::setWidth(const quint32 w)
{
	if (m_width == w)
		return;
	m_x = (double *)realloc(m_x, sizeof(double) * w);
	m_y = (double *)realloc(m_y, sizeof(double) * w);
	for (quint32 i = m_width; i < w; ++i)
	{
		m_x[i] = -0.5 * i;
		m_y[i] = 0.0;
	}
	m_width = w;
}

void RatePlot::setValue(const double val)
{
	m_lastVal = val;
}

void RatePlot::setRateMin(const double val)
{
	m_minRate->setYValue(val);
}

void RatePlot::setRateMax(const double val)
{
	m_maxRate->setYValue(val);
}

void RatePlot::drawItems(QPainter *painter, const QRect &rect, const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const
{
//	qDebug() << __FUNCTION__;
//	painter->rotate(90);
	QwtPlot::drawItems(painter, rect, map, pfilter);
}

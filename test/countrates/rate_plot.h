#ifndef _RATE_PLOT_H
#define _RATE_PLOT_H

#include <qwt_plot.h>

class QwtPlotMarker;
class QPaintEvent;

const int PLOT_SIZE = 201;      // 0 to 200

class RatePlot : public QwtPlot
{
	Q_OBJECT

public:
	RatePlot(QWidget* = NULL);

	void setRateMin(const double val);

	void setRateMax(const double val);

public slots:
	void setTimerInterval(double interval);

	void setValue(const double val);

protected:
	virtual void timerEvent(QTimerEvent *e);

private:
	void alignScales();

	void drawItems(QPainter *painter, const QRect &rect, const QwtScaleMap map[axisCnt], const QwtPlotPrintFilter &pfilter) const;

	void setWidth(const quint32 w);

private:
	double		*m_x;

	double		*m_y;

	quint32		m_width;

	int 		m_interval; // timer in ms

	int 		m_timerId;

	QwtPlotMarker 	*m_minRate;

	QwtPlotMarker 	*m_maxRate;

	double		m_lastVal;
};

#endif

#include <qwt_plot.h>

class QwtPlotZoomer;
class QwtPlotPanner;
class QwtPlotCurve;
class QwtPlotSpectrogram;
class QwtLinearColorMap;
class QwtScaleWidget;
class SpectrumData;
class HistogramData;

class Plot : public QwtPlot
{
	Q_OBJECT

public:
	enum Mode
	{
		None	= 0,
		Spectrum,
		Histogram, 
	};

	Plot(QWidget * = NULL);

	Mode displayMode(void) const
	{
		return m_mode;
	}

	void setDisplayMode(const Mode &m);

public slots:
	void 	setLinLog(const bool);

	void	replot(void);

private:
	QwtPlotZoomer 		*m_zoomer;

	QwtPlotPanner		*m_panner;

	QwtPlotCurve 		*m_curve;

	QwtPlotSpectrogram	*m_histogram;

	SpectrumData		*m_spectrumData;

	HistogramData		*m_histogramData;

	QwtLinearColorMap	*m_linColorMap;

	QwtLinearColorMap	*m_logColorMap;

	QwtScaleWidget		*m_rightAxis;

	enum Mode		m_mode;
};


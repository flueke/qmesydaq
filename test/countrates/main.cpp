#include <QApplication>
#include <QMainWindow>

#include "rate_plot.h"
#include <cmath>
#include <cstdlib>

class MainWindow: public QMainWindow
{
public:
	MainWindow()
	{
		plot = new RatePlot(this);
		setCentralWidget(plot);

		plot->setRateMin(0.1);
		plot->setRateMax(1.9);

		startTimer(550);
	}

	virtual void timerEvent(QTimerEvent *e)
	{
		plot->setValue(generateNewValue());
	}

//  Generate new values
	double generateNewValue(void)
	{
		static double phase = 0.0;
		double val = 1 + sin(phase) * (-1.0 + 2.0 * double(rand()) / double(RAND_MAX));
#if 0
		double val = 0.8 - (2.0 * phase/M_PI) + 0.4 * double(rand()) / double(RAND_MAX);
#endif
		phase += M_PI * 0.02;
		if (phase > (M_PI - 0.0001))
			phase = 0.0;
		return val;
	}

private:
        RatePlot *plot;
};

int main(int argc, char **argv)
{
	QApplication a(argc, argv);

	MainWindow mainWindow;

	mainWindow.resize(600, 400);
	mainWindow.show();

	return a.exec();
}

#include "histogram.h"

#include <QApplication>
#include <QStringList>
#include <QTimer>

#include "qmlogging.h"
#include "measurement.h"
#include "mesydaq2.h"

#include <iostream>

void version(void)
{
	std::cout << "version : " << VERSION << std::endl;
}

extern Histogram	*m_posHist;
extern Histogram	*m_ampHist;
extern Spectrum		*m_diffractogram;

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	startLogging("[-v] histogram_file", "  -v           display version info");

	DEBUGLEVEL = WARNING;

	QStringList 	args = app.arguments();
	QString		fileName;

	if (args.size() > 1)
	{
		for (int i = 1; i < args.size(); ++i)
			if (args.at(i) == "-v")
			{
				version();
				return 1;
			}
			else
				fileName = args.at(i);

		MSG_FATAL << QObject::tr("Read file : %1 ").arg(fileName);

		Measurement *meas = new Measurement(new Mesydaq2());
		
		meas->readHistograms(fileName);

		Histogram *hp = meas->hist(Measurement::PositionHistogram);
		Histogram *h = meas->hist(Measurement::CorrectedPositionHistogram);

		MSG_ERROR << hp->width() << ", " << hp->height();
		for (int i = 0; i < 8; ++i)
			MSG_ERROR << "hp(0, " << i << ") = " << hp->value(0, i);
		for (int i = 0; i < 8; ++i)
			MSG_ERROR << "hp(" << i << ", 0) = " << hp->value(i, 0);

		MSG_ERROR << h->width() << ", " << h->height();
		for (int i = 0; i < 8; ++i)
			MSG_ERROR << "h(" << i << ", 0) = " << h->value(i, 0);
	}
	else
//		help(argv[0]);
	return 0;
}
